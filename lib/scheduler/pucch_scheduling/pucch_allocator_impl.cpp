// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "pucch_allocator_impl.h"
#include "pucch_allocator_helpers.h"
#include "pucch_collision_manager.h"
#include "ocudu/ocudulog/log_channel.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/ran/csi_report/csi_report_config_helpers.h"
#include "ocudu/ran/csi_report/csi_report_on_pucch_helpers.h"
#include "ocudu/ran/pucch/pucch_configuration.h"
#include "ocudu/ran/pucch/pucch_uci_bits.h"
#include "ocudu/ran/resource_allocation/ofdm_symbol_range.h"
#include "ocudu/scheduler/config/pucch_default_resource.h"
#include "ocudu/scheduler/config/serving_cell_config_factory.h"
#include "ocudu/scheduler/resource_grid_util.h"
#include "ocudu/scheduler/result/pucch_info.h"
#include "ocudu/scheduler/result/sched_result.h"
#include "ocudu/support/ocudu_assert.h"
#include "fmt/std.h"
#include <algorithm>
#include <fmt/format.h>
#include <string>

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
  cell_resources(cell_cfg_.bwp_res[to_bwp_id(0)].ul().pucch),
  res_params(cell_cfg.params.init_bwp.pucch.resources),
  csi_report_cfg(get_csi_report_cfg(cell_cfg.params)),
  col_manager(cell_cfg_),
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

  col_manager.slot_indication(sl_tx);

  // Clear previous slot PUCCH grants allocations.
  slots_ctx[(sl_tx - 1).to_uint()].clear();
}

void pucch_allocator_impl::stop()
{
  col_manager.stop();
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

  if (existing_ue_grants != nullptr) {
    // Release resources previously allocated to this UE from the resource manager.
    free_resources(pucch_slot_alloc, *existing_ue_grants, tcrnti);
  }

  // Try to get an available PUCCH common resource for HARQ-ACK.
  std::optional<unsigned> d_pri;
  // As per Section 9.2.1, TS 38.213, this is the number of available \f$\Delta_{PRI}\f$, which is a 3-bit unsigned.
  static constexpr unsigned nof_d_pri = 8U;
  // Loop over the values of \Delta_PRI to find an available common resource that doesn't collide with existing grants.
  for (unsigned pri = 0; pri != nof_d_pri; ++pri) {
    const auto& res = pucch_helper::get_common_resource(cell_cfg, dci_info.ctx, pri);
    if (col_manager.can_alloc(pucch_slot_alloc, res, alloc_ctx.rnti)) {
      d_pri = pri;
      break;
    }
  }

  if (not d_pri.has_value()) {
    if (existing_ue_grants != nullptr) {
      // Restore the previous allocation in the resource manager, since the new allocation failed.
      alloc_resources(pucch_slot_alloc, *existing_ue_grants, tcrnti);
    }

    alloc_ctx.log_skipped_alloc(logger.debug, "no common resource available");
    return std::nullopt;
  }

  // Update the UE grants.
  ue_grants grants               = existing_ue_grants != nullptr ? *existing_ue_grants : ue_grants{};
  grants.common                  = pucch_slot_alloc.result.ul.pucchs.emplace();
  grants.d_pri                   = *d_pri;
  slot_ctx.ue_grants_map[tcrnti] = grants;

  // Fill scheduler output.
  pucch_info& common_pdu = pucch_slot_alloc.result.ul.pucchs[*grants.common];
  pucch_helper::fill_common_pdu(
      common_pdu, cell_cfg, pucch_helper::get_common_resource(cell_cfg, dci_info.ctx, *d_pri), tcrnti);

  // Allocate the resources in the resource manager.
  alloc_resources(pucch_slot_alloc, grants, tcrnti);

  return d_pri;
}

std::optional<unsigned> pucch_allocator_impl::alloc_common_and_ded_harq_ack(cell_resource_allocator&     res_alloc,
                                                                            const ue_cell_configuration& ue_cell_cfg,
                                                                            unsigned                     k0,
                                                                            unsigned                     k1,
                                                                            const pdcch_dl_information&  dci_info)
{
  // Get the slot allocation grid considering the PDSCH delay (k0) and the PUCCH delay wrt PDSCH (k1).
  cell_slot_resource_allocator& pucch_slot_alloc = res_alloc[k0 + k1 + res_alloc.cfg.ntn_cs_koffset];
  auto&                         slot_ctx         = slots_ctx[pucch_slot_alloc.slot.to_uint()];
  alloc_context alloc_ctx{alloc_context::alloc_type::common_and_ded_harq_ack, ue_cell_cfg.crnti, pucch_slot_alloc.slot};

  ue_grants* existing_grants = slot_ctx.find_ue_grants(ue_cell_cfg.crnti);
  if (not can_allocate_pucch(pucch_slot_alloc, existing_grants, alloc_ctx)) {
    return std::nullopt;
  }

  if (not can_allocate_fallback_pucch(pucch_slot_alloc, existing_grants, alloc_ctx)) {
    return std::nullopt;
  }

  ue_grants old_grants{};
  if (existing_grants != nullptr) {
    old_grants = *existing_grants;
    // Release resources previously allocated to this UE from the resource manager.
    free_resources(pucch_slot_alloc, old_grants, ue_cell_cfg.crnti);
  }

  pucch_uci_bits uci_bits = old_grants.uci_bits(pucch_slot_alloc.result.ul.pucchs);
  ++uci_bits.harq_ack_nof_bits;
  ocudu_assert(uci_bits.harq_ack_nof_bits == 1U, "PUCCH grant for HARQ-ACK has already been allocated");

  auto d_pri = select_pri(pucch_slot_alloc, ue_cell_cfg, uci_bits, &dci_info.ctx);
  if (not d_pri.has_value()) {
    if (existing_grants != nullptr) {
      // Restore the previous allocation in the resource manager, since the new allocation failed.
      alloc_resources(pucch_slot_alloc, old_grants, ue_cell_cfg.crnti);
    }
    alloc_ctx.log_skipped_alloc(logger.debug, "no PRI available for both common and dedicated resources");
    return std::nullopt;
  }

  auto new_grants = multiplex_and_allocate_pucch(pucch_slot_alloc, uci_bits, old_grants, ue_cell_cfg, d_pri, alloc_ctx);
  if (not new_grants.has_value()) {
    if (existing_grants != nullptr) {
      // Restore the previous allocation in the resource manager, since the new allocation failed.
      alloc_resources(pucch_slot_alloc, old_grants, ue_cell_cfg.crnti);
    }
    return std::nullopt;
  }

  // Update the UE grants.
  new_grants->common                        = pucch_slot_alloc.result.ul.pucchs.emplace();
  slot_ctx.ue_grants_map[ue_cell_cfg.crnti] = *new_grants;

  // Fill scheduler output.
  pucch_info& common_pdu = pucch_slot_alloc.result.ul.pucchs[*new_grants->common];
  pucch_helper::fill_common_pdu(
      common_pdu, cell_cfg, pucch_helper::get_common_resource(cell_cfg, dci_info.ctx, *d_pri), ue_cell_cfg.crnti);

  // Allocate the resources in the resource manager.
  alloc_resources(pucch_slot_alloc, *new_grants, ue_cell_cfg.crnti);

  return d_pri;
}

std::optional<unsigned> pucch_allocator_impl::alloc_ded_harq_ack(cell_resource_allocator&     res_alloc,
                                                                 const ue_cell_configuration& ue_cell_cfg,
                                                                 unsigned                     k0,
                                                                 unsigned                     k1)
{
  // NOTE: This function does not check whether there are PUSCH grants allocated for the same UE. The check needs to
  // be performed by the caller.

  // Get the slot allocation grid considering the PDSCH delay (k0) and the PUCCH delay wrt PDSCH (k1).
  cell_slot_resource_allocator& pucch_slot_alloc = res_alloc[k0 + k1 + res_alloc.cfg.ntn_cs_koffset];
  auto&                         slot_ctx         = slots_ctx[pucch_slot_alloc.slot.to_uint()];
  alloc_context alloc_ctx{alloc_context::alloc_type::ded_harq_ack, ue_cell_cfg.crnti, pucch_slot_alloc.slot};

  ue_grants* existing_grants = slot_ctx.find_ue_grants(ue_cell_cfg.crnti);
  if (not can_allocate_pucch(pucch_slot_alloc, existing_grants, alloc_ctx)) {
    return std::nullopt;
  }

  // As per Section 9.2.1, TS 38.213:
  // - If a UE is not provided pdsch-HARQ-ACK-Codebook, the UE generates at most one HARQ-ACK information bit.
  // Multiplexing of multiple HARQ-ACK bits in a PUCCH common grant is not allowed.
  if (existing_grants != nullptr and existing_grants->common.has_value()) {
    alloc_ctx.log_skipped_alloc(logger.debug, "existing common PUCCH grant for the same UE");
    return std::nullopt;
  }

  ue_grants old_grants{};
  if (existing_grants != nullptr) {
    old_grants = *existing_grants;
  }

  pucch_uci_bits new_bits = old_grants.uci_bits(pucch_slot_alloc.result.ul.pucchs);
  ++new_bits.harq_ack_nof_bits;

  // From TS 38.213, Section 9.2.1:
  // > "If the UE transmits O_UCI UCI information bits, that include HARQ-ACK information bits, the UE determines a
  //    PUCCH resource set to be ..."
  // We can infer that we only need to run the multiplexing algorithm in the following cases:
  // - the first allocated HARQ-ACK bit (to multiplex the new HARQ-ACK resource with the existing grants)
  // - the third allocated HARQ-ACK bit (to promote from Resource Set ID 0 to Resource Set ID 1)
  // In all other cases, the multiplexing algorithm would yield the same result as in the previous allocation of this
  // UE, so we skip it.
  if (existing_grants != nullptr and new_bits.harq_ack_nof_bits != 1U and new_bits.harq_ack_nof_bits != 3U) {
    return update_harq_ack_bits(pucch_slot_alloc, old_grants, new_bits.harq_ack_nof_bits, alloc_ctx);
  }

  if (existing_grants != nullptr) {
    // Release resources previously allocated to this UE from the resource manager before re-running the multiplexing.
    free_resources(pucch_slot_alloc, old_grants, ue_cell_cfg.crnti);
  }

  std::optional<unsigned> d_pri;
  if (new_bits.harq_ack_nof_bits == 1U) {
    d_pri = select_pri(pucch_slot_alloc, ue_cell_cfg, new_bits, nullptr);

    if (not d_pri.has_value()) {
      if (existing_grants != nullptr) {
        // Restore the previous allocation in the resource manager, since the new allocation failed.
        alloc_resources(pucch_slot_alloc, old_grants, ue_cell_cfg.crnti);
      }
      alloc_ctx.log_skipped_alloc(logger.debug, "no resource indicator available for dedicated PUCCH resource");
      return std::nullopt;
    }
  }

  auto new_grants = multiplex_and_allocate_pucch(
      pucch_slot_alloc, new_bits, old_grants, ue_cell_cfg, d_pri.has_value() ? *d_pri : old_grants.d_pri, alloc_ctx);

  if (not new_grants.has_value()) {
    if (existing_grants != nullptr) {
      // Restore the previous allocation in the resource manager, since the new allocation failed.
      alloc_resources(pucch_slot_alloc, old_grants, ue_cell_cfg.crnti);
    }
    return std::nullopt;
  }

  // Update the UE grants and allocate the resources in the resource manager.
  slot_ctx.ue_grants_map[ue_cell_cfg.crnti] = *new_grants;
  alloc_resources(pucch_slot_alloc, *new_grants, ue_cell_cfg.crnti);

  return new_grants->d_pri;
}

bool pucch_allocator_impl::alloc_sr_opportunity(cell_slot_resource_allocator& pucch_slot_alloc,
                                                const ue_cell_configuration&  ue_cell_cfg)
{
  auto&         slot_ctx = slots_ctx[pucch_slot_alloc.slot.to_uint()];
  alloc_context alloc_ctx{alloc_context::alloc_type::sr, ue_cell_cfg.crnti, pucch_slot_alloc.slot};

  ue_grants* existing_grants = slot_ctx.find_ue_grants(ue_cell_cfg.crnti);
  if (not can_allocate_pucch(pucch_slot_alloc, existing_grants, alloc_ctx)) {
    return false;
  }

  if (existing_grants != nullptr and cell_cfg.is_pucch_f0_and_f2() and existing_grants->harq_ack.has_value()) {
    // In the F0+F2 case, the PRI used for the HARQ-ACK is restricted if CSI or SR is multiplexed with it.
    // If HARQ-ACK has already been allocated in this slot, the PRI has already been set and could be incompatible.
    alloc_ctx.log_skipped_alloc(logger.info, "existing HARQ-ACK grant");
    return false;
  }

  ue_grants old_grants{};
  if (existing_grants != nullptr) {
    old_grants = *existing_grants;
    // Release resources previously allocated to this UE from the resource manager.
    free_resources(pucch_slot_alloc, old_grants, ue_cell_cfg.crnti);
  }

  pucch_uci_bits uci_bits = old_grants.uci_bits(pucch_slot_alloc.result.ul.pucchs);
  ocudu_assert(uci_bits.sr_bits == sr_nof_bits::no_sr, "SR has already been allocated");
  uci_bits.sr_bits = sr_nof_bits::one;

  auto new_grants =
      multiplex_and_allocate_pucch(pucch_slot_alloc, uci_bits, old_grants, ue_cell_cfg, std::nullopt, alloc_ctx);
  if (not new_grants.has_value()) {
    if (existing_grants != nullptr) {
      // Restore the previous allocation in the resource manager, since the new allocation failed.
      alloc_resources(pucch_slot_alloc, old_grants, ue_cell_cfg.crnti);
    }
    return false;
  }

  // Update the UE grants and allocate the resources in the resource manager.
  slot_ctx.ue_grants_map[ue_cell_cfg.crnti] = *new_grants;
  alloc_resources(pucch_slot_alloc, *new_grants, ue_cell_cfg.crnti);

  return true;
}

bool pucch_allocator_impl::alloc_csi_opportunity(cell_slot_resource_allocator& pucch_slot_alloc,
                                                 const ue_cell_configuration&  ue_cell_cfg)
{
  auto&         slot_ctx = slots_ctx[pucch_slot_alloc.slot.to_uint()];
  alloc_context alloc_ctx{alloc_context::alloc_type::csi, ue_cell_cfg.crnti, pucch_slot_alloc.slot};

  ue_grants* existing_grants = slot_ctx.find_ue_grants(ue_cell_cfg.crnti);
  if (not can_allocate_pucch(pucch_slot_alloc, existing_grants, alloc_ctx)) {
    return false;
  }

  if (existing_grants != nullptr and cell_cfg.is_pucch_f0_and_f2() and existing_grants->harq_ack.has_value()) {
    // In the F0+F2 case, the PRI used for the HARQ-ACK is restricted if CSI or SR is multiplexed with it.
    // If HARQ-ACK has already been allocated in this slot, the PRI has already been set and could be incompatible.
    alloc_ctx.log_skipped_alloc(logger.info, "existing HARQ-ACK grant");
    return false;
  }

  ue_grants old_grants{};
  if (existing_grants != nullptr) {
    old_grants = *existing_grants;
    // Release resources previously allocated to this UE from the resource manager.
    free_resources(pucch_slot_alloc, old_grants, ue_cell_cfg.crnti);
  }

  pucch_uci_bits uci_bits = old_grants.uci_bits(pucch_slot_alloc.result.ul.pucchs);
  ocudu_assert(uci_bits.csi_part1_nof_bits == 0U, "CSI has already been allocated");
  uci_bits.csi_part1_nof_bits = get_csi_report_pucch_size(*csi_report_cfg).part1_size.value();

  auto new_grants =
      multiplex_and_allocate_pucch(pucch_slot_alloc, uci_bits, old_grants, ue_cell_cfg, std::nullopt, alloc_ctx);
  if (not new_grants.has_value()) {
    if (existing_grants != nullptr) {
      // Restore the previous allocation in the resource manager, since the new allocation failed.
      alloc_resources(pucch_slot_alloc, old_grants, ue_cell_cfg.crnti);
    }
    return false;
  }

  // Update the UE grants and allocate the resources in the resource manager.
  slot_ctx.ue_grants_map[ue_cell_cfg.crnti] = *new_grants;
  alloc_resources(pucch_slot_alloc, *new_grants, ue_cell_cfg.crnti);

  return true;
}

pucch_uci_bits pucch_allocator_impl::remove_ue_uci_from_pucch(cell_slot_resource_allocator& slot_alloc,
                                                              const ue_cell_configuration&  ue_cell_cfg)
{
  // Get the PUCCH grants for the slot.
  auto&      slot_ctx           = slots_ctx[slot_alloc.slot.to_uint()];
  ue_grants* existing_ue_grants = slot_ctx.find_ue_grants(ue_cell_cfg.crnti);
  if (existing_ue_grants == nullptr) {
    // No PUCCH grants found for the UE in this slot.
    return pucch_uci_bits{};
  }
  ocudu_assert(
      not existing_ue_grants->common.has_value(), "Unexpected common PUCCH grant found for rnti={}", ue_cell_cfg.crnti);

  // Get the UCI bits that were allocated for the UE in this slot.
  pucch_uci_bits removed_uci_info = existing_ue_grants->uci_bits(slot_alloc.result.ul.pucchs);

  // Remove the PUCCH PDUs and free the resources.
  bool all_found = true;
  if (existing_ue_grants->harq_ack.has_value()) {
    const auto& pdu = slot_alloc.result.ul.pucchs[*existing_ue_grants->harq_ack];
    all_found &= col_manager.free(slot_alloc, *pdu.res, ue_cell_cfg.crnti);
    slot_alloc.result.ul.pucchs.erase(*existing_ue_grants->harq_ack);
  }
  if (existing_ue_grants->sr.has_value()) {
    const auto& pdu = slot_alloc.result.ul.pucchs[*existing_ue_grants->sr];
    all_found &= col_manager.free(slot_alloc, *pdu.res, ue_cell_cfg.crnti);
    slot_alloc.result.ul.pucchs.erase(*existing_ue_grants->sr);
  }
  if (existing_ue_grants->csi.has_value()) {
    const auto& pdu = slot_alloc.result.ul.pucchs[*existing_ue_grants->csi];
    all_found &= col_manager.free(slot_alloc, *pdu.res, ue_cell_cfg.crnti);
    slot_alloc.result.ul.pucchs.erase(*existing_ue_grants->csi);
  }
  ocudu_assert(all_found, "Failed to free all PUCCH resources for UE with RNTI {}", ue_cell_cfg.crnti);

  // Erase the UE grants from the slot context.
  slot_ctx.ue_grants_map.erase(ue_cell_cfg.crnti);
  return removed_uci_info;
}

bool pucch_allocator_impl::has_common_pucch_grant(rnti_t rnti, slot_point sl_tx) const
{
  const auto& slot_ctx = slots_ctx[sl_tx.to_uint()];
  auto        it       = slot_ctx.ue_grants_map.find(rnti);
  return it != slot_ctx.ue_grants_map.end() and it->second.common.has_value();
}

//////////////     Sub-class definitions       //////////////

unsigned pucch_allocator_impl::pucch_grant_list::nof_grants() const
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

static_vector<stable_id_t, pucch_allocator_impl::ue_grants::max_nof_ue_grants>
pucch_allocator_impl::ue_grants::pdu_indices(bool include_common) const
{
  static_vector<stable_id_t, max_nof_ue_grants> indices;
  if (include_common and common.has_value()) {
    indices.push_back(*common);
  }
  if (harq_ack.has_value()) {
    indices.push_back(*harq_ack);
  }
  if (sr.has_value()) {
    indices.push_back(*sr);
  }
  if (csi.has_value()) {
    indices.push_back(*csi);
  }
  return indices;
}

unsigned pucch_allocator_impl::ue_grants::nof_grants(bool include_common) const
{
  unsigned nof_grants = 0;
  if (include_common and common.has_value()) {
    ++nof_grants;
  }
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

pucch_uci_bits pucch_allocator_impl::ue_grants::uci_bits(const stable_id_map<pucch_info>& pdus) const
{
  pucch_uci_bits bits{};
  for (auto idx : pdu_indices()) {
    const auto& pdu = pdus[idx];
    // The UCI bits for the UE are the maximum of the UCI bits of all the PUCCH PDUs allocated to the UE.
    bits.harq_ack_nof_bits  = std::max(bits.harq_ack_nof_bits, pdu.uci_bits.harq_ack_nof_bits);
    bits.sr_bits            = std::max(bits.sr_bits, pdu.uci_bits.sr_bits);
    bits.csi_part1_nof_bits = std::max(bits.csi_part1_nof_bits, pdu.uci_bits.csi_part1_nof_bits);
  }
  return bits;
}

///////////////  Main private functions   //////////////

std::optional<unsigned> pucch_allocator_impl::select_pri(const cell_slot_resource_allocator& pucch_slot_alloc,
                                                         const ue_cell_configuration&        ue_cell_cfg,
                                                         const pucch_uci_bits&               bits,
                                                         const dci_context_information*      dci_info)
{
  if (cell_cfg.is_pucch_f0_and_f2() and (bits.csi_part1_nof_bits != 0U or bits.sr_bits != sr_nof_bits::no_sr)) {
    unsigned d_pri =
        bits.sr_bits != sr_nof_bits::no_sr ? res_params.res_set_size.value() : res_params.res_set_size.value() + 1;
    if (dci_info != nullptr) {
      const auto& common_res = pucch_helper::get_common_resource(cell_cfg, *dci_info, d_pri);
      if (not col_manager.can_alloc(pucch_slot_alloc, common_res, ue_cell_cfg.crnti)) {
        return std::nullopt;
      }
    }
    const auto& res0 = pucch_helper::get_harq_resource<0>(ue_cell_cfg, d_pri);
    if (not col_manager.can_alloc(pucch_slot_alloc, res0, ue_cell_cfg.crnti)) {
      return std::nullopt;
    }
    const auto& res1 = pucch_helper::get_harq_resource<1>(ue_cell_cfg, d_pri);
    if (not col_manager.can_alloc(pucch_slot_alloc, res1, ue_cell_cfg.crnti)) {
      return std::nullopt;
    }
    return d_pri;
  }

  for (unsigned d_pri = 0; d_pri != res_params.res_set_size.value(); ++d_pri) {
    if (dci_info != nullptr) {
      const auto& common_res = pucch_helper::get_common_resource(cell_cfg, *dci_info, d_pri);
      if (not col_manager.can_alloc(pucch_slot_alloc, common_res, ue_cell_cfg.crnti)) {
        continue;
      }
    }
    const auto& res0 = pucch_helper::get_harq_resource<0>(ue_cell_cfg, d_pri);
    if (not col_manager.can_alloc(pucch_slot_alloc, res0, ue_cell_cfg.crnti)) {
      continue;
    }
    const auto& res1 = pucch_helper::get_harq_resource<1>(ue_cell_cfg, d_pri);
    if (not col_manager.can_alloc(pucch_slot_alloc, res1, ue_cell_cfg.crnti)) {
      continue;
    }
    return d_pri;
  }
  return std::nullopt;
}

std::optional<pucch_allocator_impl::ue_grants>
pucch_allocator_impl::multiplex_and_allocate_pucch(cell_slot_resource_allocator& pucch_slot_alloc,
                                                   const pucch_uci_bits&         new_bits,
                                                   const ue_grants&              old_grants,
                                                   const ue_cell_configuration&  ue_cell_cfg,
                                                   std::optional<unsigned>       d_pri,
                                                   const alloc_context&          alloc_ctx)
{
  // NOTE: In this function, the \c candidate_grants report the data about the grants BEFORE the multiplexing is
  // applied. Each grant contains only one UCI type (HARQ grant contains HARQ bits, SR grant contains SR bits and so
  // on); on the contrary, \c grants_to_tx contains the grants AFTER the multiplexing; this means that 1 grant can
  // contain more than 1 UCI bit type.

  // Find the grants/resources needed for the UCI bits to be reported, assuming the resources are not multiplexed.
  pucch_grant_list candidate_grants =
      get_resources_pre_multiplexing(ue_cell_cfg, new_bits, d_pri.has_value() ? d_pri : old_grants.d_pri);

  pucch_grant_list new_grants = multiplex_resources(ue_cell_cfg, candidate_grants);
  if (new_grants.nof_grants() == 0U) {
    // Multiplexing failed.
    return std::nullopt;
  }

  // Check that all of the resulting grants can be allocated.
  if (new_grants.harq_ack.has_value()) {
    bool new_res = (not old_grants.harq_ack.has_value()) or
                   (*pucch_slot_alloc.result.ul.pucchs[*old_grants.harq_ack].res != *new_grants.harq_ack->res);
    if (new_res and not col_manager.can_alloc(pucch_slot_alloc, *new_grants.harq_ack->res, alloc_ctx.rnti)) {
      alloc_ctx.log_skipped_alloc(logger.debug, "HARQ-ACK resource not available");
      return std::nullopt;
    }
  }
  if (new_grants.sr.has_value() and not old_grants.sr.has_value()) {
    if (not col_manager.can_alloc(pucch_slot_alloc, *new_grants.sr->res, alloc_ctx.rnti)) {
      alloc_ctx.log_skipped_alloc(logger.debug, "SR resource not available");
      return std::nullopt;
    }
  }
  if (new_grants.csi.has_value() and not old_grants.csi.has_value()) {
    if (not col_manager.can_alloc(pucch_slot_alloc, *new_grants.csi->res, alloc_ctx.rnti)) {
      alloc_ctx.log_skipped_alloc(logger.debug, "CSI resource not available");
      return std::nullopt;
    }
  }

  // Allocate the grants.
  return allocate_grants(pucch_slot_alloc, ue_cell_cfg, old_grants, new_grants, alloc_ctx);
}

pucch_allocator_impl::pucch_grant_list
pucch_allocator_impl::get_resources_pre_multiplexing(const ue_cell_configuration& ue_cell_cfg,
                                                     const pucch_uci_bits&        bits,
                                                     std::optional<unsigned>      d_pri)
{
  pucch_grant_list candidate_resources;

  if (bits.sr_bits != sr_nof_bits::no_sr) {
    candidate_resources.sr.emplace(pucch_grant{.type = pucch_resource_type::sr,
                                               .res  = &pucch_helper::get_sr_resource(ue_cell_cfg),
                                               .bits = {.sr_bits = bits.sr_bits}});
  }

  if (bits.csi_part1_nof_bits != 0U) {
    candidate_resources.csi.emplace(pucch_grant{.type = pucch_resource_type::csi,
                                                .res  = &pucch_helper::get_csi_resource(ue_cell_cfg),
                                                .bits = {.csi_part1_nof_bits = bits.csi_part1_nof_bits}});
  }

  if (bits.harq_ack_nof_bits != 0U) {
    ocudu_assert(d_pri.has_value(), "d_pri must be provided when HARQ-ACK bits are to be allocated");
    auto get_res =
        bits.harq_ack_nof_bits <= 2U ? pucch_helper::get_harq_resource<0> : pucch_helper::get_harq_resource<1>;
    candidate_resources.harq_ack.emplace(pucch_grant{.type = pucch_resource_type::harq_ack,
                                                     .res  = &get_res(ue_cell_cfg, *d_pri),
                                                     .bits = {.harq_ack_nof_bits = bits.harq_ack_nof_bits}});
    candidate_resources.d_pri = *d_pri;
  }

  return candidate_resources;
}

pucch_allocator_impl::pucch_grant_list
pucch_allocator_impl::multiplex_resources(const ue_cell_configuration& ue_cell_cfg,
                                          const pucch_grant_list&      candidate_grants)
{
  // This function implements the multiplexing pseudo-code for PUCCH resources defined in Section 9.2.5, TS 38.213.
  // Refer to paragraph starting as "Set Q to the set of resources for transmission of corresponding PUCCHs in a
  // single slot without repetitions where".

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

  // Implementation of the pseudo-code for multiplexing the resources provided in Section 9.2.5, TS 38.213.
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
            ue_cell_cfg, span<const pucch_grant>(&resource_set_q[j_cnt - o_cnt], o_cnt + 1), candidate_grants.d_pri);
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
  ocudu_assert(resource_set_q.size() == 1U,
               "After the multiplexing procedure, there should be exactly one resource in the set");
  const auto& mplexed_grant = resource_set_q.front();
  if (mplexed_grant.res->format() >= pucch_format::FORMAT_2 and
      mplexed_grant.bits.get_total_bits() > res_params.max_payload_234()) {
    // The new payload wouldn't fit in the resource, so the multiplexing fails.
    return {};
  }

  pucch_grant_list result;
  switch (mplexed_grant.type) {
    case pucch_resource_type::harq_ack:
      result.harq_ack.emplace(mplexed_grant);
      result.d_pri = candidate_grants.d_pri;
      break;
    case pucch_resource_type::sr:
      result.sr.emplace(mplexed_grant);
      break;
    case pucch_resource_type::csi:
      result.csi.emplace(mplexed_grant);
      break;
  }

  if (result.harq_ack.has_value() and result.harq_ack->res->format() == pucch_format::FORMAT_1 and
      resource_set_q.front().bits.sr_bits != sr_nof_bits::no_sr) {
    // The PUCCH resource multiplexing rules are specified from the UE's perspective. When multiplexing HARQ-ACK and SR
    // in a PUCCH Format 1, the SR is signalled by the resource used by the UE (HARQ-ACK res -> -SR; SR res -> +SR).
    // Therefore, from the POV of the gNB, we need to schedule both possible PUCCH transmissions.
    const pucch_resource& sr_res = pucch_helper::get_sr_resource(ue_cell_cfg);
    result.sr.emplace(pucch_grant{.type = pucch_resource_type::sr, .res = &sr_res, .bits = mplexed_grant.bits});
    result.harq_ack->bits.sr_bits = sr_nof_bits::no_sr;
  }
  return result;
}

std::optional<pucch_allocator_impl::pucch_grant>
pucch_allocator_impl::merge_pucch_resources(const ue_cell_configuration& ue_cell_cfg,
                                            span<const pucch_grant>      resources_to_merge,
                                            unsigned                     d_pri)
{
  // This function implements the merging rules for HARQ-ACK, SR and CSI defined in Section 9.2.5.1 and 9.2.5.2,
  // TS 38.213.
  ocudu_assert(resources_to_merge.size() == 2 or resources_to_merge.size() == 3,
               "Invalid number of resources to merge");
  const pucch_grant* harq_ack = nullptr;
  const pucch_grant* sr       = nullptr;
  const pucch_grant* csi      = nullptr;
  for (const pucch_grant& g : resources_to_merge) {
    switch (g.type) {
      case pucch_resource_type::harq_ack:
        ocudu_assert(g.bits.harq_ack_nof_bits != 0U, "The HARQ-ACK resource should carry HARQ-ACK bits");
        ocudu_assert(g.bits.sr_bits == sr_nof_bits::no_sr, "The HARQ-ACK resource should not carry SR bits");
        ocudu_assert(g.bits.csi_part1_nof_bits == 0U, "The HARQ-ACK resource should not carry CSI bits");
        harq_ack = &g;
        break;
      case pucch_resource_type::sr:
        ocudu_assert(g.bits.harq_ack_nof_bits == 0U, "The SR resource should not carry HARQ-ACK bits");
        ocudu_assert(g.bits.sr_bits != sr_nof_bits::no_sr, "The SR resource should carry SR bits");
        ocudu_assert(g.bits.csi_part1_nof_bits == 0U, "The SR resource should not carry CSI bits");
        sr = &g;
        break;
      case pucch_resource_type::csi:
        ocudu_assert(g.bits.harq_ack_nof_bits == 0U, "The CSI resource should not carry HARQ-ACK bits");
        ocudu_assert(g.bits.sr_bits == sr_nof_bits::no_sr, "The CSI resource should not carry SR bits");
        ocudu_assert(g.bits.csi_part1_nof_bits != 0U, "The CSI resource should carry CSI bits");
        csi = &g;
        break;
    }
  }

  // HARQ-ACK and SR (Section 9.2.5.1).
  if (harq_ack and sr and not csi) {
    pucch_grant grant  = *harq_ack;
    grant.bits.sr_bits = sr->bits.sr_bits;
    return grant;
  }

  // SR + CSI (Section 9.2.5.1).
  if (not harq_ack and sr and csi) {
    pucch_grant grant  = *csi;
    grant.bits.sr_bits = sr->bits.sr_bits;
    return grant;
  }

  // HARQ-ACK + CSI (Section 9.2.5.2).
  if (harq_ack and not sr and csi) {
    // Use the HARQ-ACK resource.
    pucch_grant grant             = *harq_ack;
    grant.bits.csi_part1_nof_bits = csi->bits.csi_part1_nof_bits;
    if (grant.res->format() < pucch_format::FORMAT_2) {
      // Promote to a HARQ-ACK resource from Resource Set ID 1, so it can carry the CSI bits.
      const auto& set1_res = pucch_helper::get_harq_resource<1>(ue_cell_cfg, d_pri);
      grant.res            = &set1_res;
    }
    return grant;
  }

  // HARQ-ACK + SR + CSI (Section 9.2.5.2).
  if (harq_ack and sr and csi) {
    pucch_grant grant             = *harq_ack;
    grant.bits.sr_bits            = sr->bits.sr_bits;
    grant.bits.csi_part1_nof_bits = csi->bits.csi_part1_nof_bits;

    if (grant.res->format() < pucch_format::FORMAT_2) {
      // Promote to a HARQ-ACK resource from Resource Set ID 1, so it can carry the CSI bits.
      const auto& set1_res = pucch_helper::get_harq_resource<1>(ue_cell_cfg, d_pri);
      grant.res            = &set1_res;
    }
    return grant;
  }

  ocudu_assertion_failure("Invalid combination of resources to merge");
  return std::nullopt;
}

std::optional<unsigned> pucch_allocator_impl::update_harq_ack_bits(cell_slot_resource_allocator& pucch_slot_alloc,
                                                                   const ue_grants&              grants,
                                                                   unsigned                      harq_ack_nof_bits,
                                                                   const alloc_context&          alloc_ctx)
{
  auto& pucch_pdus = pucch_slot_alloc.result.ul.pucchs;

  for (auto pdu_idx : grants.pdu_indices(false)) {
    pucch_info& pdu = pucch_pdus[pdu_idx];
    ocudu_assert(pdu.uci_bits.harq_ack_nof_bits != 0U, "The PUCCH PDU should carry HARQ-ACK bits");

    // Verify that the resource can fit the UCI bits.
    pucch_uci_bits bits = pdu.uci_bits;
    ++bits.harq_ack_nof_bits;
    if (pdu.format() >= pucch_format::FORMAT_2 and bits.get_total_bits() > res_params.max_payload_234()) {
      alloc_ctx.log_skipped_alloc(logger.debug, "UCI bits exceed PUCCH payload");
      return std::nullopt;
    }

    pucch_helper::fill_ded_pdu(
        pdu, cell_cfg, *pdu.res, bits, csi_report_cfg.has_value() ? &*csi_report_cfg : nullptr, alloc_ctx.rnti);
  }

  return grants.d_pri;
}

std::optional<pucch_allocator_impl::ue_grants>
pucch_allocator_impl::allocate_grants(cell_slot_resource_allocator& pucch_slot_alloc,
                                      const ue_cell_configuration&  ue_cell_cfg,
                                      const ue_grants&              old_grants,
                                      const pucch_grant_list&       new_grants,
                                      const alloc_context&          alloc_ctx)
{
  auto& pucch_pdus = pucch_slot_alloc.result.ul.pucchs;

  // Check if we can fit the new PUCCH PDUs in the output results.
  const unsigned nof_extra_grants = new_grants.nof_grants() >= old_grants.nof_grants(false)
                                        ? new_grants.nof_grants() - old_grants.nof_grants(false)
                                        : 0U;
  if (not is_there_space_for_new_pucch_grants(pucch_slot_alloc.result, nof_extra_grants)) {
    alloc_ctx.log_skipped_alloc(logger.info, "max number of UL/PUCCH grants reached");
    return std::nullopt;
  }

  // Note: we won't be touching the common HARQ-ACK PDU.
  auto pdu_indices = old_grants.pdu_indices(false);
  for (unsigned i = 0; i != nof_extra_grants; ++i) {
    const stable_id_t pdu_idx = pucch_pdus.emplace();
    pdu_indices.push_back(pdu_idx);
  }

  unsigned nof_used_pdus = 0;
  auto     alloc_grant   = [&](const pucch_grant& grant) -> stable_id_t {
    const stable_id_t pdu_idx = pdu_indices[nof_used_pdus++];
    auto&             pdu     = pucch_pdus[pdu_idx];
    pucch_helper::fill_ded_pdu(
        pdu, cell_cfg, *grant.res, grant.bits, csi_report_cfg.has_value() ? &*csi_report_cfg : nullptr, alloc_ctx.rnti);
    return pdu_idx;
  };

  ue_grants result{.common = old_grants.common, .d_pri = old_grants.d_pri};
  if (new_grants.harq_ack.has_value()) {
    result.harq_ack = alloc_grant(*new_grants.harq_ack);
    result.d_pri    = new_grants.d_pri;
  }
  if (new_grants.sr.has_value()) {
    result.sr = alloc_grant(*new_grants.sr);
  }
  if (new_grants.csi.has_value()) {
    result.csi = alloc_grant(*new_grants.csi);
  }

  // Remove unused PUCCH PDU, if any.
  for (unsigned i = nof_used_pdus; i != pdu_indices.size(); ++i) {
    pucch_pdus.erase(pdu_indices[i]);
  }

  return result;
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
    if (existing_ue_grants->common.has_value() or existing_ue_grants->harq_ack.has_value()) {
      ocudu_assertion_failure("It is expected that there are no existing PUCCH grants for the same UE during fallback");
      return false;
    }

    if (existing_ue_grants->sr.has_value() and existing_ue_grants->csi.has_value()) {
      ocudu_assertion_failure(
          "It is expected that there are either no grants, or at most 1 PUCCH grant (SR grant or CSI grant)");
      return false;
    }
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

void pucch_allocator_impl::alloc_resources(cell_slot_resource_allocator& pucch_slot_alloc,
                                           const ue_grants&              grants,
                                           rnti_t                        rnti)
{
  for (auto pdu_idx : grants.pdu_indices()) {
    const auto& res = *pucch_slot_alloc.result.ul.pucchs[pdu_idx].res;
    col_manager.do_alloc(pucch_slot_alloc, res, rnti);
  }
}

void pucch_allocator_impl::free_resources(cell_slot_resource_allocator& pucch_slot_alloc,
                                          const ue_grants&              grants,
                                          rnti_t                        rnti)
{
  for (auto pdu_idx : grants.pdu_indices()) {
    const auto& res   = *pucch_slot_alloc.result.ul.pucchs[pdu_idx].res;
    bool        freed = col_manager.free(pucch_slot_alloc, res, rnti);
    ocudu_assert(freed, "Failed to free PUCCH resource {} for UE with RNTI {}", res.res_id, rnti);
  }
}
