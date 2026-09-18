// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "srs_allocator_impl.h"
#include "../cell/resource_grid.h"
#include "../config/ue_configuration.h"
#include "ocudu/ran/srs/srs_bandwidth_configuration.h"
#include "ocudu/ran/srs/srs_configuration_helpers.h"

using namespace ocudu;

// Helper to generate an SRS info PDU for a given SRS resource.
static srs_info
create_srs_pdu(rnti_t rnti, const bwp_configuration& ul_bwp_cfg, const srs_config::srs_resource& srs_res_cfg)
{
  srs_info pdu;
  pdu.crnti             = rnti;
  pdu.bwp_cfg           = &ul_bwp_cfg;
  pdu.nof_antenna_ports = static_cast<uint8_t>(srs_res_cfg.nof_ports);
  pdu.symbols           = get_srs_symbol_range(srs_res_cfg, ul_bwp_cfg.cp);
  pdu.nof_repetitions   = srs_res_cfg.res_mapping.rept_factor;

  pdu.config_index         = srs_res_cfg.freq_hop.c_srs;
  pdu.sequence_id          = srs_res_cfg.sequence_id;
  pdu.bw_index             = srs_res_cfg.freq_hop.b_srs;
  pdu.tx_comb              = srs_res_cfg.tx_comb.size;
  pdu.comb_offset          = srs_res_cfg.tx_comb.tx_comb_offset;
  pdu.cyclic_shift         = srs_res_cfg.tx_comb.tx_comb_cyclic_shift;
  pdu.freq_position        = srs_res_cfg.freq_domain_pos;
  pdu.freq_shift           = srs_res_cfg.freq_domain_shift;
  pdu.freq_hopping         = srs_res_cfg.freq_hop.b_hop;
  pdu.group_or_seq_hopping = srs_res_cfg.grp_or_seq_hop;
  pdu.resource_type        = srs_res_cfg.res_type;

  pdu.normalized_channel_iq_matrix_requested = true;

  // NOTE: t_srs_period, t_offset and positioning_report_requested are not set for aperiodic SRS.

  return pdu;
}

static grant_info get_srs_res_grid_grant(const bwp_configuration&        ul_bwp_cfg,
                                         const srs_config::srs_resource& srs_res_cfg)
{
  // NOTE: the validator ensures that B_SRS = 0 (or srs_res_cfg.freq_hop.b_srs == 0); under this assumption, the only
  // possible value for b_SRS is 0 (as per TS 38.211, Section 6.4.1.4.3).
  constexpr uint8_t                      b_srs_0        = 0;
  const unsigned                         nof_ul_bwp_rbs = ul_bwp_cfg.crbs.length();
  const std::optional<srs_configuration> srs_params     = srs_configuration_get(srs_res_cfg.freq_hop.c_srs, b_srs_0);
  ocudu_sanity_check(srs_params.has_value() and nof_ul_bwp_rbs >= srs_params.value().m_srs,
                     "The SRS configuration is not valid");
  const unsigned     starting_crb = (nof_ul_bwp_rbs - srs_params.value().m_srs) / 2;
  const crb_interval srs_crbs{starting_crb, starting_crb + srs_res_cfg.freq_hop.b_srs};
  return {ul_bwp_cfg.scs, get_srs_symbol_range(srs_res_cfg, ul_bwp_cfg.cp), srs_crbs};
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

srs_allocator_impl::srs_allocator_impl(const cell_configuration&      cell_cfg_,
                                       std::optional<srs_periodicity> prohibit_window) :
  cell_cfg(cell_cfg_),
  srs_prohibit_time(prohibit_window.has_value()
                        ? std::optional<unsigned>(static_cast<unsigned>(prohibit_window.value()))
                        : std::nullopt),
  srs_allocation_ring(RES_MANAGER_RING_BUFFER_SIZE, slot_srs_allocation(MAX_NOF_DU_UES))
{
  for (auto& srs_slot_alloc : srs_allocation_ring) {
    srs_slot_alloc.reset();
  }

  if (not cell_cfg.is_tdd()) {
    // With FDD, we consider all the slots can be used for aperiodic SRS; we only use one entry in the bitset.
    srs_slots.resize(1U);
    constexpr unsigned only_fdd_slot_idx = 0U;
    srs_slots.set(only_fdd_slot_idx);
  } else {
    const auto&    tdd_cfg             = *cell_cfg.params.tdd_cfg;
    const unsigned nof_slot_tdd_period = nof_slots_per_tdd_period(tdd_cfg);
    srs_slots.resize(nof_slot_tdd_period);
    srs_slots.reset();

    // TODO: For future extensions, the viable SRS slots could be passed as part of the scheduler configuration.
    // If there are no special slots, we take the first UL as SRS slot.
    // It is assumed that (i.e, pre-validated in the config):
    // - If no special slot is present, we take the first full UL slot.
    // - If pattern 2 has a special slot, it must have the same number of symbols as pattern1 special slot, or 0
    // symbols.
    const std::optional<unsigned> first_ul_slot = find_next_tdd_ul_slot(tdd_cfg, /* start_slot_index */ 0U);
    ocudu_assert(first_ul_slot.has_value(), "At least an UL slot in the TDD configuration is required.");
    ocudu_assert(first_ul_slot.value() < srs_slots.size(), "UL slot exceeded bitset size");
    srs_slots.set(first_ul_slot.value());

    if (tdd_cfg.pattern2.has_value() and tdd_cfg.pattern2.value().nof_ul_symbols != 0) {
      const std::optional<unsigned> second_special_slot =
          find_next_tdd_ul_slot(tdd_cfg, tdd_cfg.pattern1.dl_ul_tx_period_nof_slots);
      ocudu_assert(second_special_slot.has_value(), "At least an UL in the TDD configuration is required.");
      ocudu_assert(second_special_slot.value() < srs_slots.size(), "Pattern 2 UL special slot exceeded bitset size");
      srs_slots.set(second_special_slot.value());
    }
  }
}

/////////////////////          Public functions        ////////////////////////////

void srs_allocator_impl::slot_indication(slot_point slot_tx)
{
  if (not srs_prohibit_time.has_value()) {
    return;
  }

  // If last_sl_ind is not valid (not initialized), then the check sl_tx == last_sl_ind + 1 does not matter.
  ocudu_sanity_check(not last_sl_ind.valid() or slot_tx == last_sl_ind + 1, "Detected a skipped slot");

  // Update Slot.
  last_sl_ind = slot_tx;

  // Clear previous slot SRS usage.
  srs_allocation_ring[(slot_tx - 1).count()].reset();
}

aperiodic_srs_alloc_info srs_allocator_impl::allocate_aperiodic_srs(cell_resource_allocator&     res_alloc,
                                                                    slot_point                   last_srs_slot,
                                                                    const ue_cell_configuration& ue_cfg)
{
  if (not srs_prohibit_time.has_value()) {
    return {};
  }

  const srs_config& srs_cfg     = ue_cfg.init_bwp().ul.ded()->srs_cfg.value();
  const slot_point  pdcch_slot  = res_alloc[0].slot;
  const auto&       srs_res     = srs_cfg.srs_res_list.front();
  const auto&       srs_set     = srs_cfg.srs_res_set_list.front();
  const auto& aperiodic_srs_set = std::get<srs_config::srs_resource_set::aperiodic_resource_type>(srs_set.res_type);
  // NOTE: the presence of the optional slot_offset must have been validated at the moment the cfg is created.
  const unsigned   sl_offset          = aperiodic_srs_set.slot_offset.value();
  const slot_point candidate_srs_slot = pdcch_slot + sl_offset + cell_cfg.ntn_cs_koffset;
  const unsigned   srs_slot_idx       = get_slot_idx(candidate_srs_slot);
  ocudu_assert(srs_slot_idx < srs_slots.size(), "SRS slot index");

  // Check if the SRS can be allocated in the candidate slot.
  if (not srs_slots.test(srs_slot_idx)) {
    return {};
  }

  // We can't transmit if candidate_srs_slot is inside the prohibit window.
  if (last_srs_slot.valid() and candidate_srs_slot < last_srs_slot + srs_prohibit_time.value()) {
    return {};
  }

  auto& srs_slot_res_alloc = res_alloc[sl_offset + cell_cfg.ntn_cs_koffset];
  if (srs_slot_res_alloc.result.ul.srss.full()) {
    return {};
  }

  if (not alloc_srs_resource(candidate_srs_slot, srs_res.id.cell_res_id)) {
    return {};
  }

  // At this point, the SRS allocation has been successful, we proceed with adding the results (SRS PDU and marking the
  // resource grid).
  srs_slot_res_alloc.result.ul.srss.emplace_back(
      create_srs_pdu(ue_cfg.crnti, ue_cfg.cell_cfg_common.params.ul_cfg_common.init_ul_bwp.generic_params, srs_res));
  srs_slot_res_alloc.ul_res_grid.fill(
      get_srs_res_grid_grant(ue_cfg.cell_cfg_common.params.ul_cfg_common.init_ul_bwp.generic_params, srs_res));

  return {.aperiodic_srs_res_trigger = aperiodic_srs_set.aperiodic_srs_res_trigger, .slot_offset = sl_offset};
}

bool srs_allocator_impl::alloc_srs_resource(slot_point srs_slot, unsigned cell_res_id)
{
  auto& slot_alloc = srs_allocation_ring[srs_slot.count()];

  ocudu_assert(cell_res_id < slot_alloc.size(),
               "Cell resource ID {} exceeds the bitset size {}",
               cell_res_id,
               slot_alloc.size());
  if (slot_alloc.test(cell_res_id)) {
    return false;
  }

  slot_alloc.set(cell_res_id);
  return true;
}

unsigned srs_allocator_impl::get_slot_idx(slot_point sl) const
{
  if (not cell_cfg.is_tdd()) {
    return 0U;
  }

  const unsigned tdd_period_slots = nof_slots_per_tdd_period(*cell_cfg.params.tdd_cfg);
  return sl.count() % tdd_period_slots;
}
