// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "pucch_allocator_impl.h"
#include "pucch_allocator_helpers.h"
#include "pucch_resource_manager.h"
#include "ocudu/ocudulog/log_channel.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/ran/csi_report/csi_report_config_helpers.h"
#include "ocudu/ran/pucch/pucch_configuration.h"
#include "ocudu/ran/pucch/pucch_info.h"
#include "ocudu/ran/pucch/pucch_uci_bits.h"
#include "ocudu/ran/resource_allocation/ofdm_symbol_range.h"
#include "ocudu/scheduler/config/pucch_default_resource.h"
#include "ocudu/scheduler/config/serving_cell_config_factory.h"
#include "ocudu/scheduler/resource_grid_util.h"
#include "ocudu/scheduler/result/sched_result.h"
#include "ocudu/support/ocudu_assert.h"
#include "fmt/std.h"
#include <algorithm>
#include <fmt/format.h>
#include <string>
#include <variant>

using namespace ocudu;

struct pucch_allocator_impl::alloc_context {
  enum class alloc_type { common_harq_ack, common_and_ded_harq_ack, ded_harq_ack, sr, csi };
  alloc_type type;
  rnti_t     rnti;
  slot_point slot;

  const char* type_str() const
  {
    switch (type) {
      case alloc_type::common_harq_ack:
        return "common HARQ-ACK";
      case alloc_type::common_and_ded_harq_ack:
        return "common+ded HARQ-ACK";
      case alloc_type::ded_harq_ack:
        return "ded HARQ-ACK";
      case alloc_type::sr:
        return "SR";
      case alloc_type::csi:
        return "CSI";
      default:
        return "unknown";
    }
  }

  /// Helper for logging PUCCH allocation skip events.
  void log_skipped_alloc(ocudulog::log_channel& log, const char* cause) const
  {
    log("PUCCH {} allocation skipped (rnti={} slot={}). Cause: {}", type_str(), rnti, slot, cause);
  }

  /// Helper for logging PUCCH PDU allocation events.
  void log_pdu_alloc(ocudulog::log_channel& log, const pucch_info& pucch_pdu, const char* res_type, bool updated) const
  {
    switch (pucch_pdu.format()) {
      case pucch_format::FORMAT_0: {
        const auto& f0 = std::get<pucch_resource::f0_config>(pucch_pdu.res->format_params);
        log("PUCCH PDU on {} F0 resource (rnti={} slot={}, updated={}): prbs={} prb2={} syms={} cs={} uci_bits={}",
            res_type,
            rnti,
            slot,
            updated,
            pucch_pdu.res->prbs(),
            pucch_pdu.res->second_hop_prb,
            pucch_pdu.res->syms,
            f0.initial_cyclic_shift,
            pucch_pdu.uci_bits);
      } break;
      case pucch_format::FORMAT_1: {
        const auto& f1 = std::get<pucch_resource::f1_config>(pucch_pdu.res->format_params);
        log("PUCCH PDU on {} F1 resource (rnti={} slot={}, updated={}): prbs={} prb2={} syms={} cs={} occ={} "
            "uci_bits={}",
            res_type,
            rnti,
            slot,
            updated,
            pucch_pdu.res->prbs(),
            pucch_pdu.res->second_hop_prb,
            pucch_pdu.res->syms,
            f1.initial_cyclic_shift,
            f1.time_domain_occ,
            pucch_pdu.uci_bits);
      } break;
      case pucch_format::FORMAT_2: {
        log("PUCCH PDU on {} F2 resource (rnti={} slot={}, updated={}): prbs={} prbs2={} syms={} uci_bits={}",
            res_type,
            rnti,
            slot,
            updated,
            pucch_pdu.res->prbs(),
            pucch_pdu.res->second_hop_prb,
            pucch_pdu.res->syms,
            pucch_pdu.uci_bits);
      } break;
      case pucch_format::FORMAT_3: {
        log("PUCCH PDU on {} F3 resource (rnti={} slot={}, updated={}): prbs={} prbs2={} syms={} uci_bits={}",
            res_type,
            rnti,
            slot,
            updated,
            pucch_pdu.res->prbs(),
            pucch_pdu.res->second_hop_prb,
            pucch_pdu.res->syms,
            pucch_pdu.uci_bits);
      } break;
      case pucch_format::FORMAT_4: {
        const auto& f4 = std::get<pucch_resource::f4_config>(pucch_pdu.res->format_params);
        log("PUCCH PDU on {} F4 resource (rnti={} slot={}, updated={}): prbs={} prbs2={} syms={} occ={} uci_bits={}",
            res_type,
            rnti,
            slot,
            updated,
            pucch_pdu.res->prbs(),
            pucch_pdu.res->second_hop_prb,
            pucch_pdu.res->syms,
            fmt::underlying(f4.occ_index),
            pucch_pdu.uci_bits);
      } break;
      default:
        ocudu_assertion_failure("Invalid PUCCH format");
    }
  }
};

// Helper function to create the CSI report configuration from the cell configuration, if CSI is configured in the cell.
static std::optional<csi_report_configuration> get_csi_report_cfg(const ran_cell_config& cell_cfg)
{
  if (not cell_cfg.init_bwp.csi.has_value()) {
    return std::nullopt;
  }

  // Note that even though the CSI-MeasConfig is not equal accross UEs, the parameters needed for the CSI report
  // configuration are the same, so we can just compute it from the cell configuration.
  // TODO: we should not need the complete CSI-MeasConfig to get the csi_report_configuration.
  return create_csi_report_configuration(
      config_helpers::make_default_ue_cell_config(cell_cfg).serv_cell_cfg.csi_meas_cfg.value());
}

//////////////    Public functions       //////////////

pucch_allocator_impl::pucch_allocator_impl(const cell_configuration& cell_cfg_,
                                           unsigned                  max_pucchs_per_slot,
                                           unsigned                  max_ul_grants_per_slot_) :
  cell_cfg(cell_cfg_),
  max_pucch_grants_per_slot(max_pucchs_per_slot),
  max_ul_grants_per_slot(max_ul_grants_per_slot_),
  res_params(cell_cfg.params.init_bwp.pucch.resources),
  csi_report_cfg(get_csi_report_cfg(cell_cfg.params)),
  resource_manager(cell_cfg_),
  logger(ocudulog::fetch_basic_logger("SCHED"))
{
  // The ring must be at least 1 slot larger than the max. UL allocation delay, to take into account the current slot.
  const unsigned ring_size = get_allocator_ring_size_gt_min(get_max_slot_ul_alloc_delay(cell_cfg.ntn_cs_koffset) + 1);
  slots_ctx.resize(ring_size);
}

pucch_allocator_impl::~pucch_allocator_impl() = default;

void pucch_allocator_impl::slot_indication(slot_point sl_tx)
{
  // If last_sl_ind is not valid (not initialized), then the check sl_tx == last_sl_ind + 1 does not matter.
  ocudu_sanity_check(not last_sl_ind.valid() or sl_tx == last_sl_ind + 1, "Detected a skipped slot");

  // Update Slot.
  last_sl_ind = sl_tx;

  resource_manager.slot_indication(sl_tx);

  // Clear previous slot PUCCH grants allocations.
  slots_ctx[(sl_tx - 1).to_uint()].clear();
}

void pucch_allocator_impl::stop()
{
  resource_manager.stop();
  for (auto& ctx : slots_ctx) {
    ctx.clear();
  }
  last_sl_ind = {};
}

std::optional<unsigned> pucch_allocator_impl::alloc_common_harq_ack(cell_resource_allocator&    res_alloc,
                                                                    rnti_t                      tcrnti,
                                                                    unsigned                    k0,
                                                                    unsigned                    k1,
                                                                    const pdcch_dl_information& dci_info)
{
  // Get the slot allocation grid considering the PDSCH delay (k0) and the PUCCH delay wrt PDSCH (k1).
  cell_slot_resource_allocator& pucch_slot_alloc = res_alloc[k0 + k1 + res_alloc.cfg.ntn_cs_koffset];
  auto&                         slot_ctx         = slots_ctx[pucch_slot_alloc.slot.to_uint()];
  alloc_context                 alloc_ctx{alloc_context::alloc_type::common_harq_ack, tcrnti, pucch_slot_alloc.slot};

  ue_grants* existing_ue_grants = slot_ctx.find_ue_grants(tcrnti);
  if (not can_allocate_pucch(pucch_slot_alloc, existing_ue_grants, alloc_ctx)) {
    return std::nullopt;
  }

  if (not can_allocate_fallback_pucch(pucch_slot_alloc, existing_ue_grants, alloc_ctx)) {
    return std::nullopt;
  }

  // The common grant won't ever be multiplexed with SR/CSI grants from the same UE
  if (not is_there_space_for_new_pucch_grants(pucch_slot_alloc.result, 1U)) {
    alloc_ctx.log_skipped_alloc(logger.info, "max number of UL/PUCCH grants reached");
    return std::nullopt;
  }

  // Try to get an available PUCCH common resource for HARQ-ACK.
  std::optional<pucch_common_params> pucch_res =
      alloc_pucch_common_res_harq(pucch_slot_alloc, dci_info.ctx, alloc_ctx.rnti);

  if (not pucch_res.has_value()) {
    alloc_ctx.log_skipped_alloc(logger.debug, "no resources available");
    return std::nullopt;
  }

  // Fill scheduler output.
  pucch_info& pucch_info = pucch_slot_alloc.result.ul.pucchs.emplace_back();
  fill_common_pdu(pucch_info, cell_cfg.bwp_res[to_bwp_id(0)].ul().pucch.common[pucch_res.value().r_pucch], tcrnti);
  unsigned pucch_res_indicator = pucch_res.value().pucch_res_indicator;

  if (existing_ue_grants != nullptr) {
    existing_ue_grants->has_common_pucch = true;
  } else {
    slot_ctx.ue_grants_map.emplace(tcrnti, ue_grants{.has_common_pucch = true});
  }

  return pucch_res_indicator;
}

std::optional<unsigned> pucch_allocator_impl::alloc_common_and_ded_harq_ack(cell_resource_allocator&     res_alloc,
                                                                            rnti_t                       rnti,
                                                                            const ue_cell_configuration& ue_cell_cfg,
                                                                            unsigned                     k0,
                                                                            unsigned                     k1,
                                                                            const pdcch_dl_information&  dci_info)
{
  // Get the slot allocation grid considering the PDSCH delay (k0) and the PUCCH delay wrt PDSCH (k1).
  cell_slot_resource_allocator& pucch_slot_alloc = res_alloc[k0 + k1 + res_alloc.cfg.ntn_cs_koffset];
  auto&                         slot_ctx         = slots_ctx[pucch_slot_alloc.slot.to_uint()];
  alloc_context alloc_ctx{alloc_context::alloc_type::common_and_ded_harq_ack, rnti, pucch_slot_alloc.slot};

  ue_grants* existing_ue_grants = slot_ctx.find_ue_grants(rnti);
  if (not can_allocate_pucch(pucch_slot_alloc, existing_ue_grants, alloc_ctx)) {
    return std::nullopt;
  }

  if (not can_allocate_fallback_pucch(pucch_slot_alloc, existing_ue_grants, alloc_ctx)) {
    return std::nullopt;
  }

  // Keep track of whether a new ue_grants has been added; in case the function fails, remove it before exiting.
  const bool new_ue_grants_added = existing_ue_grants == nullptr;
  ue_grants& current_grants =
      new_ue_grants_added ? slot_ctx.ue_grants_map.emplace(rnti, ue_grants{}).first->second : *existing_ue_grants;

  // Find a couple of PUCCH resources (1 common, 1 dedicated) that are (i) are available and that (ii) have the same
  // PUCCH resource indicator.
  std::optional<pucch_common_params> pucch_common_info =
      find_common_and_ded_harq_res_available(pucch_slot_alloc, current_grants, ue_cell_cfg, dci_info.ctx, alloc_ctx);

  if (pucch_common_info.has_value()) {
    compute_pucch_common_params_and_alloc(pucch_slot_alloc, rnti, pucch_common_info.value());
    return pucch_common_info.value().pucch_res_indicator;
  }

  if (new_ue_grants_added) {
    // Remove the newly added ue_grants, since the allocation failed.
    slot_ctx.ue_grants_map.erase(rnti);
  }

  alloc_ctx.log_skipped_alloc(logger.debug,
                              "no resource indicator available for both common and dedicated PUCCH resources");
  return std::nullopt;
}

std::optional<unsigned> pucch_allocator_impl::alloc_ded_harq_ack(cell_resource_allocator&     res_alloc,
                                                                 rnti_t                       crnti,
                                                                 const ue_cell_configuration& ue_cell_cfg,
                                                                 unsigned                     k0,
                                                                 unsigned                     k1)
{
  // NOTE: This function does not check whether there are PUSCH grants allocated for the same UE. The check needs to
  // be performed by the caller.

  // Get the slot allocation grid considering the PDSCH delay (k0) and the PUCCH delay wrt PDSCH (k1).
  cell_slot_resource_allocator& pucch_slot_alloc = res_alloc[k0 + k1 + res_alloc.cfg.ntn_cs_koffset];
  auto&                         slot_ctx         = slots_ctx[pucch_slot_alloc.slot.to_uint()];
  alloc_context                 alloc_ctx{alloc_context::alloc_type::ded_harq_ack, crnti, pucch_slot_alloc.slot};

  ue_grants* existing_ue_grants = slot_ctx.find_ue_grants(crnti);
  if (not can_allocate_pucch(pucch_slot_alloc, existing_ue_grants, alloc_ctx)) {
    return std::nullopt;
  }

  // As per Section 9.2.1, TS 38.213:
  // - If a UE is not provided pdsch-HARQ-ACK-Codebook, the UE generates at most one HARQ-ACK information bit.
  // Multiplexing of multiple HARQ-ACK bits in a PUCCH common grant is not allowed.
  if (existing_ue_grants != nullptr and existing_ue_grants->has_common_pucch) {
    alloc_ctx.log_skipped_alloc(logger.debug, "existing common PUCCH grant for the same UE");
    return std::nullopt;
  }

  // Multiplex the HARQ-ACK bit with the existing grants, if any. When the UE has no existing grants, use a local empty
  // grant and only add it to the scheduler list if the allocation succeeds.
  ue_grants  fresh_grants{};
  ue_grants& grants = existing_ue_grants != nullptr ? *existing_ue_grants : fresh_grants;

  const pucch_uci_bits current_bits = grants.pucch_grants.get_uci_bits();
  pucch_uci_bits       new_bits     = current_bits;
  ++new_bits.harq_ack_nof_bits;

  std::optional<unsigned> pucch_res_ind =
      multiplex_and_allocate_pucch(pucch_slot_alloc, new_bits, grants, ue_cell_cfg, std::nullopt, alloc_ctx);

  if (pucch_res_ind.has_value() and existing_ue_grants == nullptr) {
    slot_ctx.ue_grants_map.emplace(crnti, fresh_grants);
  }
  return pucch_res_ind;
}

void pucch_allocator_impl::alloc_sr_opportunity(cell_slot_resource_allocator& pucch_slot_alloc,
                                                rnti_t                        crnti,
                                                const ue_cell_configuration&  ue_cell_cfg)
{
  auto&         slot_ctx = slots_ctx[pucch_slot_alloc.slot.to_uint()];
  alloc_context alloc_ctx{alloc_context::alloc_type::sr, crnti, pucch_slot_alloc.slot};

  ue_grants* existing_ue_grants = slot_ctx.find_ue_grants(crnti);
  if (not can_allocate_pucch(pucch_slot_alloc, existing_ue_grants, alloc_ctx)) {
    return;
  }

  if (existing_ue_grants != nullptr) {
    // NOTE: This check can be removed in future refactors, it's not required by the SR allocator. At the moment, we
    // schedule the SRs before anything else, therefore we don't expect to find any existing PUCCH grant.
    if (not existing_ue_grants->pucch_grants.is_empty()) {
      alloc_ctx.log_skipped_alloc(logger.warning, "existing grants for the same UE");
    } else {
      // Allocation of dedicated + common resources are handled by allocating PUCCH common on existing SR, not the other
      // way around. If the function enters the path, it means it too early to start scheduling the SR.
      alloc_ctx.log_skipped_alloc(logger.info, "existing common PUCCH grant for the same UE");
    }
    return;
  }

  // [Implementation-defined] We only allow a max number of PUCCH + PUSCH grants per slot.
  if (not is_there_space_for_new_pucch_grants(pucch_slot_alloc.result, 1U)) {
    alloc_ctx.log_skipped_alloc(logger.warning, "max number of UL/PUCCH grants reached");
    return;
  }

  // The guard will release the resources reserved through it unless commit() is called (i.e., on success).
  pucch_resource_manager::ue_reservation_guard guard(&resource_manager, pucch_slot_alloc, crnti, ue_cell_cfg);

  // Get the index of the PUCCH resource to be used for SR.
  const pucch_resource* sr_res = guard.reserve_sr_resource();
  if (sr_res == nullptr) {
    alloc_ctx.log_skipped_alloc(logger.warning, "SR resource not available");
    return;
  }
  ocudu_assert(sr_res->format() == pucch_format::FORMAT_0 or sr_res->format() == pucch_format::FORMAT_1,
               "Invalid PUCCH format for SR resource (should be 0 or 1)");

  // Allocate PUCCH SR grant only.
  fill_ded_pdu(pucch_slot_alloc.result.ul.pucchs.emplace_back(),
               *sr_res,
               pucch_uci_bits{.sr_bits = sr_nof_bits::one},
               crnti,
               false);

  // Save the info in the scheduler list of PUCCH grants.
  slot_ctx.ue_grants_map.emplace(crnti,
                                 ue_grants{.pucch_grants = {.sr = pucch_grant{.type = pucch_grant_type::sr,
                                                                              .res  = sr_res,
                                                                              .bits = {.sr_bits = sr_nof_bits::one}}}});

  // Finalize the reservation of the resources.
  guard.commit();
}

void pucch_allocator_impl::alloc_csi_opportunity(cell_slot_resource_allocator& pucch_slot_alloc,
                                                 rnti_t                        crnti,
                                                 const ue_cell_configuration&  ue_cell_cfg,
                                                 unsigned                      csi_part1_nof_bits)
{
  auto&         slot_ctx = slots_ctx[pucch_slot_alloc.slot.to_uint()];
  alloc_context alloc_ctx{alloc_context::alloc_type::csi, crnti, pucch_slot_alloc.slot};

  ue_grants* existing_ue_grants = slot_ctx.find_ue_grants(crnti);
  if (not can_allocate_pucch(pucch_slot_alloc, existing_ue_grants, alloc_ctx)) {
    return;
  }

  if (existing_ue_grants != nullptr and existing_ue_grants->has_common_pucch) {
    // Allocation of dedicated + common resources are handled by allocating PUCCH common on existing CSI, not the
    // other way around. If the function enters the path, it means it too early to start scheduling the CSI.
    alloc_ctx.log_skipped_alloc(logger.info, "existing common PUCCH grant for the same UE");
    return;
  }

  // Multiplex the CSI bits with the existing grants, if any. When the UE has no existing grants, use a local empty
  // grant and only add it to the scheduler list if the allocation succeeds.
  ue_grants  fresh_grants{};
  ue_grants& grants = existing_ue_grants != nullptr ? *existing_ue_grants : fresh_grants;

  pucch_uci_bits bits_for_uci = grants.pucch_grants.get_uci_bits();
  ocudu_assert(bits_for_uci.csi_part1_nof_bits == 0, "PUCCH grant for CSI has already been allocated");
  bits_for_uci.csi_part1_nof_bits = csi_part1_nof_bits;
  const std::optional<unsigned> alloc_result =
      multiplex_and_allocate_pucch(pucch_slot_alloc, bits_for_uci, grants, ue_cell_cfg, std::nullopt, alloc_ctx);

  if (alloc_result.has_value() and existing_ue_grants == nullptr) {
    slot_ctx.ue_grants_map.emplace(crnti, fresh_grants);
  }
}

pucch_uci_bits pucch_allocator_impl::remove_ue_uci_from_pucch(cell_slot_resource_allocator& slot_alloc,
                                                              rnti_t                        crnti,
                                                              const ue_cell_configuration&  ue_cell_cfg)
{
  pucch_uci_bits removed_uci_info;

  // Get the PUCCH grants for the slot.
  auto&      slot_ctx           = slots_ctx[slot_alloc.slot.to_uint()];
  ue_grants* existing_ue_grants = slot_ctx.find_ue_grants(crnti);

  auto& pucch_pdus = slot_alloc.result.ul.pucchs;
  auto* pdu        = std::find_if(
      pucch_pdus.begin(), pucch_pdus.end(), [crnti](const pucch_info& info) { return info.crnti == crnti; });

  if ((existing_ue_grants != nullptr) != (pdu != pucch_pdus.end())) {
    logger.error("rnti={}: Inconsistent PUCCH state: PUCCH grant list and PUCCH PDUs list are not in sync", crnti);
  }

  // Get the bits from the PUCCH grants and remove the item from the list.
  if (existing_ue_grants != nullptr) {
    // This function can be called for UE with dedicated PUCCH grants only.
    if (existing_ue_grants->has_common_pucch) {
      logger.error("rnti={}: unexpected call of this function for UE with PUCCH common grant allocated", crnti);
    }

    // Get the UCI bits.
    removed_uci_info = existing_ue_grants->pucch_grants.get_uci_bits();

    // Release the resources used in the PUCCH resource manager first.
    pucch_resource_manager::ue_reservation_guard guard(&resource_manager, slot_alloc, crnti, ue_cell_cfg);
    if (existing_ue_grants->pucch_grants.harq_ack.has_value()) {
      if (existing_ue_grants->pucch_grants.harq_ack.value().harq_id.pucch_set_idx == pucch_res_set_id::set_0) {
        guard.release_harq_set_0_resource();
      } else {
        guard.release_harq_set_1_resource();
      }
    }
    if (existing_ue_grants->pucch_grants.sr.has_value()) {
      guard.release_sr_resource();
    }
    if (existing_ue_grants->pucch_grants.csi.has_value()) {
      guard.release_csi_resource();
    }
    // Finalize the release of the resources.
    guard.commit();

    slot_ctx.ue_grants_map.erase(crnti);
  }

  // Start from the (first) PUCCH PDU that was found above.
  for (auto* pdu_it = pdu; pdu_it != pucch_pdus.end();) {
    if (pdu_it->crnti == crnti) {
      pdu_it = pucch_pdus.erase(pdu_it);
    } else {
      ++pdu_it;
    }
  }

  return removed_uci_info;
}

bool pucch_allocator_impl::has_common_pucch_grant(rnti_t rnti, slot_point sl_tx) const
{
  const auto& slot_ctx = slots_ctx[sl_tx.to_uint()];
  auto        it       = slot_ctx.ue_grants_map.find(rnti);
  return it != slot_ctx.ue_grants_map.end() and it->second.has_common_pucch;
}

//////////////     Sub-class definitions       //////////////

pucch_uci_bits pucch_allocator_impl::pucch_grant_list::get_uci_bits() const
{
  pucch_uci_bits bits;

  // If there is an SR grant, then the SR bits are retrieved from it. Otherwise, the SR bits are retrieved from the
  // HARQ or CSI grants; as HARQ and CSI grants can separate, we need to take the SR value that is non-zero, if any.
  if (sr.has_value()) {
    bits.sr_bits = sr->bits.sr_bits;
  } else {
    if (harq_ack.has_value()) {
      bits.sr_bits = harq_ack->bits.sr_bits;
    }
    if (csi.has_value() and bits.sr_bits == sr_nof_bits::no_sr) {
      bits.sr_bits = csi->bits.sr_bits;
    }
  }

  // If there is a CSI grant, then the CSI part 1 bits are retrieved from it. Otherwise, the HARQ bits are retrieved
  // from the HARQ grant (it can be zero).
  if (csi.has_value()) {
    bits.csi_part1_nof_bits = csi.value().bits.csi_part1_nof_bits;
  } else if (harq_ack.has_value()) {
    bits.csi_part1_nof_bits = harq_ack.value().bits.csi_part1_nof_bits;
  }

  // If there is an HARQ grant, then the HARQ bits are retrieved from it. Otherwise, they are retrieved from the
  // SR or CSI grants; as SR and CSI grants can separate, we need to take the HARQ bits value that is non-zero, if any.
  if (harq_ack.has_value()) {
    bits.harq_ack_nof_bits = harq_ack.value().bits.harq_ack_nof_bits;
  } else {
    if (sr.has_value()) {
      bits.harq_ack_nof_bits = sr.value().bits.harq_ack_nof_bits;
    }
    if (csi.has_value() and bits.harq_ack_nof_bits == 0U) {
      bits.harq_ack_nof_bits = csi.value().bits.harq_ack_nof_bits;
    }
  }

  return bits;
}

bool pucch_allocator_impl::pucch_grant_list::is_empty() const
{
  return not sr.has_value() and not harq_ack.has_value() and not csi.has_value();
}

unsigned pucch_allocator_impl::pucch_grant_list::get_nof_grants() const
{
  unsigned nof_grants = 0;
  if (harq_ack.has_value()) {
    ++nof_grants;
  }
  if (sr.has_value()) {
    ++nof_grants;
  }
  if (csi.has_value()) {
    ++nof_grants;
  }
  return nof_grants;
}

///////////////  Main private functions   //////////////

// The function returns an available common PUCCH resource (i.e., not used by other UEs); it returns a null optional
// if no resource is available.
std::optional<pucch_allocator_impl::pucch_common_params>
pucch_allocator_impl::alloc_pucch_common_res_harq(cell_slot_resource_allocator&  pucch_slot_alloc,
                                                  const dci_context_information& dci_info,
                                                  rnti_t                         rnti)
{
  // Get N_CCE (nof_coreset_cces) and n_{CCE,0} (start_cce_idx), as per TS 38.213, Section 9.2.1.
  const unsigned nof_coreset_cces = dci_info.coreset_cfg->get_nof_cces();
  const unsigned start_cce_idx    = dci_info.cces.ncce;

  // As per Section 9.2.1, TS 38.213, this is the max value of \f$\Delta_{PRI}\f$, which is a 3-bit unsigned.
  static constexpr unsigned max_d_pri = 7;
  // Loop over the values of \Delta_PRI to find an available common resource that doesn't collide with existing grants.
  for (unsigned d_pri = 0; d_pri != max_d_pri + 1; ++d_pri) {
    // r_PUCCH, as per Section 9.2.1, TS 38.213.
    const unsigned r_pucch = get_pucch_default_resource_index(start_cce_idx, nof_coreset_cces, d_pri);
    ocudu_assert(r_pucch < 16, "r_PUCCH must be less than 16");

    // Look for an available PUCCH common resource.
    if (not resource_manager.reserve_harq_common_resource(pucch_slot_alloc, r_pucch, rnti)) {
      continue;
    }

    return pucch_common_params{.pucch_res_indicator = d_pri, .r_pucch = r_pucch};
  }

  // This is the case in which there exists no available resource.
  return std::nullopt;
}

void pucch_allocator_impl::compute_pucch_common_params_and_alloc(cell_slot_resource_allocator& pucch_alloc,
                                                                 rnti_t                        rnti,
                                                                 pucch_common_params           pucch_params)
{
  // Fill scheduler output.
  pucch_info& common_pdu = pucch_alloc.result.ul.pucchs.emplace_back();
  fill_common_pdu(common_pdu, cell_cfg.bwp_res[to_bwp_id(0)].ul().pucch.common[pucch_params.r_pucch], rnti);

  // Update the PUCCH grants with the common resource.
  auto&      slot_ctx           = slots_ctx[pucch_alloc.slot.to_uint()];
  ue_grants* existing_ue_grants = slot_ctx.find_ue_grants(rnti);
  ocudu_assert(existing_ue_grants != nullptr, "UE grants allocation failed at some point");
  existing_ue_grants->has_common_pucch = true;

  // TODO: once the code has been properly tested, remove the debug message.
  logger.debug("rnti={}: PUCCH on common and ded. resource with res_ind={} allocated for slot={}",
               rnti,
               pucch_params.pucch_res_indicator,
               pucch_alloc.slot);
}

// The function returns an available common PUCCH resource (i.e., not used by other UEs); it returns a null optional
// if no resource is available.
std::optional<pucch_allocator_impl::pucch_common_params>
pucch_allocator_impl::find_common_and_ded_harq_res_available(cell_slot_resource_allocator&  pucch_slot_alloc,
                                                             ue_grants&                     current_grants,
                                                             const ue_cell_configuration&   ue_cell_cfg,
                                                             const dci_context_information& dci_info,
                                                             const alloc_context&           alloc_ctx)
{
  // Get N_CCE (nof_coreset_cces) and n_{CCE,0} (start_cce_idx), as per TS 38.213, Section 9.2.1.
  const unsigned nof_coreset_cces = dci_info.coreset_cfg->get_nof_cces();
  const unsigned start_cce_idx    = dci_info.cces.ncce;

  // As per Section 9.2.1, TS 38.213, this is the max value of \f$\Delta_{PRI}\f$, which is a 3-bit unsigned.
  static constexpr unsigned max_d_pri = 7;
  for (unsigned d_pri = 0; d_pri != max_d_pri + 1; ++d_pri) {
    // The guard will release the resources reserved through it unless commit() is called (i.e., on success).
    pucch_resource_manager::ue_reservation_guard guard(
        &resource_manager, pucch_slot_alloc, alloc_ctx.rnti, ue_cell_cfg);

    // r_PUCCH, as per Section 9.2.1, TS 38.213.
    const unsigned r_pucch = get_pucch_default_resource_index(start_cce_idx, nof_coreset_cces, d_pri);
    ocudu_assert(r_pucch < 16, "r_PUCCH must be less than 16");

    // Look for an available PUCCH common resource.
    // TODO: allow collisions between common and dedicated resources for the same UE, since it will never send both.
    if (not resource_manager.reserve_harq_common_resource(pucch_slot_alloc, r_pucch, alloc_ctx.rnti)) {
      continue;
    }

    // Look for an available PUCCH dedicated resource with the same PUCCH resource indicator as the common's.
    const pucch_resource* ded_res = guard.reserve_harq_set_0_resource_by_res_indicator(d_pri);
    if (ded_res == nullptr) {
      resource_manager.release_harq_common_resource(pucch_slot_alloc, r_pucch, alloc_ctx.rnti);
      continue;
    }

    // In the bit to transmit, we have 1 HARQ-ACK bit, plus the SR and CSI bits if any existing grants.
    pucch_uci_bits bits_for_uci{.harq_ack_nof_bits = 1U};
    bits_for_uci.sr_bits =
        current_grants.pucch_grants.sr.has_value() ? current_grants.pucch_grants.sr->bits.sr_bits : sr_nof_bits::no_sr;
    bits_for_uci.csi_part1_nof_bits =
        current_grants.pucch_grants.csi.has_value() ? current_grants.pucch_grants.csi->bits.csi_part1_nof_bits : 0U;

    // The PUCCH resource indicator must match the one used for the common resource.
    std::optional<unsigned> d_pri_ded =
        multiplex_and_allocate_pucch(pucch_slot_alloc, bits_for_uci, current_grants, ue_cell_cfg, d_pri, alloc_ctx);

    // In the case of Formats 0 and 2, the PUCCH (dedicated) resources that can be used when there are SR/CSI
    // pre-allocated are constrained in the PUCCH resource indicator. Therefore, the \ref multiplex_and_allocate_pucch
    // function might not preserve the requested PUCCH resource indicator \ref d_pri.
    if (not d_pri_ded.has_value() or (cell_cfg.is_pucch_f0_and_f2() and *d_pri_ded != d_pri)) {
      resource_manager.release_harq_common_resource(pucch_slot_alloc, r_pucch, alloc_ctx.rnti);
      continue;
    }
    ocudu_assert(*d_pri_ded == d_pri, "The PUCCH resource indicator must match for common and ded. resources");

    // Finalize the reservation of the resources.
    guard.commit();
    return pucch_common_params{.pucch_res_indicator = d_pri, .r_pucch = r_pucch};
  };
  return std::nullopt;
}

std::optional<unsigned>
pucch_allocator_impl::multiplex_and_allocate_pucch(cell_slot_resource_allocator& pucch_slot_alloc,
                                                   const pucch_uci_bits&         new_bits,
                                                   ue_grants&                    current_grants,
                                                   const ue_cell_configuration&  ue_cell_cfg,
                                                   std::optional<uint8_t>        preserve_res_indicator,
                                                   const alloc_context&          alloc_ctx)
{
  // NOTE: In this function, the \c candidate_grants report the data about the grants BEFORE the multiplexing is
  // applied. Each grant contains only one UCI type (HARQ grant contains HARQ bits, SR grant contains SR bits and so
  // on); on the contrary, \c grants_to_tx contains the grants AFTER the multiplexing; this means that 1 grant can
  // contain more than 1 UCI bit type.

  // The guard will release the resources reserved through it unless commit() is called (i.e., on success).
  pucch_resource_manager::ue_reservation_guard guard(&resource_manager, pucch_slot_alloc, alloc_ctx.rnti, ue_cell_cfg);

  // Find the grants/resources needed for the UCI bits to be reported, assuming the resources are not multiplexed.
  std::optional<pucch_grant_list> candidate_grants = get_pucch_res_pre_multiplexing(guard, current_grants, new_bits);
  if (not candidate_grants.has_value()) {
    return std::nullopt;
  }

  pucch_grant_list grants_to_tx = multiplex_resources(guard, candidate_grants.value(), preserve_res_indicator);

  if (grants_to_tx.is_empty()) {
    return std::nullopt;
  }

  // Reserve the resources needed for the multiplexed grants.
  if (grants_to_tx.sr.has_value()) {
    const auto* sr_res = guard.reserve_sr_resource();
    if (sr_res == nullptr) {
      alloc_ctx.log_skipped_alloc(logger.error, "SR resource not available after multiplexing");
      return std::nullopt;
    }
  }
  if (grants_to_tx.csi.has_value()) {
    const auto* csi_res = guard.reserve_csi_resource();
    if (csi_res == nullptr) {
      alloc_ctx.log_skipped_alloc(logger.error, "CSI resource not available after multiplexing");
      return std::nullopt;
    }
  }

  // Allocate the grants.
  return allocate_grants(guard, pucch_slot_alloc, current_grants, grants_to_tx, alloc_ctx);
}

// TODO: check how to handle F0+F3/F4.
static unsigned get_pucch_resource_ind_f0_sr_csi(pucch_uci_bits bits, const pucch_resource_builder_params& params)
{
  // With Format 0, with HARQ-ACK bits <= 2, pick a resource from PUCCH resource set 0.

  if (bits.harq_ack_nof_bits <= 2U) {
    // At position res_set_size the resource coincides with the SR resource.
    if (bits.sr_bits != sr_nof_bits::no_sr) {
      return params.res_set_size.value();
    }
    // NOTE: Either CSI or SR bits are non-zero, but not both.
    // At position res_set_size + 1 the resource is of Format 0, but set on the same PRBs/symbols as the CSI resource.
    return params.res_set_size.value() + 1;
  }

  // This if for bits.harq_ack_nof_bits > 2U.

  // At position res_set_size the resource is of Format 2, but set on the same PRBs/symbols as the SR resource.
  if (bits.sr_bits != sr_nof_bits::no_sr) {
    return params.res_set_size.value();
  }
  // NOTE: Either CSI or SR bits are non-zero, but not both.
  // At position res_set_size + 1 the resource coincides with the CSI resource.
  return params.res_set_size.value() + 1U;
}

std::optional<pucch_allocator_impl::pucch_grant_list>
pucch_allocator_impl::get_pucch_res_pre_multiplexing(pucch_resource_manager::ue_reservation_guard& guard,
                                                     const ue_grants&                              ue_current_grants,
                                                     const pucch_uci_bits&                         new_bits)
{
  pucch_grant_list candidate_resources;

  if (new_bits.sr_bits != sr_nof_bits::no_sr) {
    candidate_resources.sr.emplace(pucch_grant{.type = pucch_grant_type::sr});
    pucch_grant& sr_candidate_grant = candidate_resources.sr.value();

    // Get the resource from the resource manager; the UCI bits will be added later.
    // NOTE: if the SR resource was already assigned to this UE, the resource manager will only return the PUCCH
    // config that was reserved previously.
    const pucch_resource* sr_res = guard.peek_sr_resource();
    ocudu_assert(sr_res != nullptr, "rnti={}: Peeking a PUCCH SR resource returned nullptr", guard.get_rnti());
    sr_candidate_grant.res = sr_res;
    // Only copy the SR bits, as at this stage we only need to consider the UCI bits assuming the resources still
    // need to be multiplexed.
    sr_candidate_grant.bits.harq_ack_nof_bits  = 0U;
    sr_candidate_grant.bits.sr_bits            = new_bits.sr_bits;
    sr_candidate_grant.bits.csi_part1_nof_bits = 0U;
  }

  if (new_bits.csi_part1_nof_bits != 0U) {
    candidate_resources.csi.emplace(pucch_grant{.type = pucch_grant_type::csi});
    pucch_grant& csi_candidate_grant = candidate_resources.csi.value();

    // Get the resource from the resource manager; the UCI bits will be added later.
    // NOTE: if the CSI resource was already assigned to this UE, the resource manager will only return the PUCCH
    // config that was reserved previously.
    const pucch_resource* csi_res = guard.peek_csi_resource();
    ocudu_assert(csi_res != nullptr, "rnti={}: Peeking a PUCCH CSI resource returned nullptr", guard.get_rnti());
    csi_candidate_grant.res = csi_res;
    // Only copy the HARQ-ACK bits, as at this stage we only need to consider the UCI bits assuming the resources
    // still need to be multiplexed.
    csi_candidate_grant.bits.harq_ack_nof_bits  = 0U;
    csi_candidate_grant.bits.sr_bits            = sr_nof_bits::no_sr;
    csi_candidate_grant.bits.csi_part1_nof_bits = new_bits.csi_part1_nof_bits;
  }

  if (new_bits.harq_ack_nof_bits > 0) {
    // Case HARQ ACK bits 1 or 2, resource to be chosen from PUCCH resource set 0; else, pick from PUCCH resource
    // set 1.
    const pucch_res_set_id pucch_set_idx =
        new_bits.harq_ack_nof_bits <= 2U ? pucch_res_set_id::set_0 : pucch_res_set_id::set_1;

    // Not all UEs support transmission of more than 1 PUCCHs per slot. This would be the case if:
    //  - UE configured with F0 and F2 resources
    //  - UE needs to transmit HARQ-ACK + SR or CSI in the same slot
    //  - The resource for HARQ-ACK and the resource for SR/CSI do not overlap in symbols
    //  [Implementation-defined] In this case, we force the UE to use a PUCCH resource for HARQ-ACK that is guaranteed
    //  to overlap (in symbols) with the SR or CSI resource.
    const bool f0_and_f2_and_ue_with_sr_or_csi =
        cell_cfg.is_pucch_f0_and_f2() and (new_bits.sr_bits != sr_nof_bits::no_sr or new_bits.csi_part1_nof_bits != 0U);

    candidate_resources.harq_ack.emplace(pucch_grant{.type = pucch_grant_type::harq_ack});
    pucch_grant& harq_candidate_grant = candidate_resources.harq_ack.value();

    // Handle these 2 cases: (i) There is already a PUCCH resource for HARQ-ACK; if so, we use the info and
    // configuration from this resource; (ii) the UE has Format 0 resources, and it needs to transmit HARQ-ACK bits +
    // SR or CSI in the same slot.
    if ((ue_current_grants.pucch_grants.harq_ack.has_value() and
         ue_current_grants.pucch_grants.harq_ack->harq_id.pucch_set_idx == pucch_set_idx) or
        f0_and_f2_and_ue_with_sr_or_csi) {
      // NOTE: If the UE has Format 0 resources, and it needs to transmit HARQ-ACK bits + SR or CSI in the same slot,
      // use the HARQ-ACK resource that has highest PUCCH resource indicator; the UE's dedicated PUCCH config has been
      // constructed in such a way that this resource overlaps with the SR or CSI resource.
      const unsigned pucch_res_ind =
          f0_and_f2_and_ue_with_sr_or_csi
              ? get_pucch_resource_ind_f0_sr_csi(new_bits, cell_cfg.params.init_bwp.pucch.resources)
              : ue_current_grants.pucch_grants.harq_ack.value().harq_id.pucch_res_ind;

      const pucch_resource* harq_res = pucch_set_idx == pucch_res_set_id::set_0
                                           ? guard.reserve_harq_set_0_resource_by_res_indicator(pucch_res_ind)
                                           : guard.reserve_harq_set_1_resource_by_res_indicator(pucch_res_ind);
      if (harq_res == nullptr) {
        if (f0_and_f2_and_ue_with_sr_or_csi) {
          ocudu_assertion_failure("rnti={}: PUCCH HARQ-ACK that should be reserved for this UE is not available",
                                  guard.get_rnti());
        } else {
          ocudu_assertion_failure("rnti={}: PUCCH HARQ-ACK resource previously reserved not available anymore",
                                  guard.get_rnti());
        }

        return std::nullopt;
      }
      harq_candidate_grant.harq_id.pucch_set_idx = pucch_set_idx;
      harq_candidate_grant.harq_id.pucch_res_ind = static_cast<uint8_t>(pucch_res_ind);
      harq_candidate_grant.res                   = harq_res;
    }
    // Get a new PUCCH resource for HARQ-ACK from the correct PUCCH resource set.
    else {
      // Only copy the HARQ-ACK bits, as at this stage we only need to consider the UCI bits before multiplexing.
      pucch_harq_resource_alloc_record harq_res = pucch_set_idx == pucch_res_set_id::set_0
                                                      ? guard.reserve_harq_set_0_resource_next_available()
                                                      : guard.reserve_harq_set_1_resource_next_available();
      if (harq_res.resource == nullptr) {
        return std::nullopt;
      }
      harq_candidate_grant.harq_id.pucch_set_idx = pucch_set_idx;
      harq_candidate_grant.harq_id.pucch_res_ind = harq_res.pucch_res_indicator;
      harq_candidate_grant.res                   = harq_res.resource;
    }
    // Only copy the HARQ-ACK bits, as at this stage we only need to consider the UCI bits assuming the resources
    // still need to be multiplexed.
    harq_candidate_grant.bits.harq_ack_nof_bits  = new_bits.harq_ack_nof_bits;
    harq_candidate_grant.bits.sr_bits            = sr_nof_bits::no_sr;
    harq_candidate_grant.bits.csi_part1_nof_bits = 0U;
  }

  return candidate_resources;
}

pucch_allocator_impl::pucch_grant_list
pucch_allocator_impl::multiplex_resources(pucch_resource_manager::ue_reservation_guard& guard,
                                          const pucch_grant_list&                       candidate_grants,
                                          std::optional<uint8_t>                        preserve_res_indicator)
{
  // This function implements the multiplexing pseudo-code for PUCCH resources defined in Section 9.2.5, TS 38.213.
  // Refer to paragraph starting as "Set Q to the set of resources for transmission of corresponding PUCCHs in a
  // single slot without repetitions where".
  pucch_grant_list mplexed_grants;

  // The vector should contain at most 1 HARQ-ACK grant, 1 SR grant, 1 CSI grant.
  static constexpr size_t                   max_nof_grant = 3;
  static_vector<pucch_grant, max_nof_grant> resource_set_q;

  // Build the resource set Q. Refer to Section 9.2.5, TS 38.213.
  if (candidate_grants.harq_ack.has_value()) {
    resource_set_q.emplace_back(candidate_grants.harq_ack.value());
  }
  if (candidate_grants.sr.has_value()) {
    resource_set_q.emplace_back(candidate_grants.sr.value());
  }
  if (candidate_grants.csi.has_value()) {
    resource_set_q.emplace_back(candidate_grants.csi.value());
  }

  // Sort the resources in the set based on the number of symbols.
  auto sort_res_set_q = [&resource_set_q]() {
    std::sort(resource_set_q.begin(), resource_set_q.end(), [](const pucch_grant& lhs_res, const pucch_grant& rhs_res) {
      return lhs_res.res->syms.start() < rhs_res.res->syms.start() or
             (lhs_res.res->syms.start() == rhs_res.res->syms.start() and
              lhs_res.res->syms.length() > rhs_res.res->syms.length());
    });
  };

  sort_res_set_q();

  // This is the implementation of the pseudo-code for multiplexing the resources provided in Section 9.2.5,
  // TS 38.213.
  unsigned o_cnt = 0;
  unsigned j_cnt = 0;
  while (j_cnt < resource_set_q.size()) {
    if (j_cnt < resource_set_q.size() - 1 and
        resource_set_q[j_cnt - o_cnt].res->syms.overlaps(resource_set_q[j_cnt + 1].res->syms)) {
      ++j_cnt;
      ++o_cnt;
    } else {
      if (o_cnt > 0U) {
        // Merge the overlapping resources.
        std::optional<pucch_grant> new_res = merge_pucch_resources(
            guard, span<const pucch_grant>(&resource_set_q[j_cnt - o_cnt], o_cnt + 1), preserve_res_indicator);
        if (not new_res.has_value()) {
          return {};
        }
        // Remove the old resources that got merged from the set.
        // TODO: check if, by using a different data structure, we can achieve the deletion more efficiently.
        resource_set_q.erase(resource_set_q.begin() + j_cnt - o_cnt, resource_set_q.begin() + j_cnt + 1);

        // Add the new resource (resulting from the previous merge) to the set.
        resource_set_q.push_back(new_res.value());

        // Sort the resources in the set based on the first symbol position and number of symbols.
        sort_res_set_q();

        // Reset the counter and start from the beginning.
        j_cnt = 0;
        o_cnt = 0;
      } else {
        ++j_cnt;
      }
    }
  }

  // The PUCCH resource multiplexing algorithm above is specified from the UE's perspective. In the GNB, we need to
  // add an extra resource Format 1 if slot there is a SR opportunity and HARQ bits to be reported with PUCCH
  // Format 1.
  if (resource_set_q.size() == 1 and resource_set_q.front().res->format() == pucch_format::FORMAT_1 and
      resource_set_q.front().bits.harq_ack_nof_bits != 0 and
      resource_set_q.front().bits.sr_bits != sr_nof_bits::no_sr) {
    const pucch_resource* sr_res = guard.reserve_sr_resource();
    ocudu_assert(sr_res != nullptr, "rnti={}: PUCCH SR resource should be available", guard.get_rnti());

    // For Format 1, the grant with SR bit is the one with the SR-dedicated resource; in the HARQ-ACK grant we only
    // report the HARQ bits.
    pucch_uci_bits bits        = {.harq_ack_nof_bits  = resource_set_q.front().bits.harq_ack_nof_bits,
                                  .sr_bits            = resource_set_q.front().bits.sr_bits,
                                  .csi_part1_nof_bits = 0};
    auto&          sr_grant_f1 = resource_set_q.emplace_back(pucch_grant{.type = pucch_grant_type::sr, .bits = bits});
    sr_grant_f1.res            = sr_res;
    resource_set_q.front().bits.sr_bits = sr_nof_bits::no_sr;
  }

  // Build the final list of PUCCH resources that need to be transmitted.
  for (const auto& mplex_res : resource_set_q) {
    // Check if the UCI payload fits in the PUCCH resource. For Format 0/1 the payload is structurally bounded, so the
    // check only applies to Format 2/3/4 resources.
    if (mplex_res.res->format() >= pucch_format::FORMAT_2 and
        mplex_res.bits.get_total_bits() > get_max_payload(mplex_res.res->format())) {
      return {};
    }
    if (mplex_res.type == pucch_grant_type::harq_ack) {
      mplexed_grants.harq_ack.emplace(mplex_res);
    } else if (mplex_res.type == pucch_grant_type::sr) {
      mplexed_grants.sr.emplace(mplex_res);
    } else if (mplex_res.type == pucch_grant_type::csi) {
      mplexed_grants.csi.emplace(mplex_res);
    }
  }
  return mplexed_grants;
}

std::optional<pucch_allocator_impl::pucch_grant>
pucch_allocator_impl::merge_pucch_resources(pucch_resource_manager::ue_reservation_guard& guard,
                                            span<const pucch_grant>                       resources_to_merge,
                                            std::optional<uint8_t>                        preserve_res_indicator)
{
  // This function implements the merging rules for HARQ-ACK, SR and CSI defined in Section 9.2.5.1 and 9.2.5.2,
  // TS 38.213.

  // This function should only be called if there are 2 or 3 resources.
  if (resources_to_merge.size() == 1U or resources_to_merge.size() > 3U) {
    return std::nullopt;
  }

  if (resources_to_merge.size() == 2) {
    const pucch_grant& r_0 = resources_to_merge[0];
    const pucch_grant& r_1 = resources_to_merge[1];

    // SR and HARQ only.
    if ((r_0.type == pucch_grant_type::sr and r_1.type == pucch_grant_type::harq_ack) or
        (r_0.type == pucch_grant_type::harq_ack and r_1.type == pucch_grant_type::sr)) {
      const pucch_grant& r_harq = r_0.type == pucch_grant_type::harq_ack ? r_0 : r_1;
      const pucch_grant& r_sr   = r_0.type == pucch_grant_type::sr ? r_0 : r_1;

      // When merging SR and HARQ, no matter the format, the resource to be used is the one from HARQ-ACK.
      pucch_grant new_resource{.type = pucch_grant_type::harq_ack};
      // For the case of HARQ-ACK with Format 0 or Format 1, the pre-multiplexing HARQ-ACK grant only has HARQ-ACK
      // bits. NOTE: This rule is defined in Section 9.2.5.1, TS 38.213. For Format 1, in the GNB we need to consider
      // there can be either a resource with positive SR or a resource with negative SR; in this algorithm we assume
      // the positive SR; we'll add the resource for negative SR later on (the algorithm specified in Section 9.2.5
      // would not work with both resources).
      if (r_harq.res->format() == pucch_format::FORMAT_0 or r_harq.res->format() == pucch_format::FORMAT_1) {
        ocudu_assert(r_sr.res->format() == r_harq.res->format(), "The two resources must have the same format");
        // TODO: Multiplexing of SR + HARQ on F0 needs to be checked, as the TS is not clear about it.
        new_resource              = r_harq;
        new_resource.bits.sr_bits = r_sr.bits.sr_bits;
        return new_resource;
      }
      // Apply HARQ merging rule for Format >= 2: all PUCCH resources will be multiplexed in a PUCCH resource from
      // PUCCH res set 1. NOTE: This rule is defined in Section 9.2.5.1 or 9.2.5.2 (if any CSI bits), TS 38.213.
      // NOTE: For the case of HARQ-ACK with Format >= 2, the HARQ-ACK resource has HARQ-ACK bits and optionally CSI
      // bits.
      ocudu_assert(r_sr.res->format() == pucch_format::FORMAT_0 or r_sr.res->format() == pucch_format::FORMAT_1,
                   "The two resources must have the same format");
      // Apply F2 CSI merging rule: SR and CSI PUCCH resources will be multiplexed in the CSI PUCCH resource.
      // A HARQ resource from PUCCH resource set idx 1 already exits. Use that one.
      if (r_harq.harq_id.pucch_set_idx == pucch_res_set_id::set_1) {
        new_resource = r_harq;
      }
      // Get a resource from PUCCH resource set idx 1, if available, with the same PUCCH resource indicator as for
      // the PUCCH resource from set idx 0. NOTE: This sub-case is used by the PUCCH common and dedicated
      // allocator.
      else if (preserve_res_indicator.has_value()) {
        const pucch_resource* harq_res =
            guard.reserve_harq_set_1_resource_by_res_indicator(preserve_res_indicator.value());
        if (harq_res == nullptr) {
          return std::nullopt;
        }
        new_resource.harq_id.pucch_set_idx = pucch_res_set_id::set_1;
        new_resource.harq_id.pucch_res_ind = r_harq.harq_id.pucch_res_ind;
        new_resource.res                   = harq_res;
      }
      // Get a new HARQ resource (from PUCCH resource set idx 1) from the resource manager.
      else {
        pucch_harq_resource_alloc_record harq_res = guard.reserve_harq_set_1_resource_next_available();
        if (harq_res.resource == nullptr) {
          return std::nullopt;
        }
        new_resource.harq_id.pucch_set_idx = pucch_res_set_id::set_1;
        new_resource.harq_id.pucch_res_ind = harq_res.pucch_res_indicator;
        new_resource.res                   = harq_res.resource;
      }
      // If any CSI bits, these are contained in the HARQ-ACK resource.
      new_resource.bits.harq_ack_nof_bits  = r_harq.bits.harq_ack_nof_bits;
      new_resource.bits.csi_part1_nof_bits = r_harq.bits.csi_part1_nof_bits;
      new_resource.bits.sr_bits            = r_sr.bits.sr_bits;
      // Check if the UCI payload fits in the PUCCH resource.
      if (new_resource.bits.get_total_bits() > get_max_payload(new_resource.res->format())) {
        return std::nullopt;
      }
      return new_resource;
    }

    // SR and CSI only. In the case, the SR resource has only SR bits, and the CSI resource has only CSI bits.
    // Apply CSI merging rule for SR + CSI; this is defined in Section 9.2.5.1, TS 38.213.
    if ((r_0.type == pucch_grant_type::sr and r_1.type == pucch_grant_type::csi) or
        (r_0.type == pucch_grant_type::csi and r_1.type == pucch_grant_type::sr)) {
      // Apply F2 CSI merging rule: SR and CSI PUCCH resources will be multiplexed in the CSI PUCCH resource.
      pucch_grant        new_resource{.type = pucch_grant_type::csi};
      const pucch_grant& r_csi = r_0.type == pucch_grant_type::csi ? r_0 : r_1;
      const pucch_grant& r_sr  = r_0.type == pucch_grant_type::sr ? r_0 : r_1;
      //  We don't support SR with Format 0 on the same slot as CSI.
      ocudu_assert(r_sr.res->format() != pucch_format::FORMAT_0,
                   "SR with Format 0 is not supported on the same slot as CSI");
      // Copy the SR bits in the CSI resource.
      new_resource              = r_csi;
      new_resource.bits.sr_bits = r_sr.bits.sr_bits;

      // Check if the UCI payload fits in the PUCCH resource.
      if (new_resource.bits.get_total_bits() > get_max_payload(new_resource.res->format())) {
        return std::nullopt;
      }
      return new_resource;
    }

    // HARQ and CSI only.  Apply HARQ merging rule for Format >= 2: all PUCCH resources will be multiplexed in a
    // PUCCH resource from PUCCH resource set 1. NOTE: This rule is defined in Section 9.2.5.2, TS 38.213. NOTE: SR
    // bits, if present, can be in either HARQ or CSI, but not in both.
    if ((r_0.type == pucch_grant_type::harq_ack and r_1.type == pucch_grant_type::csi) or
        (r_0.type == pucch_grant_type::csi and r_1.type == pucch_grant_type::harq_ack)) {
      // Apply F2 HARQ merging rule: all PUCCH resources will be multiplexed in a PUCCH resource from PUCCH res
      // set 1.
      pucch_grant        new_resource{.type = pucch_grant_type::harq_ack};
      const pucch_grant& r_harq = r_0.type == pucch_grant_type::harq_ack ? r_0 : r_1;
      const pucch_grant& r_csi  = r_0.type == pucch_grant_type::csi ? r_0 : r_1;

      // A HARQ resource from PUCCH resource set idx 1 already exits. Use that one.
      if (r_harq.harq_id.pucch_set_idx == pucch_res_set_id::set_1) {
        new_resource = r_harq;
      }
      // NOTE: In this case, the PUCCH resource before merging is from PUCCH resource set 0.
      else {
        // Get a resource from PUCCH resource set idx 1, if available, with the same PUCCH resource indicator as for
        // the PUCCH resource from set idx 0.
        // NOTE: If preserve_res_indicator has a value, then the function was called by the PUCCH common and dedicated
        // allocator.
        const unsigned pucch_res_indicator =
            preserve_res_indicator.has_value() ? preserve_res_indicator.value() : r_harq.harq_id.pucch_res_ind;
        const pucch_resource* harq_res = guard.reserve_harq_set_1_resource_by_res_indicator(pucch_res_indicator);
        if (harq_res == nullptr) {
          return std::nullopt;
        }
        new_resource.harq_id.pucch_set_idx = pucch_res_set_id::set_1;
        new_resource.harq_id.pucch_res_ind = r_harq.harq_id.pucch_res_ind;
        new_resource.res                   = harq_res;
      }

      new_resource.bits.harq_ack_nof_bits  = r_harq.bits.harq_ack_nof_bits;
      new_resource.bits.csi_part1_nof_bits = r_csi.bits.csi_part1_nof_bits;
      // SR bits, if present, can be in either HARQ or CSI, but not in both.
      if (r_harq.bits.sr_bits != sr_nof_bits::no_sr) {
        new_resource.bits.sr_bits = r_harq.bits.sr_bits;
      } else if (r_csi.bits.sr_bits != sr_nof_bits::no_sr) {
        new_resource.bits.sr_bits = r_csi.bits.sr_bits;
      }

      // Check if the UCI payload fits in the PUCCH resource.
      if (new_resource.bits.get_total_bits() > get_max_payload(new_resource.res->format())) {
        return std::nullopt;
      }
      return new_resource;
    }
  }

  // Apply HARQ merging rule for Format >= 2: all PUCCH resources will be multiplexed in a PUCCH resource from PUCCH
  // resource set 1.
  // NOTE: This rule is defined in Section 9.2.5.2, TS 38.213.
  if (resources_to_merge.size() == 3) {
    // Apply F2 HARQ merging rule: all PUCCH resources will be multiplexed in a PUCCH resource from PUCCH res set 1.
    pucch_grant        new_resource{.type = pucch_grant_type::harq_ack};
    const pucch_grant* r_harq = nullptr;
    const pucch_grant* r_sr   = nullptr;
    const pucch_grant* r_csi  = nullptr;
    for (const auto& grant : resources_to_merge) {
      if (grant.type == pucch_grant_type::harq_ack) {
        r_harq = &grant;
      } else if (grant.type == pucch_grant_type::sr) {
        r_sr = &grant;
      } else if (grant.type == pucch_grant_type::csi) {
        r_csi = &grant;
      }
    }

    ocudu_assert(r_harq != nullptr and r_sr != nullptr and r_csi != nullptr, "The three resources must be present");
    if (r_sr->res->format() == pucch_format::FORMAT_0) {
      ocudu_assertion_failure("This case is not yet supported");
      // SR and CSI are not supported on the same slot if SR uses Format 0.
      return std::nullopt;
    }

    if (r_harq->res->format() != pucch_format::FORMAT_0 and r_harq->res->format() != pucch_format::FORMAT_1) {
      new_resource = *r_harq;
    }
    // NOTE: In this case, the PUCCH resource before merging is from PUCCH resource set 0.
    else {
      // Get a resource from PUCCH resource set idx 1, if available, with the same PUCCH resource indicator as for
      // the PUCCH resource from set idx 0.

      const pucch_resource* harq_res =
          guard.reserve_harq_set_1_resource_by_res_indicator(r_harq->harq_id.pucch_res_ind);
      if (harq_res == nullptr) {
        return std::nullopt;
      }
      new_resource.harq_id.pucch_set_idx = pucch_res_set_id::set_1;
      new_resource.harq_id.pucch_res_ind = r_harq->harq_id.pucch_res_ind;
      new_resource.res                   = harq_res;
    }
    new_resource.bits.harq_ack_nof_bits  = r_harq->bits.harq_ack_nof_bits;
    new_resource.bits.sr_bits            = r_sr->bits.sr_bits;
    new_resource.bits.csi_part1_nof_bits = r_csi->bits.csi_part1_nof_bits;

    // Check if the UCI payload fits in the PUCCH resource.
    if (new_resource.bits.get_total_bits() > get_max_payload(new_resource.res->format())) {
      return std::nullopt;
    }
    return new_resource;
  }

  return std::nullopt;
}

std::optional<unsigned> pucch_allocator_impl::allocate_grants(pucch_resource_manager::ue_reservation_guard& guard,
                                                              cell_slot_resource_allocator& pucch_slot_alloc,
                                                              ue_grants&                    existing_pucchs,
                                                              const pucch_grant_list&       grants_to_tx,
                                                              const alloc_context&          alloc_ctx)
{
  auto& pucch_pdus = pucch_slot_alloc.result.ul.pucchs;

  const rnti_t crnti = guard.get_rnti();

  // Retrieve the existing PUCCH PDUs.
  pucch_existing_pdus_handler existing_pdus(crnti, pucch_pdus, cell_cfg.params.init_bwp.pucch.resources);

  // Check if we can fit the new PUCCH PDUs in the output results.
  unsigned nof_extra_grants = grants_to_tx.get_nof_grants() >= existing_pdus.get_nof_unallocated_pdu()
                                  ? grants_to_tx.get_nof_grants() - existing_pdus.get_nof_unallocated_pdu()
                                  : 0U;
  if (not is_there_space_for_new_pucch_grants(pucch_slot_alloc.result, nof_extra_grants)) {
    alloc_ctx.log_skipped_alloc(logger.info, "max number of UL/PUCCH grants reached");
    return std::nullopt;
  }

  bool existing_sr_grant_updated  = false;
  bool existing_csi_grant_updated = false;

  // If there was a CSI grant, re-use the previous one and update the UCI bits with SR.
  if (grants_to_tx.csi.has_value() and existing_pucchs.pucch_grants.csi.has_value() and
      existing_pdus.csi_pdu != nullptr) {
    const auto& csi_pdu = *existing_pdus.csi_pdu;
    const auto& csi_res = grants_to_tx.csi.value();
    ocudu_assert(csi_res.res->format() == pucch_format::FORMAT_2 or csi_res.res->format() == pucch_format::FORMAT_3 or
                     csi_res.res->format() == pucch_format::FORMAT_4,
                 "Invalid PUCCH Format for CSI resource (should be 2, 3 or 4)");

    existing_pdus.update_csi_pdu_bits(csi_res.bits.csi_part1_nof_bits, csi_res.bits.sr_bits);
    alloc_ctx.log_pdu_alloc(logger.debug, csi_pdu, "CSI", true);
    existing_csi_grant_updated = true;
  }
  // If there was a SR grant, re-use the previous one and update UCI bits with HARQ bits.
  else if (grants_to_tx.sr.has_value() and existing_pucchs.pucch_grants.sr.has_value() and
           existing_pdus.sr_pdu != nullptr) {
    const auto& sr_pdu = *existing_pdus.sr_pdu;
    const auto& sr_res = grants_to_tx.sr.value();
    ocudu_assert(sr_res.res->format() == pucch_format::FORMAT_0 or sr_res.res->format() == pucch_format::FORMAT_1,
                 "Invalid PUCCH Format for SR resource (should be 0 or 1)");

    existing_pdus.update_sr_pdu_bits(sr_res.bits.sr_bits, sr_res.bits.harq_ack_nof_bits);
    alloc_ctx.log_pdu_alloc(logger.debug, sr_pdu, "SR", true);
    existing_sr_grant_updated = true;
  }

  if (grants_to_tx.csi.has_value() and not existing_csi_grant_updated) {
    const auto& csi_res = grants_to_tx.csi.value();
    ocudu_assert(csi_res.res->format() == pucch_format::FORMAT_2 or csi_res.res->format() == pucch_format::FORMAT_3 or
                     csi_res.res->format() == pucch_format::FORMAT_4,
                 "Invalid PUCCH Format for CSI resource (should be 2, 3, or 4)");
    ocudu_assert(csi_res.bits.harq_ack_nof_bits == 0, "CSI grant cannot contain HARQ-ACK bits");

    pucch_info* csi_pdu = existing_pdus.get_next_pdu(pucch_pdus);
    ocudu_assert(csi_pdu != nullptr and csi_res.res != nullptr,
                 "Neither the (CSI) return grant nor the PUCCH res configuration can be nullptr");
    fill_ded_pdu(*csi_pdu, *csi_res.res, csi_res.bits, crnti, false);
    alloc_ctx.log_pdu_alloc(logger.debug, *csi_pdu, "CSI", false);
  }

  if (grants_to_tx.sr.has_value() and not existing_sr_grant_updated) {
    const auto& sr_res = grants_to_tx.sr.value();
    ocudu_assert(sr_res.res->format() == pucch_format::FORMAT_0 or sr_res.res->format() == pucch_format::FORMAT_1,
                 "Invalid PUCCH Format for SR resource (should be 0 or 1)");

    pucch_info* sr_pdu = existing_pdus.get_next_pdu(pucch_pdus);
    ocudu_assert(sr_pdu != nullptr and sr_res.res != nullptr,
                 "Neither the (SR) return grant nor the PUCCH resource configuration can be nullptr");
    fill_ded_pdu(*sr_pdu, *sr_res.res, sr_res.bits, crnti, false);
    alloc_ctx.log_pdu_alloc(logger.debug, *sr_pdu, "SR", false);
  }

  if (grants_to_tx.harq_ack.has_value()) {
    const auto& harq_res = grants_to_tx.harq_ack.value();

    pucch_info* harq_pdu = existing_pdus.get_next_pdu(pucch_pdus);
    ocudu_assert(harq_pdu != nullptr and harq_res.res != nullptr,
                 "Neither the (HARQ-ACK) return grant nor the PUCCH resource configuration can be nullptr");
    fill_ded_pdu(*harq_pdu, *harq_res.res, harq_res.bits, crnti, true);
    alloc_ctx.log_pdu_alloc(logger.debug, *harq_pdu, "HARQ-ACK", false);
  }

  // Remove unused PUCCH PDU, if any.
  existing_pdus.remove_unused_pdus(pucch_pdus, crnti);

  // Remove the previously allocated PUCCH resources which are not needed after the new allocation.
  remove_unused_pucch_res(guard, existing_pucchs, grants_to_tx);

  // Update the new grants to the UE allocation record.
  existing_pucchs.pucch_grants = grants_to_tx;

  // Finalize the reservation of the resources.
  guard.commit();

  // The return value is only relevant if the allocation was called for a HARQ-ACK grant.
  return grants_to_tx.harq_ack.has_value() ? grants_to_tx.harq_ack.value().harq_id.pucch_res_ind : 0U;
}

///////////////  Private helpers   ///////////////

bool pucch_allocator_impl::can_allocate_pucch(const cell_slot_resource_allocator& pucch_slot_alloc,
                                              const ue_grants*                    existing_ue_grants,
                                              const alloc_context&                alloc_ctx) const
{
  // [Implementation-defined] We only allocate PUCCH grants on fully UL slots.
  if (not cell_cfg.is_fully_ul_enabled(pucch_slot_alloc.slot)) {
    return false;
  }

  // Check if there is space in the PUCCH grants list of the slot.
  const auto& slot_ctx = slots_ctx[pucch_slot_alloc.slot.to_uint()];
  if (existing_ue_grants == nullptr and slot_ctx.ue_grants_map.size() == slot_ctx.ue_grants_map.max_size()) {
    alloc_ctx.log_skipped_alloc(logger.info, "PUCCH allocator grant list is full");
    return false;
  }

  return true;
}

bool pucch_allocator_impl::can_allocate_fallback_pucch(const cell_slot_resource_allocator& pucch_slot_alloc,
                                                       const ue_grants*                    existing_ue_grants,
                                                       const alloc_context&                alloc_ctx) const
{
  // The UE can't multiplex PUCCH and PUSCH during fallback, so skip PUCCH if there is an existing PUSCH for that UE.
  // Note: if the PUSCH and PUCCH don't overlap in OFDM symbols, they wouldn't require multiplexing and we could
  // schedule both, but this will never happen in the current implementation due to:
  // - PUSCH taking all symbols unless SRS is configured for the last N symbols.
  // - SRS using all RBs.
  if (std::any_of(pucch_slot_alloc.result.ul.puschs.begin(),
                  pucch_slot_alloc.result.ul.puschs.end(),
                  [rnti = alloc_ctx.rnti](const ul_sched_info& pusch) { return pusch.pusch_cfg.rnti == rnti; })) {
    alloc_ctx.log_skipped_alloc(logger.debug, "existing PUSCH for the same UE");
    return false;
  }

  if (existing_ue_grants != nullptr) {
    // As per Section 9.2.1, TS 38.213:
    // - If a UE is not provided pdsch-HARQ-ACK-Codebook, the UE generates at most one HARQ-ACK information bit.
    // Since we don't ever expect to have multiple HARQ-ACK bits to be sent in the same slot during fallback, there
    // shouldn't be any existing HARQ-ACK PUCCH grants for this UE. This is a defensive check for possible bugs.
    const bool has_existing_ded_harq_ack_grant = existing_ue_grants->pucch_grants.harq_ack.has_value();
    const bool has_existing_common_grant       = existing_ue_grants->has_common_pucch;
    if (has_existing_common_grant or has_existing_ded_harq_ack_grant) {
      alloc_ctx.log_skipped_alloc(
          logger.error, "existing PUCCH with HARQ-ACK bits for the same UE (should never happen in fallback)");
      return false;
    }

    ocudu_assert(
        not(existing_ue_grants->pucch_grants.sr.has_value() and existing_ue_grants->pucch_grants.csi.has_value()),
        "It is expected that there are either no grants, or at most 1 PUCCH grant (SR grant or CSI grant)");
  }

  return true;
}

bool pucch_allocator_impl::is_there_space_for_new_pucch_grants(const sched_result& slot_result,
                                                               unsigned            nof_grants_to_allocate) const

{
  int max_nof_pucch_grants = slot_result.ul.pucchs.capacity();
  // [Implementation-defined] We only allow a max number of PUCCH grants per slot.
  max_nof_pucch_grants = std::min(max_nof_pucch_grants, static_cast<int>(max_pucch_grants_per_slot));
  // [Implementation-defined] We only allow a max number of PUCCH + PUSCH grants per slot.
  max_nof_pucch_grants = std::min(
      max_nof_pucch_grants, static_cast<int>(max_ul_grants_per_slot) - static_cast<int>(slot_result.ul.puschs.size()));

  const int nof_total_pucchs = slot_result.ul.pucchs.size() + nof_grants_to_allocate;
  return nof_total_pucchs <= max_nof_pucch_grants;
}

unsigned pucch_allocator_impl::get_max_payload(pucch_format format) const
{
  switch (format) {
    case pucch_format::FORMAT_0:
    case pucch_format::FORMAT_1:
      // PUCCH Format 0 and 1 can carry at most 2 HARQ-ACK bits.
      return 2U;
    case pucch_format::FORMAT_2:
    case pucch_format::FORMAT_3:
    case pucch_format::FORMAT_4:
      return res_params.max_payload_234();
    default:
      ocudu_assertion_failure("Invalid PUCCH format");
      return 0;
  }
}

void pucch_allocator_impl::fill_common_pdu(pucch_info& pucch_pdu, const pucch_resource& common_res, rnti_t rnti) const
{
  pucch_pdu.crnti   = rnti;
  pucch_pdu.bwp_cfg = &cell_cfg.params.ul_cfg_common.init_ul_bwp.generic_params;
  pucch_pdu.res     = &common_res;

  const pucch_config_common& pucch_cmn = *cell_cfg.params.ul_cfg_common.init_ul_bwp.pucch_cfg_common;
  const unsigned             n_id_hop =
      pucch_cmn.hopping_id.has_value() ? *pucch_cmn.hopping_id : static_cast<unsigned>(cell_cfg.params.pci);

  switch (common_res.format()) {
    case pucch_format::FORMAT_0: {
      auto& f0         = pucch_pdu.format_params.emplace<pucch_info::f0_config>();
      f0.group_hopping = pucch_cmn.group_hopping;
      f0.n_id_hopping  = n_id_hop;
      // SR cannot be reported using common PUCCH resources.
      pucch_pdu.uci_bits.sr_bits = sr_nof_bits::no_sr;
      // [Implementation-defined] For the default PUCCH resources, we assume only 1 HARQ-ACK process needs to be
      // reported.
      pucch_pdu.uci_bits.harq_ack_nof_bits = 1;
      break;
    }
    case pucch_format::FORMAT_1: {
      auto& format_1         = pucch_pdu.format_params.emplace<pucch_info::f1_config>();
      format_1.group_hopping = pucch_cmn.group_hopping;
      format_1.n_id_hopping  = n_id_hop;
      // SR cannot be reported using common PUCCH resources.
      pucch_pdu.uci_bits.sr_bits = sr_nof_bits::no_sr;
      // [Implementation-defined] For the default PUCCH resources, we assume only 1 HARQ-ACK process needs to be
      // reported.
      pucch_pdu.uci_bits.harq_ack_nof_bits = 1;
      // This option can be configured with Dedicated PUCCH resources.
      format_1.slot_repetition = pucch_repetition_tx_slot::no_multi_slot;
      break;
    }
    default:
      ocudu_assertion_failure("Only PUCCH Format 0 and 1 can be used for PUCCH common resources");
  }
}

void pucch_allocator_impl::fill_ded_pdu(pucch_info&           pucch_pdu,
                                        const pucch_resource& pucch_res,
                                        const pucch_uci_bits& uci_bits,
                                        rnti_t                rnti,
                                        bool                  adjust_prbs) const
{
  pucch_pdu.bwp_cfg = &cell_cfg.params.ul_cfg_common.init_ul_bwp.generic_params;
  pucch_pdu.res     = &pucch_res;

  pucch_pdu.crnti   = rnti;
  pucch_pdu.bwp_cfg = &cell_cfg.params.ul_cfg_common.init_ul_bwp.generic_params;

  if (pucch_res.format() == pucch_format::FORMAT_0 or pucch_res.format() == pucch_format::FORMAT_1) {
    ocudu_assert(pucch_pdu.uci_bits.harq_ack_nof_bits <= 2, "PUCCH F0/1 can carry 2 HARQ-ACK bits at most");
    ocudu_assert(pucch_pdu.uci_bits.sr_bits == sr_nof_bits::no_sr or pucch_pdu.uci_bits.sr_bits == sr_nof_bits::one,
                 "PUCCH F0/1 can carry 1 SR bit at most");
    ocudu_assert(pucch_pdu.uci_bits.csi_part1_nof_bits == 0, "PUCCH F0/1 can't carry CSI bits");
  }
  pucch_pdu.uci_bits = uci_bits;
  // Generate CSI report configuration if there are CSI bits in UCI.
  if (pucch_pdu.uci_bits.csi_part1_nof_bits > 0) {
    pucch_pdu.csi_rep_cfg = csi_report_cfg.value();
  }

  switch (pucch_res.format()) {
    case pucch_format::FORMAT_0: {
      auto& format_0 = pucch_pdu.format_params.emplace<pucch_info::f0_config>();

      // \c pucch-GroupHopping and \c hoppingId are set as per TS 38.211, Section 6.3.2.2.1.
      format_0.group_hopping = cell_cfg.params.ul_cfg_common.init_ul_bwp.pucch_cfg_common->group_hopping;
      format_0.n_id_hopping  = cell_cfg.params.ul_cfg_common.init_ul_bwp.pucch_cfg_common->hopping_id.has_value()
                                   ? cell_cfg.params.ul_cfg_common.init_ul_bwp.pucch_cfg_common->hopping_id.value()
                                   : cell_cfg.params.pci;
    } break;
    case pucch_format::FORMAT_1: {
      auto& format_1 = pucch_pdu.format_params.emplace<pucch_info::f1_config>();

      // \c pucch-GroupHopping and \c hoppingId are set as per TS 38.211, Section 6.3.2.2.1.
      format_1.group_hopping = cell_cfg.params.ul_cfg_common.init_ul_bwp.pucch_cfg_common->group_hopping;
      format_1.n_id_hopping  = cell_cfg.params.ul_cfg_common.init_ul_bwp.pucch_cfg_common->hopping_id.has_value()
                                   ? cell_cfg.params.ul_cfg_common.init_ul_bwp.pucch_cfg_common->hopping_id.value()
                                   : cell_cfg.params.pci;
      // For PUCCH Format 1, only 1 SR bit.
      // [Implementation-defined] We do not implement PUCCH over several slots.
      format_1.slot_repetition = pucch_repetition_tx_slot::no_multi_slot;
    } break;
    case pucch_format::FORMAT_2: {
      auto& f2 = pucch_pdu.format_params.emplace<pucch_info::f2_config>();

      f2.n_id_scrambling    = n_id_scrambling();
      f2.n_id_0_scrambling  = n_id_0_scrambling();
      const auto max_c_rate = to_float(cell_cfg.params.init_bwp.pucch.resources.max_code_rate_234());
      f2.nof_prbs           = adjust_prbs
                                  ? get_pucch_format2_nof_prbs(
                              uci_bits.get_total_bits(), pucch_res.prbs().length(), pucch_res.syms.length(), max_c_rate)
                                  : pucch_res.prbs().length();
    } break;
    case pucch_format::FORMAT_3: {
      const auto& res_f3 = std::get<pucch_resource::f3_config>(pucch_res.format_params);
      auto&       f3     = pucch_pdu.format_params.emplace<pucch_info::f3_config>();

      f3.group_hopping = cell_cfg.params.ul_cfg_common.init_ul_bwp.pucch_cfg_common->group_hopping;
      f3.n_id_hopping  = cell_cfg.params.ul_cfg_common.init_ul_bwp.pucch_cfg_common->hopping_id.has_value()
                             ? cell_cfg.params.ul_cfg_common.init_ul_bwp.pucch_cfg_common->hopping_id.value()
                             : cell_cfg.params.pci;
      // [Implementation-defined] We do not implement PUCCH over several slots.
      f3.slot_repetition    = pucch_repetition_tx_slot::no_multi_slot;
      f3.n_id_scrambling    = n_id_scrambling();
      f3.n_id_0_scrambling  = n_id_0_scrambling();
      const auto max_c_rate = to_float(cell_cfg.params.init_bwp.pucch.resources.max_code_rate_234());
      f3.nof_prbs           = adjust_prbs ? get_pucch_format3_nof_prbs(uci_bits.get_total_bits(),
                                                             pucch_res.prbs().length(),
                                                             pucch_res.syms.length(),
                                                             max_c_rate,
                                                             pucch_res.second_hop_prb.has_value(),
                                                             res_f3.additional_dmrs,
                                                             res_f3.pi_2_bpsk)
                                          : pucch_res.prbs().length();
    } break;
    case pucch_format::FORMAT_4: {
      auto& f4 = pucch_pdu.format_params.emplace<pucch_info::f4_config>();

      f4.group_hopping = cell_cfg.params.ul_cfg_common.init_ul_bwp.pucch_cfg_common->group_hopping;
      f4.n_id_hopping  = cell_cfg.params.ul_cfg_common.init_ul_bwp.pucch_cfg_common->hopping_id.has_value()
                             ? cell_cfg.params.ul_cfg_common.init_ul_bwp.pucch_cfg_common->hopping_id.value()
                             : cell_cfg.params.pci;
      // [Implementation-defined] We do not implement PUCCH over several slots.
      f4.slot_repetition   = pucch_repetition_tx_slot::no_multi_slot;
      f4.n_id_scrambling   = n_id_scrambling();
      f4.n_id_0_scrambling = n_id_0_scrambling();
    } break;
    default:
      ocudu_assertion_failure("Invalid PUCCH format");
  }
}

void pucch_allocator_impl::remove_unused_pucch_res(pucch_resource_manager::ue_reservation_guard& guard,
                                                   const ue_grants&                              existing_pucchs,
                                                   const pucch_grant_list&                       grants_to_tx)
{
  // Remove the PUCCH resources by evaluating the difference between the previously allocated resources and the
  // current ones.

  // When using Formats 0 and 2, the special resources SR_F0, SR_F2, CSI_F0 and CSI_F2 are used when multiplexing HARQ
  // with SR/CSI. When reserving these resources through the pucch_resource_manager, we don't allocate them in the
  // pucch_collision_manager. This is because:
  // - SR_F2 and CSI_F0: these resources are not present in the cell PUCCH resource list, so they can't be managed by
  // the pucch_collision_manager.
  // - SR_F0 and CSI_F2: these resources point to the same cell resource as the SR and CSI resources, respectively, so
  // they were already allocated in the collision manager when reserving the SR and CSI resources.
  // Therefore, after multiplexing HARQ with SR/CSI in this case, we need to take care not to release the SR/CSI
  // resources, or we will risk collisions of the PUCCHs with other UL grants.
  if (existing_pucchs.pucch_grants.csi.has_value() and not grants_to_tx.csi.has_value()) {
    // TODO: If using Formats 0 and 2 and HARQ+CSI+SR are all multiplexed together, we should release the CSI resource,
    // since the UE will use SR_F0 or SR_F2. However, this case is not yet supported.
    if (not(cell_cfg.is_pucch_f0_and_f2() and grants_to_tx.harq_ack.has_value())) {
      guard.release_csi_resource();
    }
  }
  if (existing_pucchs.pucch_grants.sr.has_value() and not grants_to_tx.sr.has_value()) {
    if (not(cell_cfg.is_pucch_f0_and_f2() and grants_to_tx.harq_ack.has_value())) {
      guard.release_sr_resource();
    }
  }

  if (existing_pucchs.pucch_grants.harq_ack.has_value() and
      (not grants_to_tx.harq_ack.has_value() or
       existing_pucchs.pucch_grants.harq_ack->res->format() != grants_to_tx.harq_ack->res->format())) {
    if (existing_pucchs.pucch_grants.harq_ack.value().harq_id.pucch_set_idx == pucch_res_set_id::set_0) {
      guard.release_harq_set_0_resource();
    } else {
      guard.release_harq_set_1_resource();
    }
  }
}
