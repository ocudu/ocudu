// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ra_scheduler.h"
#include "../logging/scheduler_event_logger.h"
#include "../logging/scheduler_metrics_handler.h"
#include "../pdcch_scheduling/pdcch_resource_allocator_impl.h"
#include "../support/csi_rs_helpers.h"
#include "../support/dci_builder.h"
#include "../support/dmrs_helpers.h"
#include "../support/pdsch/pdsch_default_time_allocation.h"
#include "../support/pdsch/pdsch_resource_allocation.h"
#include "../support/pucch/pucch_guardbands.h"
#include "../support/rb_helper.h"
#include "../support/sch_pdu_builder.h"
#include "ocudu/adt/scope_exit.h"
#include "ocudu/ran/band_helper.h"
#include "ocudu/ran/prach/prach_preamble_information.h"
#include "ocudu/ran/prach/prach_time_mapping.h"
#include "ocudu/ran/prach/ra_helper.h"
#include "ocudu/ran/resource_allocation/resource_allocation_frequency.h"
#include "ocudu/ran/sch/tbs_calculator.h"
#include "ocudu/support/compiler.h"

using namespace ocudu;

/// Convert CRBs to VRBs.
static vrb_interval ul_crb_to_vrb(const cell_configuration& cell_cfg, crb_interval grant_crbs)
{
  return rb_helper::crb_to_vrb_ul_non_interleaved(
      grant_crbs, cell_cfg.params.ul_cfg_common.init_ul_bwp.generic_params.crbs.start());
}

static crb_interval msg3_vrb_to_crb(const cell_configuration& cell_cfg, vrb_interval grant_vrbs)
{
  return rb_helper::vrb_to_crb_ul_non_interleaved(
      grant_vrbs, cell_cfg.params.ul_cfg_common.init_ul_bwp.generic_params.crbs.start());
}

class ra_scheduler::msgb_harq_timeout_notifier final : public harq_timeout_notifier
{
public:
  msgb_harq_timeout_notifier(circular_map<uint16_t, pending_msg3_alloc>& pending_msgbs_,
                             pci_t                                       pci_,
                             ocudulog::basic_logger&                     logger_) :
    pci(pci_), logger(logger_)
  {
  }

  void on_harq_timeout(du_ue_index_t ue_idx, bool is_dl, bool ack) override
  {
    ocudu_sanity_check(is_dl, "Only DL HARQs are managed in the MsgB HARQ notifier");

    logger.debug("pci={}: MsgB HARQ timed out. Clearing HARQ entity.", pci);
  }

  void on_feedback_disabled_harq_timeout(du_ue_index_t ue_idx, bool is_dl, units::bytes tbs) override {}

private:
  const pci_t             pci;
  ocudulog::basic_logger& logger;
};

/// Notifier of HARQ process timeouts.
class ra_scheduler::msg3_harq_timeout_notifier final : public harq_timeout_notifier
{
public:
  msg3_harq_timeout_notifier(circular_map<uint16_t, pending_msg3_alloc>& pending_msg3s_,
                             pci_t                                       pci_,
                             ocudulog::basic_logger&                     logger_) :
    pending_msg3s(pending_msg3s_), pci(pci_), logger(logger_)
  {
  }

  void on_harq_timeout(du_ue_index_t ue_idx, bool is_dl, bool ack) override
  {
    ocudu_sanity_check(not is_dl, "Only UL HARQs are managed in the RA scheduler");
    auto it = pending_msg3s.find(static_cast<uint16_t>(ue_idx));
    ocudu_sanity_check(it != pending_msg3s.end(), "timeout called but HARQ entity does not exist");

    logger.warning(
        "pci={} tc-rnti={}: Discarding Msg3 retransmission HARQ process. Cause: Retransmission period timed out.",
        pci,
        it->second.preamble.tc_rnti);

    // Erase the entry to make the slot available again.
    pending_msg3s.erase(it);
  }

  void on_feedback_disabled_harq_timeout(du_ue_index_t ue_idx, bool is_dl, units::bytes tbs) override {}

private:
  circular_map<uint16_t, pending_msg3_alloc>& pending_msg3s;
  const pci_t                                 pci;
  ocudulog::basic_logger&                     logger;
};

/// Compute max Msg3 reTx timeout period, based on the fact that it should not be longer than the ConRes timer.
static unsigned get_harq_retx_timeout_slots(const cell_configuration& cell_cfg)
{
  const auto conres_timer       = cell_cfg.params.ul_cfg_common.init_ul_bwp.rach_cfg_common->ra_con_res_timer;
  const auto conres_timer_slots = conres_timer.count() * get_nof_slots_per_subframe(cell_cfg.scs_common());
  return conres_timer_slots;
}

// (Implementation-defined) limit for maximum number of concurrent Msg3s or MsgBs.
static constexpr size_t MAX_CONCURRENT_MSG3_OR_MSGB = 512;

// (Implementation-defined) limit for maximum number of pending RACH indications.
static constexpr size_t RACH_IND_QUEUE_SIZE = MAX_PRACH_OCCASIONS_PER_SLOT * 2;

// (Implementation-defined) limit for maximum number of pending CRC indications.
static constexpr size_t CRC_IND_QUEUE_SIZE = MAX_PUCCH_PDUS_PER_SLOT * 2;

/// \brief Compute the PRACH occasion duration in slots from cell configuration.
///
/// Returns the number of slots from the PRACH start slot to its end slot (inclusive), using the PUSCH SCS as
/// reference. Accounts for the starting symbol offset within the slot.
static unsigned compute_prach_occasion_duration_slots(const cell_configuration& cell_cfg)
{
  const prach_configuration prach_cfg = prach_configuration_get(
      band_helper::get_freq_range(cell_cfg.band()),
      band_helper::get_duplex_mode(cell_cfg.band()),
      cell_cfg.params.ul_cfg_common.init_ul_bwp.rach_cfg_common->rach_cfg_generic.prach_config_index);
  return get_prach_duration_info(prach_cfg, cell_cfg.scs_common()).prach_length_slots;
}

/// Determine if a PRACH preamble detected in a shared RO is a 2-step RACH (MsgA) preamble.
///
/// With shared occasions, per SSB the preamble set is split as follows (see TS 38.321, 5.1.1 and TS 38.331):
///   [0,              nof_cb_preambles_per_ssb)                              → 4-step CB preambles
///   [nof_cb_preambles_per_ssb, ... + cb_preambles_per_ssb_per_shared_ro)   → 2-step CB (MsgA) preambles
///   [... + cb_preambles_per_ssb_per_shared_ro, preambles_per_ssb)          → non-CB preambles
static bool is_msga_preamble(const rach_config_common& rach_cfg, uint8_t preamble_id)
{
  if (not rach_cfg.two_step_rach_cfg.has_value()) {
    return false;
  }
  // Number of SSBs per RO, as an integer. For fractional values (< 1 SSB per RO), one SSB covers
  // multiple ROs and all preambles in the RO belong to that SSB (nof_ssbs_per_ro = 1).
  const auto     ssb_per_ro_idx  = static_cast<unsigned>(rach_cfg.nof_ssb_per_ro);
  const auto     one_idx         = static_cast<unsigned>(ssb_per_rach_occasions::one);
  const unsigned nof_ssbs_per_ro = ssb_per_ro_idx >= one_idx ? (1U << (ssb_per_ro_idx - one_idx)) : 1U;
  // Number of preambles assigned to each SSB within this RO.
  const unsigned preambles_per_ssb = rach_cfg.total_nof_ra_preambles / nof_ssbs_per_ro;
  // Preamble ID relative to the start of its SSB's preamble set.
  const unsigned local_id = preamble_id % preambles_per_ssb;
  return local_id >= rach_cfg.nof_cb_preambles_per_ssb and
         local_id < static_cast<unsigned>(rach_cfg.nof_cb_preambles_per_ssb) +
                        rach_cfg.two_step_rach_cfg->cb_preambles_per_ssb_per_shared_ro;
}

/// Generate circular map key of Msg3 grant based on its TC-RNTI.
/// \note the returned key can be larger than the circular_map size. This helps with disambiguation. However,
/// it cannot be higher than MAX_NOF_DU_UES, because the key is translated into a ue_index_t by the cell_harq_manager.
static uint16_t get_msg3_ring_key(rnti_t tc_rnti)
{
  return to_value(tc_rnti) % MAX_NOF_DU_UES;
}

/// Helper to fetch list of PDSCH time-domain resources for RA.
static span<const pdsch_time_domain_resource_allocation> get_ra_pdsch_td_list(const cell_configuration& cell_cfg)
{
  return get_ra_rnti_pdsch_time_domain_list(cell_cfg.params.dl_cfg_common.init_dl_bwp.pdsch_common,
                                            cell_cfg.params.dl_cfg_common.init_dl_bwp.generic_params.cp,
                                            cell_cfg.params.dmrs_typeA_pos);
}

/// Helper to select ra-SearchSpace config.
/// [3GPP TS 38.213, clause 10.1] a UE monitors PDCCH candidates in one or more of the following search spaces sets
///  - a Type1-PDCCH CSS set configured by ra-SearchSpace in PDCCH-ConfigCommon for a DCI format with
///    CRC scrambled by a RA-RNTI, a MsgB-RNTI, or a TC-RNTI on the primary cell.
static const search_space_configuration& get_ra_ss_cfg(const cell_configuration& cell_cfg)
{
  return cell_cfg.params.dl_cfg_common.init_dl_bwp.pdcch_common
      .search_spaces[cell_cfg.params.dl_cfg_common.init_dl_bwp.pdcch_common.ra_search_space_id];
}

/// Helper to fetch PUSCH time-domain resources for Msg3.
static span<const pusch_time_domain_resource_allocation> get_pusch_td_list(const cell_configuration& cell_cfg)
{
  return get_pusch_time_domain_resource_table(*cell_cfg.params.ul_cfg_common.init_ul_bwp.pusch_cfg_common);
}

class ra_scheduler::cached_bwp_info
{
public:
  cached_bwp_info(const cell_configuration& cfg) :
    preamble_td_mapper(cfg.band(),
                       cfg.init_bwp.ul.cfg().scs,
                       cfg.init_bwp.ul.rach_common()->rach_cfg_generic.prach_config_index)
  {
    fill_msga_pusch_info(cfg);
  }

  /// Whether a MsgA PUSCH can be scheduled in this slot.
  bool is_msga_pusch_slot(slot_point sl) const { return preamble_td_mapper.has_prach_occasion(sl - msga_td_offset); }

  /// Pre-reserved space in the resource grid for MsgA PUSCH.
  /// \note This space needs to pre-reserved in advance so that other PUSCH/PUCCH grants occupy it.
  grant_info reserved_msga_pusch_space;

private:
  prach_helper::preamble_slot_mapping preamble_td_mapper;
  unsigned                            msga_td_offset = 0;

  void fill_msga_pusch_info(const cell_configuration& cfg)
  {
    const auto& ul_bwp = cfg.init_bwp.ul;
    if (not ul_bwp.rach_common()->two_step_rach_cfg.has_value()) {
      return;
    }
    const auto&                                  msga_pusch_cfg = ul_bwp.rach_common()->two_step_rach_cfg->pusch;
    const pusch_time_domain_resource_allocation& td_alloc = get_pusch_td_list(cfg)[msga_pusch_cfg.pusch_td_res_index];
    // For MsgA, the PUSCH slot is at prach_slot + td_offset; k2 of PUSCH TD resource is irrelevant as per spec.
    msga_td_offset = msga_pusch_cfg.td_offset;

    const unsigned crb_start = ul_bwp.cfg().crbs.start() + msga_pusch_cfg.prb_start;
    reserved_msga_pusch_space =
        grant_info{ul_bwp.cfg().scs,
                   td_alloc.symbols,
                   crb_interval{crb_start, crb_start + msga_pusch_cfg.po_fdm * msga_pusch_cfg.nof_prbs_per_msgA_po}};
  }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ra_scheduler::ra_scheduler(const cell_configuration& cellcfg_,
                           pdcch_resource_allocator& pdcch_sch_,
                           scheduler_event_logger&   ev_logger_,
                           cell_metrics_handler&     metrics_hdlr_) :
  sched_cfg(cellcfg_.expert_cfg.ra),
  cell_cfg(cellcfg_),
  pdcch_sch(pdcch_sch_),
  ev_logger(ev_logger_),
  metrics_hdlr(metrics_hdlr_),
  ra_win_nof_slots(cell_cfg.params.ul_cfg_common.init_ul_bwp.rach_cfg_common->rach_cfg_generic.ra_resp_window),
  ra_crb_lims(pdsch_helper::get_ra_crb_limits_common(
      cell_cfg.params.dl_cfg_common.init_dl_bwp,
      cell_cfg.params.dl_cfg_common.init_dl_bwp.pdcch_common.ra_search_space_id)),
  cfra_preambles(ra_helper::get_cfra_preambles(*cell_cfg.params.ul_cfg_common.init_ul_bwp.rach_cfg_common)),
  prach_format_is_long(is_long_preamble(
      prach_configuration_get(
          band_helper::get_freq_range(cell_cfg.band()),
          band_helper::get_duplex_mode(cell_cfg.band()),
          cell_cfg.params.ul_cfg_common.init_ul_bwp.rach_cfg_common->rach_cfg_generic.prach_config_index)
          .format)),
  prach_occasion_duration_slots(compute_prach_occasion_duration_slots(cell_cfg)),
  pucch_crbs(compute_pucch_crbs(cell_cfg)),
  cached_init_bwp_info(std::make_unique<cached_bwp_info>(cell_cfg)),
  ra_harqs(MAX_NOF_DU_UES,
           1,
           std::make_unique<msgb_harq_timeout_notifier>(pending_msg3s, cell_cfg.params.pci, logger),
           std::make_unique<msg3_harq_timeout_notifier>(pending_msg3s, cell_cfg.params.pci, logger),
           get_harq_retx_timeout_slots(cell_cfg),
           get_harq_retx_timeout_slots(cell_cfg),
           cell_harq_manager::DEFAULT_ACK_TIMEOUT_SLOTS,
           cell_cfg.ntn_cs_koffset,
           cell_cfg.params.ntn_params.has_value() && cell_cfg.params.ntn_params->ul_harq_mode_b),
  pending_rachs(RACH_IND_QUEUE_SIZE),
  pending_crcs(CRC_IND_QUEUE_SIZE),
  pending_msg3s(MAX_CONCURRENT_MSG3_OR_MSGB),
  pending_cfra_ues(cfra_preambles.empty() ? 0 : MAX_NOF_DU_UES)
{
  // The maximum number of pending RARs is given by the maximum number of PRACH occasions that can accumulate from a
  // given UL slot (at which the PRACH is received) until the expiration of the RAR window. The worst case is when:
  // (i) the PRACH is received instantaneously by the scheduler, and the PRACH slot is the farthest possible from the
  //     beginning of the start of the RAR window.
  // (ii) RAR window is min(80 slots, 10 ms).
  // (iii) there are PRACHs occasions in every UL slot.
  // (iv) there are no suitable DL slots for scheduling the RARs (pending RARs will be in the vector until the RAR
  //      window expires).
  // [Implementation-defined] Assume 80 slots RAR window + TDD 2D1S7D with 30kHz SCS slots and
  // MAX_PRACH_OCCASIONS_PER_SLOT (the actual number would depend on the PRACH configuration index).
  static constexpr size_t MAX_PENDING_RARS_SLOTS = 90U;
  pending_rars.reserve(MAX_PRACH_OCCASIONS_PER_SLOT * MAX_PENDING_RARS_SLOTS);
  // MsgB window can be up to 320 slots (msgB-ResponseWindow-r16), so use the same bound.
  pending_msgbs.reserve(MAX_PRACH_OCCASIONS_PER_SLOT * MAX_PENDING_RARS_SLOTS);

  for (auto& cfra_ue : pending_cfra_ues) {
    cfra_ue.store(rnti_t::INVALID_RNTI, std::memory_order_relaxed);
  }

  // Precompute RAR PDSCH and DCI PDUs.
  precompute_rar_fields();

  // Precompute Msg3 PUSCH and DCI PDUs.
  precompute_msg3_pdus();
}

ra_scheduler::~ra_scheduler() = default;

void ra_scheduler::precompute_rar_fields()
{
  // RAR payload size in bytes as per TS38.321, 6.1.5 and 6.2.3.
  static constexpr unsigned rar_payload_size_bytes   = 7;
  static constexpr unsigned rar_subheader_size_bytes = 1;
  // As per TS 38.214, Section 5.1.3.2, nof_oh_prb = 0 if PDSCH is scheduled by PDCCH with a CRC scrambled by RA-RNTI.
  static constexpr unsigned nof_oh_prb = 0;
  static constexpr unsigned nof_layers = 1;

  const span<const pdsch_time_domain_resource_allocation> pdsch_td_list = get_ra_pdsch_td_list(cell_cfg);

  // Cache PDSCH DM-RS information and RAR required TBS and number of PRBs.
  rar_data.resize(pdsch_td_list.size());
  rar_mcs_config = pdsch_mcs_get_config(pdsch_mcs_table::qam64, sched_cfg.rar_mcs_index);
  for (unsigned td_idx = 0; td_idx != rar_data.size(); ++td_idx) {
    // > Compute and cache DM-RS information for different PDSCH TD indexes.
    rar_data[td_idx].dmrs_info =
        make_dmrs_info_common(pdsch_td_list, td_idx, cell_cfg.params.pci, cell_cfg.params.dmrs_typeA_pos);

    // > Compute and cache #PRBs and TBS information for different PDSCH TD indexes.
    const unsigned             nof_symb_sh = pdsch_td_list[td_idx].symbols.length();
    prbs_calculator_sch_config prbs_cfg{rar_payload_size_bytes + rar_subheader_size_bytes,
                                        nof_symb_sh,
                                        calculate_nof_dmrs_per_rb(rar_data[td_idx].dmrs_info),
                                        nof_oh_prb,
                                        rar_mcs_config,
                                        nof_layers};
    for (unsigned ngrant = 0; ngrant != MAX_RAR_PDUS_PER_SLOT; ++ngrant) {
      prbs_cfg.payload_size_bytes = (rar_payload_size_bytes + rar_subheader_size_bytes) * (ngrant + 1);
      rar_data[td_idx].prbs_tbs_per_nof_grants.push_back(get_nof_prbs(prbs_cfg));
    }
  }
}

void ra_scheduler::precompute_msg3_pdus()
{
  // Msg3 UL CCCH message size is up to 64bits (8 octets), and its subheader has 1 octet as per TS38.321.
  static constexpr unsigned max_msg3_sdu_payload_size_bytes = 8;
  static constexpr unsigned msg3_subheader_size_bytes       = 1;
  // As per TS 38.214, Section 5.1.3.2, nof_oh_prb = 0 if PDSCH is scheduled by PDCCH with a CRC scrambled by RA-RNTI.
  static constexpr unsigned nof_oh_prb = 0;
  static constexpr unsigned nof_layers = 1;

  msg3_mcs_config = pusch_mcs_get_config(
      pusch_mcs_table::qam64, sched_cfg.msg3_mcs_index, cell_cfg.use_msg3_transform_precoder(), false);

  const auto pusch_td_alloc_list = get_pusch_td_list(cell_cfg);

  // One position per time resource.
  msg3_data.resize(pusch_td_alloc_list.size());

  for (unsigned i = 0; i != msg3_data.size(); ++i) {
    // Compute the required PRBs and TBS for Msg3.
    const pusch_config_params pusch_cfg = get_pusch_config_f0_0_tc_rnti(cell_cfg, pusch_td_alloc_list[i]);
    const sch_prbs_tbs        prbs_tbs =
        get_nof_prbs(prbs_calculator_sch_config{max_msg3_sdu_payload_size_bytes + msg3_subheader_size_bytes,
                                                pusch_td_alloc_list[i].symbols.length(),
                                                calculate_nof_dmrs_per_rb(pusch_cfg.dmrs),
                                                nof_oh_prb,
                                                msg3_mcs_config,
                                                nof_layers});

    // Generate DCI and PUSCH PDUs.
    static constexpr uint8_t msg3_rv = 0;
    build_dci_f0_0_tc_rnti(msg3_data[i].dci,
                           cell_cfg.params.dl_cfg_common.init_dl_bwp,
                           cell_cfg.params.ul_cfg_common.init_ul_bwp.generic_params,
                           crb_interval{0, prbs_tbs.nof_prbs},
                           i,
                           sched_cfg.msg3_mcs_index,
                           msg3_rv);

    // Note: RNTI will be overwritten later.
    const vrb_interval vrbs = ul_crb_to_vrb(cell_cfg, crb_interval{0, prbs_tbs.nof_prbs});
    build_pusch_f0_0_tc_rnti(msg3_data[i].pusch,
                             pusch_cfg,
                             prbs_tbs.tbs_bytes,
                             to_rnti(0x4601),
                             cell_cfg,
                             msg3_data[i].dci.tc_rnti_f0_0,
                             vrbs,
                             true);
  }
}

void ra_scheduler::handle_rach_indication(const rach_indication_message& msg)
{
  // Buffer detected RACHs to be handled in next slot.
  if (not pending_rachs.try_push(msg)) {
    logger.warning(
        "pci={}: Discarding RACH indication for slot={}. Cause: Event queue is full", cell_cfg.params.pci, msg.slot_rx);
  }
}

void ra_scheduler::handle_rach_indication_impl(const rach_indication_message& msg, cell_resource_allocator& res_alloc)
{
  const rach_config_common& rach_cfg = *cell_cfg.params.ul_cfg_common.init_ul_bwp.rach_cfg_common;

  for (const auto& prach_occ : msg.occasions) {
    if (prach_occ.preambles.empty()) {
      // As per FAPI, this should not occur.
      logger.warning(
          "pci={}: Discarding PRACH occasion detected at slot={}. Cause: There are no preambles detected for this "
          "PRACH occasion",
          cell_cfg.params.pci,
          msg.slot_rx);
      continue;
    }

    span<const rach_indication_message::preamble> msg1_preambles;
    span<const rach_indication_message::preamble> msga_preambles;

    if (rach_cfg.two_step_rach_cfg.has_value()) {
      // Split detected preambles into Msg1 (4-step) and MsgA (2-step) spans. Preambles are assumed to be ordered by
      // preamble_id, so the two groups form contiguous ranges.
      const auto msga_begin = std::partition_point(
          prach_occ.preambles.begin(), prach_occ.preambles.end(), [&](const rach_indication_message::preamble& p) {
            return not is_msga_preamble(rach_cfg, p.preamble_id);
          });
      msg1_preambles = {prach_occ.preambles.begin(), msga_begin};
      msga_preambles = {msga_begin, prach_occ.preambles.end()};
    } else {
      // All preambles are 4-step RACH preambles.
      msg1_preambles = prach_occ.preambles;
    }

    if (not msg1_preambles.empty()) {
      handle_msg1_occasion(prach_occ, msg1_preambles, msg.slot_rx);
    }
    if (not msga_preambles.empty()) {
      handle_msga_occasion(prach_occ, msga_preambles, msg.slot_rx, res_alloc);
    }
  }

  // Forward RACH indication to metrics handler.
  metrics_hdlr.handle_rach_indication(msg, res_alloc.slot_tx());
}

void ra_scheduler::handle_msg1_occasion(const rach_indication_message::occasion&      occ,
                                        span<const rach_indication_message::preamble> preambles,
                                        slot_point                                    prach_slot_rx)
{
  // As per Section 5.1.3, TS 38.321, and from Section 5.3.2, TS 38.211, slot_idx uses as the numerology of reference
  // 15kHz for long PRACH Formats (i.e, slot_idx = subframe index); whereas, for short PRACH formats, it uses the same
  // numerology as the SCS common (i.e, slot_idx = actual slot index within the frame).
  const unsigned slot_idx = prach_format_is_long ? prach_slot_rx.subframe_index() : prach_slot_rx.slot_index();
  const rnti_t   ra_rnti  = ra_helper::get_ra_rnti(slot_idx, occ.start_symbol, occ.frequency_index);

  // Search for pending RAR with matching RA-RNTI and Rx Slot.
  auto               rar_it = std::find_if(pending_rars.begin(), pending_rars.end(), [&](const pending_rar_alloc& rar) {
    return rar.ra_rnti == ra_rnti and rar.prach_slot_rx == prach_slot_rx;
  });
  pending_rar_alloc* rar_req = rar_it != pending_rars.end() ? &*rar_it : nullptr;
  if (rar_req == nullptr) {
    // No match was found. Create new pending RAR.
    if (pending_rars.capacity() == pending_rars.size()) {
      logger.warning("pci={} ra-rnti={}: Discarding PRACH occasion. Cause: Pending RARs queue is full",
                     cell_cfg.params.pci,
                     ra_rnti);
      return;
    }
    pending_rars.emplace_back();
    rar_req                = &pending_rars.back();
    rar_req->ra_rnti       = ra_rnti;
    rar_req->prach_slot_rx = prach_slot_rx;
  }

  // Set RAR window. First slot after PRACH with active DL slot represents the start of the RAR window.
  if (cell_cfg.is_tdd()) {
    // TDD case.
    const unsigned period = nof_slots_per_tdd_period(*cell_cfg.params.tdd_cfg);
    for (unsigned sl_idx = 0; sl_idx != period; ++sl_idx) {
      const slot_point sl_start = rar_req->prach_slot_rx + prach_occasion_duration_slots + sl_idx;
      if (cell_cfg.is_dl_enabled(sl_start)) {
        rar_req->rar_window = {sl_start, sl_start + ra_win_nof_slots};
        break;
      }
    }
    ocudu_sanity_check(rar_req->rar_window.length() != 0, "Invalid configuration");
  } else {
    // FDD case.
    rar_req->rar_window = {rar_req->prach_slot_rx + prach_occasion_duration_slots,
                           rar_req->prach_slot_rx + prach_occasion_duration_slots + ra_win_nof_slots};
  }

  for (const auto& preamble : preambles) {
    // Log event.
    ev_logger.enqueue(scheduler_event_logger::prach_event{
        prach_slot_rx,
        cell_cfg.cell_index,
        preamble.preamble_id,
        ra_rnti,
        preamble.tc_rnti,
        preamble.time_advance.to_Ta(cell_cfg.params.ul_cfg_common.init_ul_bwp.generic_params.scs),
        false});

    // Check if TC-RNTI value to be scheduled is already under use.
    const uint16_t msg3_ring_idx = get_msg3_ring_key(preamble.tc_rnti);
    if (not pending_msg3s.emplace(msg3_ring_idx)) {
      logger.warning("pci={}: PRACH ignored, as the allocated TC-RNTI={} is already under use",
                     cell_cfg.params.pci,
                     preamble.tc_rnti);
      continue;
    }
    auto& msg3_entry = pending_msg3s[msg3_ring_idx];

    // Store TC-RNTI of the preamble.
    rar_req->tc_rntis.emplace_back(preamble.tc_rnti);

    // Store Msg3 request and create a HARQ entity of 1 UL HARQ.
    msg3_entry.preamble = preamble;
    msg3_entry.harq_ent = ra_harqs.add_ue(to_du_ue_index(msg3_ring_idx), preamble.tc_rnti, 1, 1);
  }
}

void ra_scheduler::handle_msga_occasion(const rach_indication_message::occasion&      occ,
                                        span<const rach_indication_message::preamble> preambles,
                                        slot_point                                    prach_slot_rx,
                                        cell_resource_allocator&                      res_alloc)
{
  const rach_config_common& rach_cfg = *cell_cfg.params.ul_cfg_common.init_ul_bwp.rach_cfg_common;
  ocudu_sanity_check(rach_cfg.two_step_rach_cfg.has_value(), "MsgA received but 2-step RACH is not configured");
  const rach_config_common_two_step&                    two_step_cfg   = *rach_cfg.two_step_rach_cfg;
  const bwp_uplink_common&                              ul_bwp         = cell_cfg.params.ul_cfg_common.init_ul_bwp;
  const rach_config_common_two_step::msgA_pusch_config& msga_pusch_cfg = two_step_cfg.pusch;

  // Derive MsgB-RNTI.
  const unsigned slot_idx  = prach_format_is_long ? prach_slot_rx.subframe_index() : prach_slot_rx.slot_index();
  const rnti_t   msgb_rnti = ra_helper::get_msgb_rnti(slot_idx, occ.start_symbol, occ.frequency_index);

  // Search for a pending MsgB entry matching in MsgB-RNTI and PRACH slot.
  auto msgb_it = std::find_if(pending_msgbs.begin(), pending_msgbs.end(), [&](const pending_msgb_alloc& msgb) {
    return msgb.msgb_rnti == msgb_rnti and msgb.prach_slot_rx == prach_slot_rx;
  });
  pending_msgb_alloc* msgb_req = msgb_it != pending_msgbs.end() ? &*msgb_it : nullptr;
  if (msgb_req == nullptr) {
    // No MsgB with matching MsgB-RNTI and PRACH slot exists. Create one.
    if (pending_msgbs.capacity() == pending_msgbs.size()) {
      logger.warning("pci={} msgb-rnti={}: Discarding MsgA occasion. Cause: Pending MsgBs queue is full",
                     cell_cfg.params.pci,
                     msgb_rnti);
      return;
    }
    msgb_req                = &pending_msgbs.emplace_back();
    msgb_it                 = pending_msgbs.end() - 1;
    msgb_req->msgb_rnti     = msgb_rnti;
    msgb_req->prach_slot_rx = prach_slot_rx;

    // Set MsgB response window. First slot after PRACH with active DL slot is the window start.
    if (cell_cfg.is_tdd()) {
      const unsigned period = nof_slots_per_tdd_period(*cell_cfg.params.tdd_cfg);
      for (unsigned sl_idx = 0; sl_idx < period; ++sl_idx) {
        const slot_point sl_start = prach_slot_rx + prach_occasion_duration_slots + sl_idx;
        if (cell_cfg.is_dl_enabled(sl_start)) {
          msgb_req->msgb_window = {sl_start, sl_start + two_step_cfg.msgB_response_window_slots};
          break;
        }
      }
      ocudu_sanity_check(msgb_req->msgb_window.length() != 0, "Invalid TDD configuration for MsgB window");
    } else {
      msgb_req->msgb_window = {prach_slot_rx + prach_occasion_duration_slots,
                               prach_slot_rx + prach_occasion_duration_slots + two_step_cfg.msgB_response_window_slots};
    }
  }

  // If by the end of this function, no MsgA PUSCHs were allocated, we remove the respective MsgB entry.
  auto erase_msgb_if_empty = make_scope_exit([this, &msgb_it]() {
    if (msgb_it->preambles.empty()) {
      // No MsgA PUSCHs were successfully allocated. Drop the MsgB altogether.
      pending_msgbs.erase(msgb_it);
    }
  });

  // Determine MsgA PUSCH slot.
  const slot_point pusch_slot = prach_slot_rx + msga_pusch_cfg.td_offset;
  if (pusch_slot < res_alloc.slot_tx()) {
    logger.warning("pci={} msgb-rnti={}: Discarding MsgA PUSCH. Cause: The PUSCH slot={} has already passed",
                   cell_cfg.params.pci,
                   msgb_rnti,
                   pusch_slot);
    return;
  }
  cell_slot_resource_allocator& pusch_alloc = res_alloc[pusch_slot];

  // Look up the PUSCH time-domain allocation to derive symbol range and mapping type for DMRS computation.
  // Note: k2 from the TD allocation is ignored; the actual PUSCH slot offset is given by td_offset as per
  // TS 38.331 "msgA-PUSCH-TimeDomainAllocation".
  const pusch_time_domain_resource_allocation& td_alloc =
      get_pusch_td_list(cell_cfg)[msga_pusch_cfg.pusch_td_res_index];

  // Determine whether MsgA PUSCH OFDM symbols fall in valid UL symbols.
  const unsigned start_ul_symbols = NOF_OFDM_SYM_PER_SLOT_NORMAL_CP - cell_cfg.get_nof_ul_symbol_per_slot(pusch_slot);
  if (not cell_cfg.is_ul_enabled(pusch_slot) or td_alloc.symbols.start() < start_ul_symbols) {
    logger.warning("pci={} msgb-rnti={}: Discarding MsgA PUSCH. Cause: PUSCH would fall in an invalid slot={}",
                   cell_cfg.params.pci,
                   msgb_rnti,
                   pusch_slot);
    return;
  }

  // Compute DMRS and TBS — shared by all preambles since nof_prbs_per_msgA_po is constant across FDM occasions.
  const dmrs_information    dmrs = make_dmrs_info_common(td_alloc, cell_cfg.params.pci, cell_cfg.params.dmrs_typeA_pos);
  const sch_mcs_description mcs_cfg = pusch_mcs_get_config(pusch_mcs_table::qam64, msga_pusch_cfg.mcs, false, false);
  const units::bytes        tbs     = tbs_calculator_calculate(tbs_calculator_configuration{
                 .nof_symb_sh      = td_alloc.symbols.length(),
                 .nof_dmrs_prb     = calculate_nof_dmrs_per_rb(dmrs),
                 .nof_oh_prb       = 0,
                 .mcs_descr        = mcs_cfg,
                 .nof_layers       = 1,
                 .tb_scaling_field = 0,
                 .n_prb            = msga_pusch_cfg.nof_prbs_per_msgA_po,
  });

  // Precompute the preamble-to-PUSCH-occasion index mapping. When po_fdm > 1, the CB preambles are divided evenly
  // across po_fdm FDM occasions. Preamble i (within the MsgA range) maps to occasion floor(i / preambles_per_po).
  const auto     ssb_per_ro_idx    = static_cast<unsigned>(rach_cfg.nof_ssb_per_ro);
  const auto     one_idx           = static_cast<unsigned>(ssb_per_rach_occasions::one);
  const unsigned nof_ssbs_per_ro   = ssb_per_ro_idx >= one_idx ? (1U << (ssb_per_ro_idx - one_idx)) : 1U;
  const unsigned preambles_per_ssb = rach_cfg.total_nof_ra_preambles / nof_ssbs_per_ro;
  const unsigned preambles_per_po  = two_step_cfg.cb_preambles_per_ssb_per_shared_ro / msga_pusch_cfg.po_fdm;

  for (const auto& preamble : preambles) {
    ocudu_sanity_check(is_msga_preamble(rach_cfg, preamble.preamble_id),
                       "Handling preamble that is not for MsgA. Are preamble IDs sorted in the RACH indication?");
    ev_logger.enqueue(scheduler_event_logger::prach_event{
        prach_slot_rx,
        cell_cfg.cell_index,
        preamble.preamble_id,
        msgb_rnti,
        preamble.tc_rnti,
        preamble.time_advance.to_Ta(cell_cfg.params.ul_cfg_common.init_ul_bwp.generic_params.scs),
        true});
    if (msgb_req->preambles.full()) {
      logger.warning("pci={} msgb-rnti={}: Discarding MsgA preamble id={}. Cause: MsgB preamble list is full",
                     cell_cfg.params.pci,
                     msgb_rnti,
                     preamble.preamble_id);
      continue;
    }

    // Determine this preamble's PUSCH occasion index and its CRB allocation.
    const unsigned msga_local_id = (preamble.preamble_id % preambles_per_ssb) - rach_cfg.nof_cb_preambles_per_ssb;
    const unsigned po_idx        = msga_local_id / preambles_per_po;
    const unsigned crb_start =
        ul_bwp.generic_params.crbs.start() + msga_pusch_cfg.prb_start + po_idx * msga_pusch_cfg.nof_prbs_per_msgA_po;
    const crb_interval preamble_crbs{crb_start, crb_start + msga_pusch_cfg.nof_prbs_per_msgA_po};

    // Note: PUSCH resources have been pre-reserved.
    ocudu_sanity_check(
        pusch_alloc.ul_res_grid.all_set(grant_info{ul_bwp.generic_params.scs, td_alloc.symbols, preamble_crbs}),
        "Invalid prereservation of MsgA PUSCH resources");

    if (pusch_alloc.result.ul.puschs.full()) {
      logger.warning(
          "pci={} msgb-rnti={} tc-rnti={}: Discarding MsgA PUSCH. Cause: PUSCH grant list is full for slot={}",
          cell_cfg.params.pci,
          msgb_rnti,
          preamble.tc_rnti,
          pusch_slot);
      continue;
    }

    // Fill scheduling result.
    ul_sched_info& ul_info     = pusch_alloc.result.ul.puschs.emplace_back();
    ul_info.context.ue_index   = INVALID_DU_UE_INDEX;
    ul_info.context.ss_id      = cell_cfg.params.dl_cfg_common.init_dl_bwp.pdcch_common.ra_search_space_id;
    ul_info.context.k2         = msga_pusch_cfg.td_offset;
    ul_info.context.nof_retxs  = 0;
    ul_info.context.nof_oh_prb = 0;

    pusch_information& pusch      = ul_info.pusch_cfg;
    pusch.rnti                    = preamble.tc_rnti;
    pusch.bwp_cfg                 = &ul_bwp.generic_params;
    pusch.rbs                     = ul_crb_to_vrb(cell_cfg, preamble_crbs);
    pusch.symbols                 = td_alloc.symbols;
    pusch.mcs_table               = pusch_mcs_table::qam64;
    pusch.mcs_index               = msga_pusch_cfg.mcs;
    pusch.mcs_descr               = mcs_cfg;
    pusch.nof_layers              = 1;
    pusch.dmrs                    = dmrs;
    pusch.n_id                    = cell_cfg.params.pci;
    pusch.pusch_dmrs_id           = cell_cfg.params.pci;
    pusch.rv_index                = 0;
    pusch.new_data                = true;
    pusch.harq_id                 = to_harq_id(0);
    pusch.tb_size_bytes           = tbs;
    pusch.num_cb                  = 0;
    pusch.transform_precoding     = false;
    pusch.intra_slot_freq_hopping = false;
    pusch.tx_direct_current_location =
        dc_offset_helper::pack(cell_cfg.expert_cfg.ue.initial_ul_dc_offset, cell_cfg.nof_ul_prbs);
    pusch.ul_freq_shift_7p5khz = false;
    pusch.dmrs_hopping_mode    = pusch_information::dmrs_hopping_mode::no_hopping;

    // MsgA PUSCH successfully allocated. We will register it in the pending MsgB.
    msgb_req->preambles.emplace_back(preamble);
  }
}

void ra_scheduler::handle_crc_indication(const ul_crc_indication& crc_ind)
{
  // Filter out CRCs that are not associated with the CBRA or CFRA.
  // Note: Only HARQ-ID=0 is relevant for the RA procedure.
  // Note: UEs on CBRA have no ue_index assigned, but CFRA UEs do. We determine that a CRC is for a CFRA by checking
  // if its RNTI is in the pending_cfra_ues map.
  auto is_ra_crc = [this](const ul_crc_pdu_indication& pdu) {
    return pdu.harq_id == to_harq_id(0) and
           (pdu.ue_index == INVALID_DU_UE_INDEX or
            (not pending_cfra_ues.empty() and
             pending_cfra_ues[pdu.ue_index].load(std::memory_order_acquire) == pdu.rnti));
  };
  ul_crc_indication ra_crc_ind;
  for (auto& crc : crc_ind.crcs) {
    if (is_ra_crc(crc)) {
      ra_crc_ind.crcs.push_back(crc);
    }
  }
  if (ra_crc_ind.crcs.empty()) {
    // Early exit: No RA CRCs found.
    return;
  }
  ra_crc_ind.sl_rx      = crc_ind.sl_rx;
  ra_crc_ind.cell_index = crc_ind.cell_index;

  if (not pending_crcs.try_push(ra_crc_ind)) {
    logger.warning(
        "pci={}: CRC indication for slot={} discarded. Cause: Event queue is full", cell_cfg.params.pci, crc_ind.sl_rx);
  }
}

void ra_scheduler::handle_cfra_mapping_update(du_ue_index_t ue_index, rnti_t crnti)
{
  ocudu_assert(not pending_cfra_ues.empty(), "RACH config does not support CFRA UEs");
  pending_cfra_ues[ue_index].store(crnti, std::memory_order_relaxed);
}

void ra_scheduler::handle_pending_crc_indications_impl(cell_resource_allocator& res_alloc)
{
  // Helper to mark MsgA PUSCH CRC outcome in pending_msgbs.
  auto mark_msga_crc = [this](rnti_t tc_rnti, bool success) {
    for (auto& msgb : pending_msgbs) {
      for (auto& p : msgb.preambles) {
        if (p.info.tc_rnti == tc_rnti) {
          p.crc_result = success;
          return true;
        }
      }
    }
    return false;
  };

  // Pop pending CRCs and process them.
  ul_crc_indication crc_ind;
  while (pending_crcs.try_pop(crc_ind)) {
    for (const ul_crc_pdu_indication& crc : crc_ind.crcs) {
      auto crc_it = pending_msg3s.find(get_msg3_ring_key(crc.rnti));
      if (crc_it == pending_msg3s.end()) {
        if (not mark_msga_crc(crc.rnti, crc.tb_crc_success)) {
          logger.warning("pci={} rnti={}: Invalid UL CRC PDU indication. Cause: Nonexistent tc-rnti",
                         cell_cfg.params.pci,
                         crc.rnti);
        }
        continue;
      }
      auto& pending_msg3 = crc_it->second;

      // See TS38.321, 5.4.2.1 - "For UL transmission with UL grant in RA Response, HARQ process identifier 0 is used."
      harq_id_t                             h_id = to_harq_id(0);
      std::optional<ul_harq_process_handle> h_ul = pending_msg3.harq_ent.ul_harq(h_id, crc_ind.sl_rx);
      if (not h_ul.has_value() or crc.harq_id != h_id) {
        logger.warning("pci={} tc-rnti={}: Invalid UL CRC for HARQ h_id={}. Cause: HARQ-Id 0 must be used in Msg3",
                       cell_cfg.params.pci,
                       crc.rnti,
                       fmt::underlying(crc.harq_id));
        continue;
      }

      // Handle CRC info.
      h_ul->ul_crc_info(crc.tb_crc_success);
      if (h_ul->empty()) {
        // Deallocate Msg3 entry.
        pending_msg3s.erase(crc_it);
        // In case of CFRA, update cfra mapping.
        if (crc.ue_index != INVALID_DU_UE_INDEX) {
          pending_cfra_ues[crc.ue_index].store(rnti_t::INVALID_RNTI, std::memory_order_release);
        }
      }

      // Forward MSG3 CRC indication to metrics handler.
      metrics_hdlr.handle_msg3_crc_indication(crc);
    }
  }

  // Allocate pending Msg3 retransmissions.
  // Note: pending_ul_retxs size will change in this iteration, so we prefetch the next iterator.
  auto pending_ul_retxs = ra_harqs.pending_ul_retxs();
  for (auto it = pending_ul_retxs.begin(); it != pending_ul_retxs.end();) {
    ul_harq_process_handle h_ul = *it;
    ++it;
    auto retx_it = pending_msg3s.find(static_cast<uint16_t>(h_ul.ue_index()));
    ocudu_sanity_check(retx_it != pending_msg3s.end(), "Msg3 retx HARQ has no matching pending_msg3 entry");
    schedule_msg3_retx(res_alloc, retx_it->second);
  }
}

void ra_scheduler::run_slot(cell_resource_allocator& res_alloc)
{
  // Reserve MsgA PUSCH space.
  reserve_msga_pusch_rbs(res_alloc);

  // Update Msg3 HARQ state.
  ra_harqs.slot_indication(res_alloc.slot_tx());

  // Handle pending CRCs, which may lead to Msg3 reTxs.
  handle_pending_crc_indications_impl(res_alloc);

  // Pop pending RACHs and process them.
  rach_indication_message rach;
  while (pending_rachs.try_pop(rach)) {
    handle_rach_indication_impl(rach, res_alloc);
  }

  // Schedule pending RARs.
  schedule_pending_rars(res_alloc);

  // Schedule pending MsgBs.
  schedule_pending_msgbs(res_alloc);
}

void ra_scheduler::stop()
{
  rach_indication_message rach;
  while (pending_rachs.try_pop(rach)) {
  }
  ul_crc_indication crc;
  while (pending_crcs.try_pop(crc)) {
  }
  pending_rars.clear();
  pending_msg3s.clear();
  pending_msgbs.clear();
}

void ra_scheduler::update_pending_rars(slot_point pdcch_slot)
{
  for (auto it = pending_rars.begin(); it != pending_rars.end();) {
    pending_rar_alloc& rar_req = *it;

    // In case of RAR being outside RAR window:
    // - if window has passed, discard RAR
    // - if window hasn't started, stop loop, as RARs are ordered by slot
    if (not rar_req.rar_window.contains(pdcch_slot)) {
      if (pdcch_slot >= rar_req.rar_window.stop()) {
        logger.warning("ra-rnti={}: Could not transmit RAR within the window={}, prach_slot={}, slot_tx={}, "
                       "failed_attempts={{pdcch={}, pdsch={}, pusch={}}}",
                       rar_req.ra_rnti,
                       rar_req.rar_window,
                       rar_req.prach_slot_rx,
                       pdcch_slot,
                       rar_req.failed_attempts.pdcch,
                       rar_req.failed_attempts.pdsch,
                       rar_req.failed_attempts.pusch);
        // Clear associated Msg3 grants that were not yet scheduled.
        for (rnti_t tcrnti : rar_req.tc_rntis) {
          pending_msg3s.erase(get_msg3_ring_key(tcrnti));
        }
        it = pending_rars.erase(it);
        continue;
      }
      break;
    }
    ++it;
  }
}

bool ra_scheduler::is_slot_candidate_for_rar(const cell_slot_resource_allocator& slot_res_alloc)
{
  slot_point pdcch_slot = slot_res_alloc.slot;

  // Check there are any RARs to schedule.
  if (pending_rars.empty() or not pending_rars.front().rar_window.contains(pdcch_slot)) {
    // There are no RARs to schedule with RAR window containing this slot.
    // Note: The pending RARs are ordered by slot, and this function should be called after outdated RARs have already
    // been removed.
    return false;
  }

  // Ensure slot for RAR PDCCH has DL enabled.
  if (not cell_cfg.is_dl_enabled(pdcch_slot)) {
    log_postponed_rar(pending_rars.front(), "PDCCH slot is not DL enabled", pdcch_slot);
    ++pending_rars.front().failed_attempts.pdcch;
    return false;
  }

  // Check for space in PDCCH result list. We check for space in PDSCH later, once the k0 is known.
  if (slot_res_alloc.result.dl.dl_pdcchs.full()) {
    log_postponed_rar(pending_rars.front(), "PDCCH grants limit reached", pdcch_slot);
    ++pending_rars.front().failed_attempts.pdcch;
    return false;
  }

  // Ensure (i) RA SearchSpace PDCCH monitoring is active for this slot and (ii) there are enough DL symbols to allocate
  // the PDCCH.
  const search_space_configuration& ss_cfg = get_ra_ss_cfg(cell_cfg);
  const coreset_configuration&      cs_cfg = cell_cfg.get_common_coreset(ss_cfg.get_coreset_id());
  if (not pdcch_helper::is_pdcch_monitoring_active(pdcch_slot, ss_cfg) or
      ss_cfg.get_first_symbol_index() + cs_cfg.duration() > cell_cfg.get_nof_dl_symbol_per_slot(pdcch_slot)) {
    // RAR scheduling only possible when PDCCH monitoring is active.
    log_postponed_rar(pending_rars.front(), "PDCCH monitoring is inactive or not enough DL symbols", pdcch_slot);
    ++pending_rars.front().failed_attempts.pdcch;
    return false;
  }

  // Ensure there are UL slots where Msg3s can be allocated.
  bool       pusch_slots_available = false;
  const auto pusch_td_list         = get_pusch_td_list(cell_cfg);
  for (const auto& pusch_td_alloc : pusch_td_list) {
    const unsigned msg3_delay =
        ra_helper::get_msg3_delay(cell_cfg.params.ul_cfg_common.init_ul_bwp.generic_params.scs, pusch_td_alloc.k2) +
        cell_cfg.ntn_cs_koffset;
    const unsigned start_ul_symbols =
        NOF_OFDM_SYM_PER_SLOT_NORMAL_CP - cell_cfg.get_nof_ul_symbol_per_slot(pdcch_slot + msg3_delay);
    if (cell_cfg.is_ul_enabled(pdcch_slot + msg3_delay) and pusch_td_alloc.symbols.start() >= start_ul_symbols) {
      pusch_slots_available = true;
      break;
    }
  }
  if (not pusch_slots_available) {
    // Msg3 scheduling only possible when PUSCH is available.
    log_postponed_rar(pending_rars.front(), "No PUSCH slot that is UL enabled.", pdcch_slot);
    ++pending_rars.front().failed_attempts.pusch;
    return false;
  }

  return true;
}

void ra_scheduler::schedule_pending_rars(cell_resource_allocator& res_alloc)
{
  if (pending_rars.empty()) {
    // No RARs to allocate.
    return;
  }

  // In case there were attempts to schedule a pending RAR in an earlier slot, we resume the scheduling of the same
  // RAR from where we left off to avoid unnecessary work.
  // In case it is the first attempt at scheduling a pending RAR, we start from the current PDCCH slot.
  unsigned sched_start_delay = pending_rars.front().last_sched_try_slot.valid()
                                   ? pending_rars.front().last_sched_try_slot + 1 - res_alloc.slot_tx()
                                   : 0;

  for (unsigned n = sched_start_delay; n <= max_dl_slots_ahead_sched and not pending_rars.empty(); ++n) {
    // Schedule RARs for the given PDCCH slot.
    schedule_pending_rars(res_alloc, res_alloc.slot_tx() + n);
  }

  // For the RARs that were not scheduled, save the last slot when an allocation was attempted. This avoids redundant
  // scheduling attempts.
  for (pending_rar_alloc& rar : pending_rars) {
    rar.last_sched_try_slot = res_alloc.slot_tx() + max_dl_slots_ahead_sched;
  }
}

void ra_scheduler::schedule_pending_rars(cell_resource_allocator& res_alloc, slot_point pdcch_slot)
{
  // Remove outdated RARs.
  update_pending_rars(pdcch_slot);

  // Check if slot is a valid candidate for RAR scheduling.
  if (not is_slot_candidate_for_rar(res_alloc[pdcch_slot])) {
    return;
  }

  for (auto it = pending_rars.begin(); it != pending_rars.end();) {
    pending_rar_alloc& rar_req = *it;
    if (not rar_req.rar_window.contains(pdcch_slot)) {
      // RAR window hasn't started yet for this RAR. Given that the RARs are in order of slot, we can stop here.
      break;
    }
    if (rar_req.tc_rntis.empty()) {
      // This should never happen, unless there was some corruption of the queue.
      logger.warning(
          "ra-rnti={}: Discarding RAR scheduling request. Cause: There are no TC-RNTIs associated with this RAR",
          rar_req.ra_rnti);
      it = pending_rars.erase(it);
      continue;
    }

    // Try to schedule DCIs + RBGs for RAR Grants
    const size_t nof_allocs = schedule_rar(rar_req, res_alloc, pdcch_slot);

    if (nof_allocs > 0) {
      // If RAR allocation was successful:
      // - in case all Msg3 grants were allocated, remove pending RAR, and continue with following RAR
      // - otherwise, erase only Msg3 grants that were allocated, and stop iteration

      if (nof_allocs >= rar_req.tc_rntis.size()) {
        it = pending_rars.erase(it);
      } else {
        // Remove only allocated Msg3 grants
        std::copy(rar_req.tc_rntis.begin() + nof_allocs, rar_req.tc_rntis.end(), rar_req.tc_rntis.begin());
        const size_t new_pending_msg3s =
            rar_req.tc_rntis.size() > nof_allocs ? rar_req.tc_rntis.size() - nof_allocs : 0;
        rar_req.tc_rntis.resize(new_pending_msg3s);
        break;
      }
    } else {
      // If RAR allocation was not successful, try next pending RAR
      ++it;
    }
  }
}

unsigned ra_scheduler::schedule_rar(pending_rar_alloc& rar, cell_resource_allocator& res_alloc, slot_point pdcch_slot)
{
  cell_slot_resource_allocator& pdcch_alloc = res_alloc[pdcch_slot];

  const subcarrier_spacing dl_scs = cell_cfg.params.dl_cfg_common.init_dl_bwp.generic_params.scs;
  span<const pdsch_time_domain_resource_allocation> pdsch_td_res_alloc_list = get_ra_pdsch_td_list(cell_cfg);
  const search_space_configuration&                 ss_cfg                  = get_ra_ss_cfg(cell_cfg);
  const unsigned coreset_duration = cell_cfg.get_common_coreset(ss_cfg.get_coreset_id()).duration();

  unsigned     pdsch_time_res_index = pdsch_td_res_alloc_list.size();
  unsigned     max_nof_allocs       = 0;
  crb_interval rar_crbs{};
  for (const auto& pdsch_td_res : pdsch_td_res_alloc_list) {
    const unsigned                      time_resource = std::distance(pdsch_td_res_alloc_list.begin(), &pdsch_td_res);
    const cell_slot_resource_allocator& pdsch_alloc   = res_alloc[pdcch_slot + pdsch_td_res.k0];

    // > Check space in DL sched result for RAR.
    if (pdsch_alloc.result.dl.rar_grants.full()) {
      // early exit.
      log_postponed_rar(rar, "RAR grants limit reached", pdcch_slot);
      ++rar.failed_attempts.pdsch;
      return 0;
    }

    // > Ensure slot for RAR PDSCH has DL enabled.
    if (not cell_cfg.is_dl_enabled(pdsch_alloc.slot)) {
      // Early exit.
      log_postponed_rar(rar, "PDSCH slot is not DL enabled", pdcch_slot);
      ++rar.failed_attempts.pdsch;
      return 0;
    }

    // > Check whether PDSCH time domain resource fits in DL symbols of the slot.
    if (pdsch_td_res.symbols.stop() > cell_cfg.get_nof_dl_symbol_per_slot(pdsch_alloc.slot)) {
      continue;
    }

    // > Check whether PDSCH time domain resource does not overlap with CORESET.
    if (pdsch_td_res.symbols.start() < ss_cfg.get_first_symbol_index() + coreset_duration) {
      continue;
    }

    if (csi_helper::is_csi_rs_slot(cell_cfg, pdsch_alloc.slot)) {
      // We cannot multiplex RAR PDSCH and CSI-RS, because at this point the UE has no access to the CSI-RS config.
      log_postponed_rar(rar, "RAR can not be in CSI-RS slot", pdcch_slot);
      ++rar.failed_attempts.pdsch;
      return 0;
    }

    // > Find available RBs in PDSCH for RAR grant.
    const unsigned          nof_rar_rbs    = get_nof_pdsch_prbs_required(time_resource, rar.tc_rntis.size()).nof_prbs;
    const ofdm_symbol_range symbols        = pdsch_td_res.symbols;
    const crb_bitmap        used_crbs      = pdsch_alloc.dl_res_grid.used_crbs(dl_scs, ra_crb_lims, symbols);
    const auto              available_crbs = rb_helper::find_empty_interval_of_length(used_crbs, nof_rar_rbs);
    // Check how many allocations can we fit in the available interval.
    // Note: we have to call \c get_nof_pdsch_prbs_required for every nof_allocs because the number of PRBs is not
    // linear w.r.t. the payload size (all RARs are sent in the same PDSCH grant). See \ref ocudu::get_nof_prbs.
    unsigned nof_allocs = rar.tc_rntis.size();
    while (nof_allocs != 0 &&
           get_nof_pdsch_prbs_required(time_resource, nof_allocs).nof_prbs > available_crbs.length()) {
      --nof_allocs;
    }

    // > Pick the TD resource that leads to the maximum allocations possible.
    if (nof_allocs > max_nof_allocs) {
      max_nof_allocs       = nof_allocs;
      rar_crbs             = available_crbs;
      pdsch_time_res_index = time_resource;
    }
  }

  if (max_nof_allocs == 0) {
    // Early exit.
    log_postponed_rar(rar, "Not enough PRBs available for RAR PDSCH", pdcch_slot);
    ++rar.failed_attempts.pdsch;
    return 0;
  }

  if (pdsch_time_res_index == pdsch_td_res_alloc_list.size()) {
    // Early exit.
    log_postponed_rar(rar, "No PDSCH time domain resource found for RAR", pdcch_slot);
    ++rar.failed_attempts.pdsch;
    return 0;
  }

  // > Find available RBs in PUSCH for Msg3 grants. This process requires searching for a valid K2 value in
  // the list of PUSCH-TimeDomainResourceAllocation in PUSCHConfigCommon.
  static_vector<msg3_alloc_candidate, MAX_GRANTS_PER_RAR> msg3_candidates;
  const auto& ul_bwp_cfg = cell_cfg.params.ul_cfg_common.init_ul_bwp.generic_params;
  const auto  pusch_list = get_pusch_td_list(cell_cfg);
  for (unsigned pusch_idx = 0; pusch_idx < pusch_list.size(); ++pusch_idx) {
    unsigned pusch_res_max_allocs =
        std::min(msg3_candidates.capacity() - msg3_candidates.size(), max_nof_allocs - msg3_candidates.size());

    // >> Verify if Msg3 delay provided by current PUSCH-TimeDomainResourceAllocation corresponds to an UL slot.
    const unsigned msg3_delay =
        ra_helper::get_msg3_delay(ul_bwp_cfg.scs, pusch_list[pusch_idx].k2) + cell_cfg.ntn_cs_koffset;
    const cell_slot_resource_allocator& msg3_alloc = res_alloc[pdcch_slot + msg3_delay];
    const unsigned                      start_ul_symbols =
        NOF_OFDM_SYM_PER_SLOT_NORMAL_CP - cell_cfg.get_nof_ul_symbol_per_slot(msg3_alloc.slot);
    if (not cell_cfg.is_ul_enabled(msg3_alloc.slot) or pusch_list[pusch_idx].symbols.start() < start_ul_symbols) {
      continue;
    }

    // >> Check space in UL sched result for remaining Msg3s.
    const unsigned list_space = msg3_alloc.result.ul.puschs.capacity() - msg3_alloc.result.ul.puschs.size();
    pusch_res_max_allocs      = std::min(pusch_res_max_allocs, list_space);
    if (pusch_res_max_allocs == 0) {
      continue;
    }

    // >> Check CRBs available in PUSCH for Msg3.
    const unsigned nof_rbs_per_msg3 = msg3_data[pusch_idx].pusch.rbs.type1().length();
    unsigned       nof_msg3_rbs     = nof_rbs_per_msg3 * pusch_res_max_allocs;
    crb_bitmap     used_ul_crbs     = msg3_alloc.ul_res_grid.used_crbs(ul_bwp_cfg, pusch_list[pusch_idx].symbols);
    // Mark the CRBs used by PUCCH as occupied.
    used_ul_crbs |= pucch_crbs;
    crb_interval   msg3_crbs              = rb_helper::find_empty_interval_of_length(used_ul_crbs, nof_msg3_rbs);
    const unsigned max_allocs_on_free_rbs = msg3_crbs.length() / nof_rbs_per_msg3;

    if (max_allocs_on_free_rbs == 0) {
      continue;
    }

    // >> Register Msg3 allocations for this PUSCH resource as successful.
    unsigned last_crb = msg3_crbs.start();
    // NOTE: this should not happen, but we added as an extra safety step.
    if (max_allocs_on_free_rbs + msg3_alloc.result.ul.puschs.size() > msg3_alloc.result.ul.puschs.capacity()) {
      logger.warning("Overestimated number of MSG3 grants that can be allocated ({} > {}). This number will be capped",
                     max_allocs_on_free_rbs,
                     list_space);
    }
    pusch_res_max_allocs = std::min(pusch_res_max_allocs, max_allocs_on_free_rbs);
    for (unsigned i = 0; i != pusch_res_max_allocs; ++i) {
      msg3_alloc_candidate& candidate = msg3_candidates.emplace_back();
      candidate.crbs                  = {last_crb, last_crb + nof_rbs_per_msg3};
      candidate.pusch_td_res_index    = pusch_idx;
      last_crb += nof_rbs_per_msg3;
    }
  }
  max_nof_allocs = msg3_candidates.size();
  if (max_nof_allocs == 0) {
    log_postponed_rar(rar, "No PUSCH time domain resource found for Msg3");
    ++rar.failed_attempts.pusch;
    return 0;
  }
  rar_crbs.resize(get_nof_pdsch_prbs_required(pdsch_time_res_index, max_nof_allocs).nof_prbs);

  // > Find space in PDCCH for RAR.
  static constexpr aggregation_level aggr_lvl = aggregation_level::n4;
  const search_space_id              ss_id = cell_cfg.params.dl_cfg_common.init_dl_bwp.pdcch_common.ra_search_space_id;
  pdcch_dl_information*              pdcch = pdcch_sch.alloc_dl_pdcch_common(pdcch_alloc, rar.ra_rnti, ss_id, aggr_lvl);
  if (pdcch == nullptr) {
    log_postponed_rar(rar, "No PDCCH space for RAR", pdcch_slot);
    ++rar.failed_attempts.pdcch;
    return 0;
  }

  // Status: RAR allocation is successful.

  // > Fill RAR and Msg3 PDSCH, PUSCH and DCI.
  fill_rar_grant(res_alloc, rar, pdcch_slot, rar_crbs, pdsch_time_res_index, msg3_candidates);

  return msg3_candidates.size();
}

void ra_scheduler::fill_rar_grant(cell_resource_allocator&         res_alloc,
                                  const pending_rar_alloc&         rar_request,
                                  slot_point                       pdcch_slot,
                                  crb_interval                     rar_crbs,
                                  unsigned                         pdsch_time_res_index,
                                  span<const msg3_alloc_candidate> msg3_candidates)
{
  const auto& init_dl_bwp             = cell_cfg.params.dl_cfg_common.init_dl_bwp;
  const auto  pdsch_td_res_alloc_list = get_ra_pdsch_td_list(cell_cfg);

  cell_slot_resource_allocator& pdcch_alloc = res_alloc[pdcch_slot];
  cell_slot_resource_allocator& rar_alloc   = res_alloc[pdcch_slot + pdsch_td_res_alloc_list[pdsch_time_res_index].k0];

  // Fill RAR DCI.
  pdcch_dl_information& pdcch = pdcch_alloc.result.dl.dl_pdcchs.back();
  build_dci_f1_0_ra_rnti(pdcch.dci, init_dl_bwp, rar_crbs, pdsch_time_res_index, sched_cfg.rar_mcs_index);

  // Allocate RBs and space for RAR.
  rar_alloc.dl_res_grid.fill(
      grant_info{init_dl_bwp.generic_params.scs, pdsch_td_res_alloc_list[pdsch_time_res_index].symbols, rar_crbs});

  // Fill RAR PDSCH.
  rar_information& rar = rar_alloc.result.dl.rar_grants.emplace_back();
  build_pdsch_f1_0_ra_rnti(rar.pdsch_cfg,
                           get_nof_pdsch_prbs_required(pdsch_time_res_index, msg3_candidates.size()).tbs_bytes,
                           pdcch.ctx.rnti,
                           cell_cfg,
                           pdcch.dci.as_ra_rnti_f1_0(),
                           rar_crbs,
                           rar_data[pdsch_time_res_index].dmrs_info);

  const auto& init_ul_bwp         = cell_cfg.params.ul_cfg_common.init_ul_bwp;
  const auto  pusch_td_alloc_list = get_pusch_td_list(cell_cfg);
  for (unsigned i = 0; i < msg3_candidates.size(); ++i) {
    const auto&    msg3_candidate = msg3_candidates[i];
    const auto&    pusch_res      = pusch_td_alloc_list[msg3_candidate.pusch_td_res_index];
    const unsigned msg3_delay =
        ra_helper::get_msg3_delay(init_ul_bwp.generic_params.scs, pusch_res.k2) + cell_cfg.ntn_cs_koffset;
    cell_slot_resource_allocator& msg3_alloc = res_alloc[pdcch_slot + msg3_delay];
    const vrb_interval            vrbs       = ul_crb_to_vrb(cell_cfg, msg3_candidate.crbs);

    auto msg3_it = pending_msg3s.find(get_msg3_ring_key(rar_request.tc_rntis[i]));
    ocudu_sanity_check(msg3_it != pending_msg3s.end(),
                       "Pending Msg3 entry should have been reserved when RACH was received");
    auto& pending_msg3 = msg3_it->second;

    // Allocate Msg3 UL HARQ.
    std::optional<ul_harq_process_handle> h_ul =
        pending_msg3.harq_ent.alloc_ul_harq(msg3_alloc.slot, sched_cfg.max_nof_msg3_harq_retxs);
    ocudu_sanity_check(h_ul.has_value(), "Pending Msg3 HARQ must be available when RAR is allocated");

    if (rar.grants.full() or msg3_alloc.result.ul.puschs.full()) {
      // Note: This should never happen.
      logger.error("RAR grant for tc-rnti={} will be dropped. Cause: no available free RAR or UL grants",
                   pending_msg3.preamble.tc_rnti);
      return;
    }

    // Add MAC SDU with UL grant (Msg3) in RAR PDU.
    rar_ul_grant& msg3_info            = rar.grants.emplace_back();
    msg3_info.rapid                    = pending_msg3.preamble.preamble_id;
    msg3_info.ta                       = pending_msg3.preamble.time_advance.to_Ta(init_ul_bwp.generic_params.scs);
    msg3_info.temp_crnti               = pending_msg3.preamble.tc_rnti;
    msg3_info.time_resource_assignment = msg3_candidate.pusch_td_res_index;
    msg3_info.freq_resource_assignment = ra_frequency_type1_get_riv(
        ra_frequency_type1_configuration{init_ul_bwp.generic_params.crbs.length(), vrbs.start(), vrbs.length()});
    msg3_info.mcs = sched_cfg.msg3_mcs_index;
    // Determine TPC command based on Table 8.2-2, TS 38.213.
    msg3_info.tpc     = (init_ul_bwp.pusch_cfg_common->msg3_delta_power.value() + 6) / 2;
    msg3_info.csi_req = false;

    // Allocate Msg3 RBs.
    const ofdm_symbol_range& symbols = pusch_td_alloc_list[msg3_candidate.pusch_td_res_index].symbols;
    msg3_alloc.ul_res_grid.fill(grant_info{init_ul_bwp.generic_params.scs, symbols, msg3_candidate.crbs});

    // Fill PUSCH for Msg3.
    ul_sched_info& pusch     = msg3_alloc.result.ul.puschs.emplace_back();
    pusch.context.ue_index   = INVALID_DU_UE_INDEX;
    pusch.context.ss_id      = init_dl_bwp.pdcch_common.ra_search_space_id;
    pusch.context.nof_retxs  = 0;
    pusch.context.msg3_delay = msg3_delay;
    pusch.pusch_cfg          = msg3_data[msg3_candidate.pusch_td_res_index].pusch;
    pusch.pusch_cfg.rnti     = pending_msg3.preamble.tc_rnti;
    pusch.pusch_cfg.rbs      = vrbs;
    pusch.pusch_cfg.rv_index = 0;
    pusch.pusch_cfg.new_data = true;

    // Store parameters used in HARQ.
    h_ul->save_grant_params(ul_harq_alloc_context{dci_ul_rnti_config_type::tc_rnti_f0_0}, pusch.pusch_cfg);
  }
}

/// Helper function to log a failure to allocate a Msg3 grant.
/// Note: Debug log level is used. We only warn once the HARQ process retransmission period times out.
static void log_failed_msg3_retx(ocudulog::basic_logger& logger,
                                 pci_t                   pci,
                                 rnti_t                  tcrnti,
                                 slot_point              pusch_slot,
                                 const char*             cause_str)
{
  if (not pusch_slot.valid()) {
    logger.debug("pci={} tc-rnti={}: Failed to allocate PUSCH Msg3 reTx grant. Retrying it in a later slot. Cause: {}",
                 pci,
                 tcrnti,
                 cause_str);
  } else {
    logger.debug("pci={} tc-rnti={}: Failed to allocate PUSCH Msg3 reTx grant in slot {}. Retrying it in a later slot. "
                 "Cause: {}",
                 pci,
                 tcrnti,
                 pusch_slot,
                 cause_str);
  }
}

void ra_scheduler::schedule_msg3_retx(cell_resource_allocator& res_alloc, pending_msg3_alloc& msg3_ctx) const
{
  cell_slot_resource_allocator& pdcch_alloc = res_alloc[0];
  const bwp_configuration&      bwp_ul_cmn  = cell_cfg.params.ul_cfg_common.init_ul_bwp.generic_params;

  if (not cell_cfg.is_dl_enabled(pdcch_alloc.slot)) {
    // Not possible to schedule Msg3s in this TDD slot.
    return;
  }

  // Verify there is space in PDCCH result lists for new allocations.
  if (pdcch_alloc.result.dl.ul_pdcchs.full()) {
    // Early exit. No space for new PDCCHs.
    log_failed_msg3_retx(logger,
                         cell_cfg.params.pci,
                         msg3_ctx.preamble.tc_rnti,
                         {},
                         "No space available in scheduler PDCCH output list");
    return;
  }

  // Check if there are enough DL symbols to allocate the UL PDCCH
  const search_space_configuration& ss_cfg = get_ra_ss_cfg(cell_cfg);
  const coreset_configuration&      cs_cfg = cell_cfg.get_common_coreset(ss_cfg.get_coreset_id());
  if (ss_cfg.get_first_symbol_index() + cs_cfg.duration() > cell_cfg.get_nof_dl_symbol_per_slot(pdcch_alloc.slot)) {
    // Early exit. RAR scheduling only possible when PDCCH monitoring is active.
    log_failed_msg3_retx(
        logger, cell_cfg.params.pci, msg3_ctx.preamble.tc_rnti, {}, "Not enough DL symbols to fit UL PDCCH");
    return;
  }

  // Fetch UL HARQ.
  ul_harq_process_handle h_ul = msg3_ctx.harq_ent.ul_harq(to_harq_id(0)).value();
  ocudu_sanity_check(h_ul.has_pending_retx(), "schedule_msg3_retx called when HARQ has no pending reTx");
  const ul_harq_process_handle::grant_params& last_harq_params = h_ul.get_grant_params();

  const span<const pusch_time_domain_resource_allocation> pusch_td_alloc_list = get_pusch_td_list(cell_cfg);
  for (unsigned pusch_td_res_index = 0; pusch_td_res_index != pusch_td_alloc_list.size(); ++pusch_td_res_index) {
    const auto&                   pusch_td_cfg = pusch_td_alloc_list[pusch_td_res_index];
    const unsigned                k2           = pusch_td_cfg.k2;
    cell_slot_resource_allocator& pusch_alloc  = res_alloc[k2 + cell_cfg.ntn_cs_koffset];
    const unsigned                start_ul_symbols =
        NOF_OFDM_SYM_PER_SLOT_NORMAL_CP - cell_cfg.get_nof_ul_symbol_per_slot(pusch_alloc.slot);
    // If it is a retx, we need to ensure we use a time_domain_resource with the same number of symbols as used for
    // the first transmission.
    const bool sym_length_match_prev_grant_for_retx = pusch_td_cfg.symbols.length() == last_harq_params.nof_symbols;
    if (not cell_cfg.is_ul_enabled(pusch_alloc.slot) or pusch_td_cfg.symbols.start() < start_ul_symbols or
        !sym_length_match_prev_grant_for_retx) {
      // Not possible to schedule Msg3s in this TDD slot due to lack of PUSCH symbols.
      continue;
    }

    // Verify there is space in PUSCH and PDCCH result lists for new allocations.
    const auto max_puschs = std::min<unsigned>(pusch_alloc.result.ul.puschs.capacity(),
                                               res_alloc.cfg.expert_cfg.ue.max_ul_grants_per_slot -
                                                   static_cast<unsigned>(pusch_alloc.result.ul.pucchs.size()));
    if (pusch_alloc.result.ul.puschs.size() >= max_puschs) {
      // Early continue. Maximum limit of PUSCH grants was reached for this slot.
      log_failed_msg3_retx(logger,
                           cell_cfg.params.pci,
                           msg3_ctx.preamble.tc_rnti,
                           pusch_alloc.slot,
                           "No space available in scheduler PUSCH output list");
      continue;
    }

    // Try to reuse previous HARQ PRBs.
    const vrb_interval msg3_vrbs = last_harq_params.rbs.type1();
    grant_info         grant;
    grant.scs     = bwp_ul_cmn.scs;
    grant.symbols = pusch_td_cfg.symbols;
    grant.crbs    = msg3_vrb_to_crb(cell_cfg, msg3_vrbs);
    if (pusch_alloc.ul_res_grid.collides(grant)) {
      // Find available symbol x RB resources.
      log_failed_msg3_retx(
          logger, cell_cfg.params.pci, msg3_ctx.preamble.tc_rnti, pusch_alloc.slot, "Not enough available RBs");
      continue;
    }

    // > Find space in PDCCH for Msg3 DCI.
    // [3GPP TS 38.213, clause 10.1] a UE monitors PDCCH candidates in one or more of the following search spaces sets
    //  - a Type1-PDCCH CSS set configured by ra-SearchSpace in PDCCH-ConfigCommon for a DCI format with
    //    CRC scrambled by a RA-RNTI, a MsgB-RNTI, or a TC-RNTI on the primary cell.
    pdcch_ul_information* pdcch =
        pdcch_sch.alloc_ul_pdcch_common(pdcch_alloc, msg3_ctx.preamble.tc_rnti, ss_cfg.get_id(), aggregation_level::n4);
    if (pdcch == nullptr) {
      // Early exit. No point in continuing iteration if PDCCH fails to allocate.
      log_failed_msg3_retx(
          logger, cell_cfg.params.pci, msg3_ctx.preamble.tc_rnti, pusch_alloc.slot, "Failed to allocate UL PDCCH");
      break;
    }

    // Mark resources as occupied in the ResourceGrid.
    pusch_alloc.ul_res_grid.fill(grant);

    // Allocate new retx in the HARQ.
    if (not h_ul.new_retx(pusch_alloc.slot)) {
      logger.error(
          "pci={}: tc-rnti={}: Failed to allocate reTx for Msg3", cell_cfg.params.pci, msg3_ctx.preamble.tc_rnti);
      pdcch_sch.cancel_last_pdcch(pdcch_alloc);
      continue;
    }

    // Fill DCI.
    static constexpr uint8_t msg3_rv = 0;
    build_dci_f0_0_tc_rnti(pdcch->dci,
                           cell_cfg.params.dl_cfg_common.init_dl_bwp,
                           cell_cfg.params.ul_cfg_common.init_ul_bwp.generic_params,
                           grant.crbs,
                           pusch_td_res_index,
                           last_harq_params.mcs,
                           msg3_rv);

    // Fill PUSCH.
    ul_sched_info& ul_info     = pusch_alloc.result.ul.puschs.emplace_back();
    ul_info.context.ue_index   = INVALID_DU_UE_INDEX;
    ul_info.context.ss_id      = ss_cfg.get_id();
    ul_info.context.k2         = k2;
    ul_info.context.nof_retxs  = h_ul.nof_retxs();
    ul_info.pusch_cfg          = msg3_data[pusch_td_res_index].pusch;
    ul_info.pusch_cfg.rnti     = msg3_ctx.preamble.tc_rnti;
    ul_info.pusch_cfg.rbs      = msg3_vrbs;
    ul_info.pusch_cfg.rv_index = pdcch->dci.tc_rnti_f0_0.redundancy_version;
    ul_info.pusch_cfg.new_data = false;

    // Store parameters used in HARQ.
    h_ul.save_grant_params(ul_harq_alloc_context{dci_ul_rnti_config_type::tc_rnti_f0_0}, ul_info.pusch_cfg);

    // successful allocation. Exit loop.
    break;
  }
}

void ra_scheduler::schedule_pending_msgbs(cell_resource_allocator& res_alloc)
{
  if (pending_msgbs.empty()) {
    // Early exit: No pending MsgBs to schedule.
    return;
  }

  const unsigned sched_start_delay = pending_msgbs.front().last_sched_try_slot.valid()
                                         ? pending_msgbs.front().last_sched_try_slot + 1 - res_alloc.slot_tx()
                                         : 0U;

  for (unsigned n = sched_start_delay; n <= max_dl_slots_ahead_sched and not pending_msgbs.empty(); ++n) {
    schedule_pending_msgbs(res_alloc, res_alloc.slot_tx() + n);
  }

  for (pending_msgb_alloc& msgb : pending_msgbs) {
    msgb.last_sched_try_slot = res_alloc.slot_tx() + max_dl_slots_ahead_sched;
  }
}

void ra_scheduler::schedule_pending_msgbs(cell_resource_allocator& res_alloc, slot_point pdcch_slot)
{
  // Expire MsgB entries whose window has passed.
  for (auto it = pending_msgbs.begin(); it != pending_msgbs.end();) {
    if (pdcch_slot >= it->msgb_window.stop()) {
      logger.warning("msgb-rnti={}: Could not transmit MsgB within the window={}, prach_slot={}",
                     it->msgb_rnti,
                     it->msgb_window,
                     it->prach_slot_rx);
      it = pending_msgbs.erase(it);
    } else {
      ++it;
    }
  }
  if (pending_msgbs.empty()) {
    return;
  }

  // Check common PDCCH slot conditions.
  if (not cell_cfg.is_dl_enabled(pdcch_slot)) {
    return;
  }
  const cell_slot_resource_allocator& pdcch_slot_alloc = res_alloc[pdcch_slot];
  if (pdcch_slot_alloc.result.dl.dl_pdcchs.full()) {
    return;
  }
  const search_space_configuration& ss_cfg           = get_ra_ss_cfg(cell_cfg);
  const coreset_configuration&      cs_cfg           = cell_cfg.get_common_coreset(ss_cfg.get_coreset_id());
  const unsigned                    coreset_duration = cs_cfg.duration();
  if (not pdcch_helper::is_pdcch_monitoring_active(pdcch_slot, ss_cfg) or
      ss_cfg.get_first_symbol_index() + coreset_duration > cell_cfg.get_nof_dl_symbol_per_slot(pdcch_slot)) {
    return;
  }

  const auto&                                             init_dl_bwp   = cell_cfg.params.dl_cfg_common.init_dl_bwp;
  const auto&                                             init_ul_bwp   = cell_cfg.params.ul_cfg_common.init_ul_bwp;
  const subcarrier_spacing                                dl_scs        = init_dl_bwp.generic_params.scs;
  const span<const pdsch_time_domain_resource_allocation> pdsch_td_list = get_ra_pdsch_td_list(cell_cfg);
  const span<const pusch_time_domain_resource_allocation> pusch_td_list = get_pusch_td_list(cell_cfg);
  static constexpr aggregation_level                      aggr_lvl      = aggregation_level::n4;
  const search_space_id                                   ss_id         = init_dl_bwp.pdcch_common.ra_search_space_id;

  for (auto msgb_it = pending_msgbs.begin(); msgb_it != pending_msgbs.end();) {
    pending_msgb_alloc& msgb = *msgb_it;

    if (not msgb.msgb_window.contains(pdcch_slot)) {
      // MsgB window has not yet started.
      ++msgb_it;
      continue;
    }

    // If any preamble's CRC hasn't arrived yet, postpone it, unless this is the last slot in the MsgB window, in which
    // case, we proceed and treat pending CRCs as FallbackRAR.
    const bool any_crc_pending = std::any_of(
        msgb.preambles.begin(), msgb.preambles.end(), [](const auto& p) { return not p.crc_result.has_value(); });
    const bool last_slot_in_window = (pdcch_slot + 1 >= msgb.msgb_window.stop());
    if (any_crc_pending and not last_slot_in_window) {
      // Postpone MsgB.
      ++msgb_it;
      continue;
    }

    const unsigned nof_preambles = msgb.preambles.size();

    // -- Find PDSCH time domain resource and available CRBs that maximizes the number of MsgB allocations --
    unsigned     pdsch_time_res_index = pdsch_td_list.size();
    unsigned     max_nof_allocs       = 0;
    crb_interval msgb_crbs{};
    for (unsigned time_resource = 0; time_resource != pdsch_td_list.size(); ++time_resource) {
      const auto&                         pdsch_td_res = pdsch_td_list[time_resource];
      const cell_slot_resource_allocator& pdsch_alloc  = res_alloc[pdcch_slot + pdsch_td_res.k0];

      if (pdsch_alloc.result.dl.rar_grants.full()) {
        break;
      }
      if (not cell_cfg.is_dl_enabled(pdsch_alloc.slot) or
          pdsch_td_res.symbols.stop() > cell_cfg.get_nof_dl_symbol_per_slot(pdsch_alloc.slot) or
          pdsch_td_res.symbols.start() < ss_cfg.get_first_symbol_index() + coreset_duration) {
        // Not enough PDSCH symbols available.
        continue;
      }
      if (csi_helper::is_csi_rs_slot(cell_cfg, pdsch_alloc.slot)) {
        // CSI-RS and MsgB cannot be multiplexed.
        continue;
      }

      const crb_bitmap used_crbs      = pdsch_alloc.dl_res_grid.used_crbs(dl_scs, ra_crb_lims, pdsch_td_res.symbols);
      const auto       available_crbs = rb_helper::find_empty_interval_of_length(
          used_crbs, get_nof_pdsch_prbs_required(time_resource, nof_preambles).nof_prbs);

      unsigned nof_allocs = nof_preambles;
      while (nof_allocs != 0 and
             get_nof_pdsch_prbs_required(time_resource, nof_allocs).nof_prbs > available_crbs.length()) {
        --nof_allocs;
      }

      if (nof_allocs > max_nof_allocs) {
        max_nof_allocs       = nof_allocs;
        msgb_crbs            = available_crbs;
        pdsch_time_res_index = time_resource;
      }
    }
    if (max_nof_allocs == 0 or pdsch_time_res_index == pdsch_td_list.size()) {
      logger.debug("msgb-rnti={}: MsgB postponed. Cause: No PDSCH resources available", msgb.msgb_rnti);
      ++msgb_it;
      continue;
    }

    // Count SuccessRAR (crc_result == true) and FallbackRAR (crc_result != true) preambles.
    // Pending CRCs (nullopt) are treated as FallbackRAR at this point.
    unsigned nof_success  = 0;
    unsigned nof_fallback = 0;
    for (const auto& p : msgb.preambles) {
      if (p.crc_result.has_value() and *p.crc_result) {
        ++nof_success;
      } else {
        ++nof_fallback;
      }
    }

    // Determine how many FallbackRAR preambles we need Msg3 resources for, within the PDSCH capacity.
    // FallbackRAR preambles that cannot get Msg3 resources are dropped from this scheduling round.
    // SuccessRAR preambles are always included if PDSCH space allows.
    const unsigned nof_fallback_candidates = std::min(nof_fallback, max_nof_allocs);

    // -- Find Msg3 PUSCH candidates for FallbackRAR preambles --
    static_vector<msg3_alloc_candidate, MAX_GRANTS_PER_RAR> msg3_candidates;
    for (unsigned pusch_idx = 0, sz = pusch_td_list.size();
         pusch_idx != sz and msg3_candidates.size() < nof_fallback_candidates;
         ++pusch_idx) {
      const unsigned pusch_res_max_allocs = std::min(msg3_candidates.capacity() - msg3_candidates.size(),
                                                     nof_fallback_candidates - msg3_candidates.size());

      const unsigned msg3_delay =
          ra_helper::get_msg3_delay(init_ul_bwp.generic_params.scs, pusch_td_list[pusch_idx].k2) +
          cell_cfg.ntn_cs_koffset;
      const cell_slot_resource_allocator& msg3_alloc = res_alloc[pdcch_slot + msg3_delay];
      const unsigned                      start_ul_sym =
          NOF_OFDM_SYM_PER_SLOT_NORMAL_CP - cell_cfg.get_nof_ul_symbol_per_slot(msg3_alloc.slot);
      if (not cell_cfg.is_ul_enabled(msg3_alloc.slot) or pusch_td_list[pusch_idx].symbols.start() < start_ul_sym) {
        continue;
      }

      const unsigned list_space = msg3_alloc.result.ul.puschs.capacity() - msg3_alloc.result.ul.puschs.size();
      const unsigned max_allocs = std::min(pusch_res_max_allocs, list_space);
      if (max_allocs == 0) {
        continue;
      }

      const unsigned nof_rbs_per_msg3 = msg3_data[pusch_idx].pusch.rbs.type1().length();
      crb_bitmap     used_ul_crbs =
          msg3_alloc.ul_res_grid.used_crbs(init_ul_bwp.generic_params, pusch_td_list[pusch_idx].symbols);
      used_ul_crbs |= pucch_crbs;
      const crb_interval msg3_crbs =
          rb_helper::find_empty_interval_of_length(used_ul_crbs, nof_rbs_per_msg3 * max_allocs);
      const unsigned max_allocs_on_free_rbs = msg3_crbs.length() / nof_rbs_per_msg3;
      if (max_allocs_on_free_rbs == 0) {
        continue;
      }

      unsigned last_crb = msg3_crbs.start();
      for (unsigned i = 0; i != std::min(max_allocs, max_allocs_on_free_rbs); ++i) {
        msg3_alloc_candidate& candidate = msg3_candidates.emplace_back();
        candidate.crbs                  = {last_crb, last_crb + nof_rbs_per_msg3};
        candidate.pusch_td_res_index    = pusch_idx;
        last_crb += nof_rbs_per_msg3;
      }
    }

    // Determine the effective number of grants we can schedule.
    const unsigned nof_fallback_to_sched = msg3_candidates.size();
    const unsigned nof_success_to_sched =
        (nof_fallback_to_sched < max_nof_allocs) ? std::min(nof_success, max_nof_allocs - nof_fallback_to_sched) : 0U;
    const unsigned effective_nof_sched = nof_success_to_sched + nof_fallback_to_sched;

    if (effective_nof_sched == 0) {
      logger.debug("msgb-rnti={}: MsgB postponed. Cause: No PUSCH resources available for Msg3", msgb.msgb_rnti);
      ++msgb_it;
      continue;
    }

    // -- Allocate PDCCH for MsgB-RNTI --
    cell_slot_resource_allocator& pdcch_alloc = res_alloc[pdcch_slot];
    pdcch_dl_information*         pdcch = pdcch_sch.alloc_dl_pdcch_common(pdcch_alloc, msgb.msgb_rnti, ss_id, aggr_lvl);
    if (pdcch == nullptr) {
      logger.debug("msgb-rnti={}: MsgB postponed. Cause: No PDCCH space available", msgb.msgb_rnti);
      ++msgb_it;
      continue;
    }

    // -- Fill DCI and PDSCH --
    msgb_crbs.resize(get_nof_pdsch_prbs_required(pdsch_time_res_index, effective_nof_sched).nof_prbs);
    cell_slot_resource_allocator& pdsch_alloc = res_alloc[pdcch_slot + pdsch_td_list[pdsch_time_res_index].k0];

    build_dci_f1_0_ra_rnti(pdcch->dci, init_dl_bwp, msgb_crbs, pdsch_time_res_index, sched_cfg.rar_mcs_index);
    pdsch_alloc.dl_res_grid.fill(grant_info{dl_scs, pdsch_td_list[pdsch_time_res_index].symbols, msgb_crbs});

    rar_information& msgb_rar = pdsch_alloc.result.dl.rar_grants.emplace_back();
    build_pdsch_f1_0_ra_rnti(msgb_rar.pdsch_cfg,
                             get_nof_pdsch_prbs_required(pdsch_time_res_index, effective_nof_sched).tbs_bytes,
                             msgb.msgb_rnti,
                             cell_cfg,
                             pdcch->dci.as_ra_rnti_f1_0(),
                             msgb_crbs,
                             rar_data[pdsch_time_res_index].dmrs_info);

    // -- Fill per-preamble grants --
    // Iterate preambles in order: schedule SuccessRAR up to nof_success_to_sched,
    // FallbackRAR up to nof_fallback_to_sched (limited by msg3_candidates).
    for (unsigned i = 0, success_count = 0, fallback_count = 0; i != nof_preambles; ++i) {
      pending_msgb_alloc::preamble_ctx& pctx = msgb.preambles[i];

      if (pctx.crc_result.has_value() and *pctx.crc_result and success_count < nof_success_to_sched) {
        // SuccessRAR: UE's MsgA PUSCH decoded — 2-step RACH completes, no Msg3 needed.
        if (msgb_rar.grants.full()) {
          logger.error("msgb-rnti={}: SuccessRAR grant for tc-rnti={} dropped. Cause: No space in grant list",
                       msgb.msgb_rnti,
                       pctx.info.tc_rnti);
          break;
        }
        rar_ul_grant& g     = msgb_rar.grants.emplace_back();
        g.rapid             = pctx.info.preamble_id;
        g.ta                = pctx.info.time_advance.to_Ta(init_ul_bwp.generic_params.scs);
        g.temp_crnti        = pctx.info.tc_rnti;
        g.freq_hop_flag     = false;
        g.mcs               = sched_cfg.msg3_mcs_index;
        g.tpc               = 0;
        g.csi_req           = false;
        g.type              = rar_ul_grant::two_step_info{true};
        pctx.msgb_scheduled = true;
        ++success_count;

      } else if (pctx.crc_result.has_value() and not *pctx.crc_result and fallback_count < nof_fallback_to_sched) {
        // FallbackRAR: MsgA PUSCH not decoded (or CRC pending at window boundary) — UE falls back to Msg3.
        if (msgb_rar.grants.full()) {
          logger.error("msgb-rnti={}: FallbackRAR grant for tc-rnti={} dropped. Cause: No space in grant list",
                       msgb.msgb_rnti,
                       pctx.info.tc_rnti);
          break;
        }

        const msg3_alloc_candidate& msg3_candidate = msg3_candidates[fallback_count];
        const auto&                 pusch_res      = pusch_td_list[msg3_candidate.pusch_td_res_index];
        const unsigned              msg3_delay =
            ra_helper::get_msg3_delay(init_ul_bwp.generic_params.scs, pusch_res.k2) + cell_cfg.ntn_cs_koffset;
        cell_slot_resource_allocator& msg3_alloc = res_alloc[pdcch_slot + msg3_delay];
        const vrb_interval            vrbs       = ul_crb_to_vrb(cell_cfg, msg3_candidate.crbs);

        // Create pending Msg3 entry (HARQ entity + preamble info).
        const uint16_t msg3_ring_idx = get_msg3_ring_key(pctx.info.tc_rnti);
        if (not pending_msg3s.emplace(msg3_ring_idx)) {
          logger.warning("pci={} tc-rnti={}: Cannot create Msg3 entry for FallbackRAR. Cause: TC-RNTI already in use",
                         cell_cfg.params.pci,
                         pctx.info.tc_rnti);
          pctx.msgb_scheduled = true;
          ++fallback_count;
          continue;
        }
        auto& pending_msg3    = pending_msg3s[msg3_ring_idx];
        pending_msg3.preamble = pctx.info;
        pending_msg3.harq_ent = ra_harqs.add_ue(to_du_ue_index(msg3_ring_idx), pctx.info.tc_rnti, 1, 1);
        std::optional<ul_harq_process_handle> h_ul =
            pending_msg3.harq_ent.alloc_ul_harq(msg3_alloc.slot, sched_cfg.max_nof_msg3_harq_retxs);
        ocudu_sanity_check(h_ul.has_value(), "Pending Msg3 HARQ must be available for FallbackRAR");

        // Fill rar_ul_grant.
        rar_ul_grant& g            = msgb_rar.grants.emplace_back();
        g.rapid                    = pctx.info.preamble_id;
        g.ta                       = pctx.info.time_advance.to_Ta(init_ul_bwp.generic_params.scs);
        g.temp_crnti               = pctx.info.tc_rnti;
        g.freq_hop_flag            = false;
        g.time_resource_assignment = msg3_candidate.pusch_td_res_index;
        g.freq_resource_assignment = ra_frequency_type1_get_riv(
            ra_frequency_type1_configuration{init_ul_bwp.generic_params.crbs.length(), vrbs.start(), vrbs.length()});
        g.mcs = sched_cfg.msg3_mcs_index;
        // Determine TPC command based on Table 8.2-2, TS 38.213.
        g.tpc     = (init_ul_bwp.pusch_cfg_common->msg3_delta_power.value() + 6) / 2;
        g.csi_req = false;
        g.type    = rar_ul_grant::two_step_info{false};

        // Allocate Msg3 RBs.
        msg3_alloc.ul_res_grid.fill(grant_info{init_ul_bwp.generic_params.scs,
                                               pusch_td_list[msg3_candidate.pusch_td_res_index].symbols,
                                               msg3_candidate.crbs});

        // Fill Msg3 PUSCH.
        ul_sched_info& ul_info     = msg3_alloc.result.ul.puschs.emplace_back();
        ul_info.context.ue_index   = INVALID_DU_UE_INDEX;
        ul_info.context.ss_id      = init_dl_bwp.pdcch_common.ra_search_space_id;
        ul_info.context.nof_retxs  = 0;
        ul_info.context.msg3_delay = msg3_delay;
        ul_info.pusch_cfg          = msg3_data[msg3_candidate.pusch_td_res_index].pusch;
        ul_info.pusch_cfg.rnti     = pctx.info.tc_rnti;
        ul_info.pusch_cfg.rbs      = vrbs;
        ul_info.pusch_cfg.rv_index = 0;
        ul_info.pusch_cfg.new_data = true;
        h_ul->save_grant_params(ul_harq_alloc_context{dci_ul_rnti_config_type::tc_rnti_f0_0}, ul_info.pusch_cfg);

        pctx.msgb_scheduled = true;
        ++fallback_count;
      }
    }

    // Erase scheduled preambles.
    auto new_end =
        std::remove_if(msgb.preambles.begin(), msgb.preambles.end(), [](const auto& p) { return p.msgb_scheduled; });
    msgb.preambles.erase(new_end, msgb.preambles.end());

    if (msgb.preambles.empty()) {
      msgb_it = pending_msgbs.erase(msgb_it);
    } else {
      ++msgb_it;
    }
  }
}

void ra_scheduler::reserve_msga_pusch_rbs(cell_resource_allocator& res_alloc)
{
  if (not cell_cfg.init_bwp.ul.rach_common()->two_step_rach_cfg.has_value()) {
    // Two-step RACH is not active.
    return;
  }

  // Helper function to fill grid resources.
  auto fill_grid_in_slot = [this, &res_alloc](unsigned ul_lookahead_slots) {
    auto& pusch_alloc = res_alloc[ul_lookahead_slots];

    if (not cached_init_bwp_info->is_msga_pusch_slot(pusch_alloc.slot)) {
      // MsgA PUSCH does not fall in this slot.
      return;
    }

    // Fill resource grid.
    pusch_alloc.ul_res_grid.fill(cached_init_bwp_info->reserved_msga_pusch_space);
  };

  if (OCUDU_UNLIKELY(first_slot_flag)) {
    first_slot_flag = false;
    for (unsigned lookahead_slots = 0; lookahead_slots != res_alloc.max_ul_slot_alloc_delay; ++lookahead_slots) {
      fill_grid_in_slot(lookahead_slots);
    }
  }

  fill_grid_in_slot(res_alloc.max_ul_slot_alloc_delay);
}

sch_prbs_tbs ra_scheduler::get_nof_pdsch_prbs_required(unsigned time_res_idx, unsigned nof_ul_grants) const
{
  ocudu_assert(nof_ul_grants > 0, "Invalid number of UL grants");

  return rar_data[time_res_idx].prbs_tbs_per_nof_grants
      [std::min(nof_ul_grants, static_cast<unsigned>(rar_data[time_res_idx].prbs_tbs_per_nof_grants.size())) - 1];
}

void ra_scheduler::log_postponed_rar(const pending_rar_alloc&  rar,
                                     const char*               cause_str,
                                     std::optional<slot_point> sl) const
{
  if (sl.has_value()) {
    logger.debug("RAR allocation for ra-rnti={} was postponed. Cause: {} at slot={}", rar.ra_rnti, cause_str, *sl);
  } else {
    logger.debug("RAR allocation for ra-rnti={} was postponed. Cause: {}", rar.ra_rnti, cause_str);
  }
}
