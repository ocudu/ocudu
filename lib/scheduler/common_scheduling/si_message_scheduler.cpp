// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "si_message_scheduler.h"
#include "../support/dci_builder.h"
#include "../support/dmrs_helpers.h"
#include "../support/pdcch/search_space_helper.h"
#include "../support/pdsch/pdsch_default_time_allocation.h"
#include "../support/pdsch/pdsch_resource_allocation.h"
#include "../support/prbs_calculator.h"
#include "../support/sch_pdu_builder.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/scheduler/support/rb_helper.h"

using namespace ocudu;

si_message_scheduler::si_message_scheduler(const cell_configuration&   cfg_,
                                           pdcch_resource_allocator&   pdcch_sch_,
                                           const si_scheduling_config& si_sched_cfg_) :
  expert_cfg(cfg_.expert_cfg.si),
  cell_cfg(cfg_),
  pdcch_sch(pdcch_sch_),
  si_sched_cfg(si_sched_cfg_),
  logger(ocudulog::fetch_basic_logger("SCHED"))
{
  pending_messages.resize(si_sched_cfg.si_messages.size());
  for (unsigned i = 0, e = pending_messages.size(); i != e; ++i) {
    pending_messages[i].active  = not si_sched_cfg.si_messages[i].requires_activation;
    pending_messages[i].msg_len = si_sched_cfg.si_messages[i].msg_len;
  }
}

void si_message_scheduler::run_slot(cell_slot_resource_allocator& res_grid, slot_point_extended sl_tx_ext)
{
  if (si_sched_cfg.si_messages.empty()) {
    return;
  }

  // Check for SI message updates
  update_si_message_windows(sl_tx_ext);

  // Schedule SI messages that are within the window for tx.
  schedule_pending_si_messages(res_grid);
}

void si_message_scheduler::stop()
{
  // Clear all windows.
  for (unsigned i = 0; i != pending_messages.size(); ++i) {
    pending_messages[i]         = {};
    pending_messages[i].active  = not si_sched_cfg.si_messages[i].requires_activation;
    pending_messages[i].msg_len = si_sched_cfg.si_messages[i].msg_len;
  }
}

void si_message_scheduler::handle_si_message_update_indication(unsigned                    new_version,
                                                               const si_scheduling_config& new_si_sched_cfg)
{
  // Update SI messages.
  version      = new_version;
  si_sched_cfg = new_si_sched_cfg;
  pending_messages.resize(si_sched_cfg.si_messages.size());

  // Reset window and transmission counters.
  std::fill(pending_messages.begin(), pending_messages.end(), message_window_context{});
  for (unsigned i = 0, e = pending_messages.size(); i != e; ++i) {
    pending_messages[i].active  = not si_sched_cfg.si_messages[i].requires_activation;
    pending_messages[i].msg_len = si_sched_cfg.si_messages[i].msg_len;
  }
}

void si_message_scheduler::activate_si_message(unsigned                si_msg_idx,
                                               slot_point_extended     activation_slot,
                                               std::optional<unsigned> nof_segments,
                                               units::bytes            msg_len)
{
  ocudu_assert(si_msg_idx < pending_messages.size(), "Invalid SI-message index");

  message_window_context& ctxt = pending_messages[si_msg_idx];
  ctxt.active                  = true;
  ctxt.msg_len                 = msg_len;

  if (not nof_segments.has_value()) {
    // Broadcast indefinitely (test_mode-configured content); never auto-deactivates.
    ctxt.active_until.reset();
    return;
  }

  // Ensure single-round PWS delivery reaches every UE, not just the ones whose paging occasion happens to land
  // early within the notification window. As per TS 38.304, idle/inactive UEs only monitor their own paging
  // occasion once per DRX cycle, so the etwsAndCmasIndication short message is repeated across a full default
  // paging cycle (see si_scheduler::try_handle_pending_pws_request). A UE notified near the end of that window
  // must still get a full cycle of segments afterwards, so keep broadcasting for the notification window's
  // duration plus one extra full segment cycle.
  const unsigned default_paging_cycle_rfs =
      static_cast<unsigned>(cell_cfg.params.dl_cfg_common.pcch_cfg.default_paging_cycle);
  const unsigned one_segment_cycle_rfs =
      nof_segments.value() * si_sched_cfg.si_messages[si_msg_idx].period_radio_frames;
  const unsigned active_duration_rfs = default_paging_cycle_rfs + one_segment_cycle_rfs;

  ctxt.active_until = activation_slot + active_duration_rfs * activation_slot.nof_slots_per_frame();
}

void si_message_scheduler::update_si_message_windows(slot_point_extended sl_tx_ext)
{
  const slot_point sl_tx = sl_tx_ext.without_hyper_sfn();
  const unsigned   sfn   = sl_tx.sfn();

  for (unsigned i = 0; i != pending_messages.size(); ++i) {
    const si_message_scheduling_config& si_msg = si_sched_cfg.si_messages[i];

    if (not pending_messages[i].window.empty()) {
      // SI message is already in the window. Check for window end.
      if (pending_messages[i].window.stop() <= sl_tx) {
        if (pending_messages[i].nof_tx_in_current_window == 0) {
          logger.warning("SI message {} window ended, but no transmissions were made.", i);
        }
        pending_messages[i].window                   = {};
        pending_messages[i].nof_tx_in_current_window = 0;
      }
      continue;
    }

    if (not pending_messages[i].active) {
      // SI-message requires activation and is currently dormant. Do not open a new window until it is activated.
      continue;
    }

    if (pending_messages[i].active_until.has_value() and pending_messages[i].active_until.value() <= sl_tx_ext) {
      // The activation's deadline has passed. Go back to dormant; any window already in-flight (handled above)
      // is left to complete normally.
      pending_messages[i].active = false;
      continue;
    }

    // Check for SI window start, as per TS 38.331, Section 5.2.2.3.2.

    // 2> For the concerned SI message, determine the number n which corresponds to the order of entry in the list of SI
    // messages configured by schedulingInfoList in si-SchedulingInfo in SIB1.
    const unsigned n = i + 1;

    // 3> Determine the integer value x = (n – 1) × w, where w is the si-WindowLength.
    unsigned x = (n - 1) * si_sched_cfg.si_window_len_slots;
    if (si_msg.si_window_position.has_value()) {
      // 3> Determine the integer value x = (si-WindowPosition -1) × w, where w is the si-WindowLength. See TS 38 331
      // V17.0.0.
      x = (si_msg.si_window_position.value() - 1) * si_sched_cfg.si_window_len_slots;
    }

    // 3> The SI-window starts at the slot #a, where a = x mod N, in the radio frame for which SFN mod T = FLOOR(x/N),
    // where T is the si-Periodicity of the concerned SI message and N is the number of slots in a radio frame as
    // specified in TS 38.213.
    const unsigned N = sl_tx.nof_slots_per_frame();
    const unsigned a = x % N;
    if (sl_tx.slot_index() != a) {
      continue;
    }

    const unsigned T = si_msg.period_radio_frames;
    if (sfn % T != (x / N)) {
      continue;
    }

    // SI window start detected.
    pending_messages[i].window = {sl_tx, sl_tx + si_sched_cfg.si_window_len_slots};

    // Reset the trasnmission counter for the new window.
    pending_messages[i].nof_tx_in_current_window = 0;
  }
}

void si_message_scheduler::schedule_pending_si_messages(cell_slot_resource_allocator& res_grid)
{
  for (unsigned i = 0; i != pending_messages.size(); ++i) {
    message_window_context& si_ctxt = pending_messages[i];

    if (si_ctxt.window.empty() or si_ctxt.nof_tx_in_current_window > 0) {
      // SI window is inactive or SI message was already transmitted.
      continue;
    }

    // Check if the searchSpaceOtherSystemInformation has monitored PDCCH candidates.
    const search_space_id ss_id =
        cell_cfg.params.dl_cfg_common.init_dl_bwp.pdcch_common.other_si_search_space_id.value_or(
            cell_cfg.params.dl_cfg_common.init_dl_bwp.pdcch_common.sib1_search_space_id);
    const search_space_configuration& ss = cell_cfg.params.dl_cfg_common.init_dl_bwp.pdcch_common.search_spaces[ss_id];
    if (not pdcch_helper::is_pdcch_monitoring_active(res_grid.slot, ss)) {
      continue;
    }

    if (allocate_si_message(i, res_grid)) {
      // Increment the transmission counters.
      ++si_ctxt.nof_tx_in_current_window;
      ++si_ctxt.total_nof_tx;
    }
  }
}

bool si_message_scheduler::allocate_si_message(unsigned si_message, cell_slot_resource_allocator& res_grid)
{
  static constexpr unsigned time_resource = 0;
  static constexpr unsigned nof_layers    = 1;
  // As per Section 5.1.3.2, TS 38.214, nof_oh_prb = 0 if PDSCH is scheduled by PDCCH with a CRC scrambled by SI-RNTI.
  static constexpr unsigned nof_oh_prb = 0;

  // Note: For SI-messages currently PWS-activated, this reflects the real (activation-time) content length.
  const units::bytes si_msg_payload_size = pending_messages[si_message].msg_len;

  const auto& pdsch_td_res_alloc_list =
      get_si_rnti_type0A_common_pdsch_time_domain_list(cell_cfg.params.dl_cfg_common.init_dl_bwp.pdsch_common,
                                                       cell_cfg.params.dl_cfg_common.init_dl_bwp.generic_params.cp,
                                                       cell_cfg.params.dmrs_typeA_pos);
  const ofdm_symbol_range si_ofdm_symbols = pdsch_td_res_alloc_list[time_resource].symbols;
  const unsigned          nof_symb_sh     = si_ofdm_symbols.length();

  // Generate dmrs information to be passed to (i) the fnc that computes number of RE used for DMRS per RB and (ii) to
  // the fnc that fills the DCI.
  const dmrs_information dmrs_info = make_dmrs_info_common(
      pdsch_td_res_alloc_list, time_resource, cell_cfg.params.pci, cell_cfg.params.dmrs_typeA_pos);

  // Compute the number of RBs necessary for the allocation.
  const sch_mcs_description mcs_descr   = pdsch_mcs_get_config(pdsch_mcs_table::qam64, expert_cfg.si_message_mcs_index);
  const sch_prbs_tbs        si_prbs_tbs = get_nof_prbs(prbs_calculator_sch_config{si_msg_payload_size.value(),
                                                                           nof_symb_sh,
                                                                           calculate_nof_dmrs_per_rb(dmrs_info),
                                                                           nof_oh_prb,
                                                                           mcs_descr,
                                                                           nof_layers});

  // > Find available RBs in PDSCH for SI message BCCH grant.
  const search_space_id ss_id =
      cell_cfg.params.dl_cfg_common.init_dl_bwp.pdcch_common.other_si_search_space_id.value_or(
          cell_cfg.params.dl_cfg_common.init_dl_bwp.pdcch_common.sib1_search_space_id);
  crb_interval si_crbs;
  {
    const crb_interval crb_lims =
        pdsch_helper::get_ra_crb_limits_common(cell_cfg.params.dl_cfg_common.init_dl_bwp, ss_id);
    const unsigned    nof_si_rbs = si_prbs_tbs.nof_prbs;
    const crb_bitmap& used_crbs  = res_grid.dl_res_grid.used_crbs(
        cell_cfg.params.dl_cfg_common.init_dl_bwp.generic_params.scs, crb_lims, si_ofdm_symbols);
    si_crbs = rb_helper::find_empty_interval_of_length(used_crbs, nof_si_rbs);
    if (si_crbs.length() < nof_si_rbs) {
      // early exit
      logger.info("Skipping SI message scheduling. Cause: Not enough PDSCH space for SI Message {}", si_message);
      return false;
    }
  }

  // > Allocate DCI_1_0 for SI message on PDCCH.
  pdcch_dl_information* pdcch =
      pdcch_sch.alloc_dl_pdcch_common(res_grid, rnti_t::SI_RNTI, ss_id, expert_cfg.si_message_dci_aggr_lev);
  if (pdcch == nullptr) {
    logger.info("Skipping SI message scheduling. Cause: Not enough PDCCH space for SI Message {}", si_message);
    return false;
  }

  // > Now that we are sure there is space in both PDCCH and PDSCH, set SI CRBs as used.
  res_grid.dl_res_grid.fill(
      grant_info{cell_cfg.params.dl_cfg_common.init_dl_bwp.generic_params.scs, si_ofdm_symbols, si_crbs});

  // > Delegate filling SI message grants to helper function.
  fill_si_grant(
      res_grid, si_message, si_crbs, time_resource, dmrs_info, si_prbs_tbs.tbs_bytes, pending_messages[si_message]);
  return true;
}

void si_message_scheduler::fill_si_grant(cell_slot_resource_allocator& res_grid,
                                         unsigned                      si_message,
                                         crb_interval                  si_crbs_grant,
                                         uint8_t                       time_resource,
                                         const dmrs_information&       dmrs_info,
                                         units::bytes                  tbs,
                                         const message_window_context& message_context)
{
  // System information indicator for SI message as per TS 38.212, Section 7.3.1.2.1 and Table 7.3.1.2.1-2.

  const auto& pdsch_td_res_alloc_list =
      get_si_rnti_type0A_common_pdsch_time_domain_list(cell_cfg.params.dl_cfg_common.init_dl_bwp.pdsch_common,
                                                       cell_cfg.params.dl_cfg_common.init_dl_bwp.generic_params.cp,
                                                       cell_cfg.params.dmrs_typeA_pos);

  // Fill SI-message DCI.
  static constexpr unsigned si_indicator = 1;
  auto&                     si_pdcch     = res_grid.result.dl.dl_pdcchs.back();
  build_dci_f1_0_si_rnti(si_pdcch.dci,
                         cell_cfg.params.dl_cfg_common.init_dl_bwp,
                         si_crbs_grant,
                         time_resource,
                         expert_cfg.sib1_mcs_index,
                         si_indicator);

  // Add SIBs of the SI-message to list of SIB information to pass to lower layers.
  sib_information& si = res_grid.result.dl.bc.sibs.emplace_back();
  si.si_indicator     = sib_information::si_indicator_type::other_si;
  si.si_msg_index     = si_message;
  si.version          = version;
  si.nof_txs          = message_context.total_nof_tx;

  // Determine if the SI message has already been transmitted within this window or not.
  si.is_repetition = (message_context.nof_tx_in_current_window != 0);

  // Fill PDSCH configuration.
  pdsch_information& pdsch = si.pdsch_cfg;
  build_pdsch_f1_0_si_rnti(pdsch,
                           cell_cfg,
                           tbs,
                           si_pdcch.dci.as_si_rnti_f1_0(),
                           si_crbs_grant,
                           pdsch_td_res_alloc_list[time_resource].symbols,
                           dmrs_info);
}
