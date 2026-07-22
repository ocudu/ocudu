// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "srs_resource_manager_aperiodic.h"
#include "srs_resource_generator.h"
#include "srs_resource_manager_helpers.h"
#include "ocudu/ran/srs/srs_configuration.h"
#include "ocudu/ran/srs/srs_constants.h"
#include "ocudu/scheduler/config/pusch_td_resource_indices.h"
#include "ocudu/scheduler/config/serving_cell_config_factory.h"

using namespace ocudu;

// Helper that computes the slot offset that can be used for the activation of the SRS resource sets (ref. to
// Section 6.2.1, TS 38.214). As per TS 38.214, 6.1.1.1, only 1 SRS Resource Set can be configured for Codebook based
// transmissions. This means that only 1 offset can be chosen.
static unsigned compute_slot_offset(const ran_cell_config& cell_cfg)
{
  const auto& pusch_td_alloc_list = cell_cfg.ul_cfg_common.init_ul_bwp.pusch_cfg_common->pusch_td_alloc_list;

  std::vector<static_vector<uint8_t, pusch_constants::MAX_NOF_PUSCH_TD_RES_ALLOCS>> pusch_td_list_per_slot =
      get_pusch_td_resource_indices_per_slot(cell_cfg.dl_cfg_common.init_dl_bwp.generic_params.scs,
                                             cell_cfg.tdd_cfg,
                                             pusch_td_alloc_list,
                                             cell_cfg.init_bwp.pucch.min_k1);

  // Find the maximum k2 used for UL scheduling.
  unsigned max_used_k2 = 0;
  for (const auto& dl_td_res_vec : pusch_td_list_per_slot) {
    for (const auto td_res_idx : dl_td_res_vec) {
      max_used_k2 = std::max(max_used_k2, static_cast<unsigned>(pusch_td_alloc_list[td_res_idx].k2));
    }
  }

  // [Implementation-defined] Take the max between min_k1 and k2; this way we ensure that:
  // - The slot offset is greater than the min_k1; if we assume the UE's SRS latency requirement is the same as for the
  // PUCCH one, a slot offset > min_k1 ensures the DCI-to-SRS latency constraint is met. Note, the TS sets some min
  // constraints for DCI-to-SRS (as per Section 6.2.1, TS 38.214); however, some test tools work better with looser
  // latency constraints.
  // - If at a given PDCCH slot we trigger an SRS with a given slot_offset, there won't be any PUSCH yet allocated at
  // PDCCH slot + slot_offset.
  const unsigned min_k1                  = cell_cfg.init_bwp.pucch.min_k1;
  const int      slot_offset_lower_bound = static_cast<int>(std::max(min_k1, max_used_k2));

  // FDD Case.
  if (not cell_cfg.tdd_cfg.has_value()) {
    // Return 1 slot offsets value, to ensure that the SRS is allocated before the PUSCH.
    return static_cast<unsigned>(slot_offset_lower_bound + 1);
  }

  const auto& tdd_cfg = cell_cfg.tdd_cfg.value();

  // TDD Case.

  // Get the target UL slot indices, i.e., the slots in which we aim at allocating the SRS. If present, we consider the
  // special slots, else the first UL slot.
  std::vector<unsigned> target_ul_slots_idx;
  if (tdd_cfg.pattern1.nof_ul_symbols != 0) {
    target_ul_slots_idx.emplace_back(tdd_cfg.pattern1.nof_dl_slots);
  }
  if (tdd_cfg.pattern2.has_value() and tdd_cfg.pattern2.value().nof_ul_slots != 0 and
      tdd_cfg.pattern2.value().nof_dl_symbols != 0) {
    target_ul_slots_idx.emplace_back(tdd_cfg.pattern1.dl_ul_tx_period_nof_slots +
                                     tdd_cfg.pattern2.value().nof_dl_slots);
  }
  if (target_ul_slots_idx.empty()) {
    target_ul_slots_idx.emplace_back(tdd_cfg.pattern1.dl_ul_tx_period_nof_slots - tdd_cfg.pattern1.nof_ul_slots);
  }

  const unsigned tdd_period_slots = nof_slots_per_tdd_period(tdd_cfg);

  // Find the candidate UL slot offsets, i.e., the slot offsets that can be used to trigger aperiodic SRS with UL DCIs.
  // These UL slot offsets only exist for DL slots that has at least a k2 value (otherwise, the DL slot can't be used
  // for UL DCIs).
  std::vector<unsigned> candidate_slot_offsets;
  for (unsigned dl_sl_idx = 0, sz = pusch_td_list_per_slot.size(); dl_sl_idx != sz; ++dl_sl_idx) {
    // Skip UL slots.
    if (pusch_td_list_per_slot[dl_sl_idx].empty()) {
      continue;
    }
    // Save the viable offsets that map the DL slots suitable for UL DCI to the candidate SRS UL slots.
    for (const auto target_slot_idx : target_ul_slots_idx) {
      // Consider the offsets that spans over several TDD periods, otherwise the constraint
      // sl_offset > slot_offset_lower_bound might not be met.
      // The maximum period multiplier 3 is the for a 4-slot TDD period to cover the worst case for the constraint
      // sl_offset > SCHEDULER_MAX_K2.
      for (unsigned period_multiplier = 0; period_multiplier != 4; ++period_multiplier) {
        const auto sl_offset =
            static_cast<int>(target_slot_idx + period_multiplier * tdd_period_slots) - static_cast<int>(dl_sl_idx);
        // The constraint offsets > slot_offset_lower_bound ensures the SRS is always allocated before a PUSCH.
        if (sl_offset > slot_offset_lower_bound and sl_offset <= static_cast<int>(srs_constants::MAX_SRS_SLOT_OFFSET)) {
          candidate_slot_offsets.emplace_back(sl_offset);
        }
      }
    }
  }

  ocudu_assert(not candidate_slot_offsets.empty(),
               "The candidate slot offsets list cannot be empty: check if the pusch_td_list_per_slot has been "
               "correctly created");

  // Return the min offset.
  const auto min_it = std::min_element(candidate_slot_offsets.begin(),
                                       candidate_slot_offsets.end(),
                                       [](const unsigned lhs, const unsigned rhs) { return lhs < rhs; });
  ocudu_assert(min_it != candidate_slot_offsets.end(), "A min is expected from a non-empty set");

  return *min_it;
}

std::vector<du_srs_resource>::const_iterator
srs_resource_manager_aperiodic::cell_context::get_du_srs_res_cfg(unsigned cell_res_id)
{
  if (cell_res_id >= cell_srs_res_list.size() or cell_srs_res_list[cell_res_id].cell_res_id != cell_res_id) {
    ocudu_assertion_failure("Cell resource ID out of range or invalid");
    return cell_srs_res_list.end();
  }
  return cell_srs_res_list.cbegin() + cell_res_id;
}

srs_resource_manager_aperiodic::cell_context::cell_context(const ran_cell_config& cfg) :
  cell_cfg(cfg),
  tdd_ul_dl_cfg_common(cfg.tdd_cfg),
  default_srs_cfg(
      config_helpers::make_default_ue_cell_config(cell_cfg).serv_cell_cfg.ul_config.value().init_ul_bwp.srs_cfg.value())
{
  ocudu_assert(cfg.init_bwp.srs_cfg.srs_type_enabled != srs_type::periodic, "Invalid SRS type");
}

void srs_resource_manager_aperiodic::add_cell(du_cell_index_t cell_idx, const ran_cell_config& cell_cfg)
{
  ocudu_assert(not cells.contains(cell_idx), "Cell index={} already configured", cell_idx);

  cell_context cell_ctx(cell_cfg);

  ocudu_assert(cell_ctx.cell_cfg.init_bwp.srs_cfg.srs_type_enabled != srs_type::periodic,
               "Request to build aperiodic SRS configuration, but periodic parameters have been provided");

  if (cell_ctx.cell_cfg.init_bwp.srs_cfg.srs_type_enabled == srs_type::disabled) {
    cells.emplace(cell_idx, std::move(cell_ctx));
    return;
  }

  const auto& srs_cfg = cell_ctx.cell_cfg.init_bwp.srs_cfg;

  // If the C_SRS is not set as an input parameter, then we compute C_SRS so that the SRS uses the maximum allowed
  // number of RBs and is located at the center of the UL BWP.
  if (srs_cfg.c_srs.has_value()) {
    cell_ctx.srs_common_params.c_srs      = srs_cfg.c_srs.value();
    cell_ctx.srs_common_params.freq_shift = srs_cfg.freq_domain_shift.value();
  } else {
    const std::optional<unsigned> c_srs =
        srs_res_mng_details::compute_c_srs(cell_ctx.cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.crbs.length());
    ocudu_assert(c_srs.has_value(), "SRS parameters didn't provide a valid C_SRS value");
    cell_ctx.srs_common_params.c_srs = c_srs.value();
    // When computed automatically, \c freqDomainShift is set so that the SRS is placed at the center of the UL BWP.
    // As per TS 38.211, Section 6.4.1.4.3, if \f$n_{shift} >= BWP_RB_start\f$, the reference point for the SRS
    // subcarriers is the CRB idx 0, else it's the BWP_RB_start; in here, we implicitly assume \f$n_{shift} >=
    // BWP_RB_start\f$.
    cell_ctx.srs_common_params.freq_shift =
        srs_res_mng_details::compute_srs_rb_start(
            c_srs.value(), cell_ctx.cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.crbs.length()) +
        cell_ctx.cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.crbs.start();
  }

  cell_ctx.srs_common_params.p0 = srs_cfg.p0;

  // NOTE: If there is pattern2, then we expect pattern 2 to have the same number of symbols in the special slot as
  // pattern1.
  const bool use_special_slot_only =
      cell_ctx.cell_cfg.tdd_cfg.has_value() and (cell_ctx.cell_cfg.tdd_cfg.value().pattern1.nof_ul_symbols != 0);
  cell_ctx.cell_srs_res_list = generate_cell_srs_list(cell_ctx.cell_cfg, use_special_slot_only);

  // Reserve the size of the vector and set the SRS counter of each resource to 0.
  cell_ctx.srs_res_usage.reserve(cell_ctx.cell_srs_res_list.size());
  cell_ctx.srs_res_usage.assign(cell_ctx.cell_srs_res_list.size(), 0U);

  ocudu_assert(cell_ctx.cell_cfg.ul_cfg_common.init_ul_bwp.pusch_cfg_common.has_value(),
               "The SRS aperiodic configuration generation requires PUSCH Config Common and PUCCH parameters");

  cell_ctx.slot_offset = compute_slot_offset(cell_ctx.cell_cfg);

  cells.emplace(cell_idx, std::move(cell_ctx));
}

bool srs_resource_manager_aperiodic::alloc_resources(ue_cell_config& ue_cell_cfg)
{
  auto& cell_cfg_ded = ue_cell_cfg.serv_cell_cfg;
  auto& ue_du_cell   = cells[cell_cfg_ded.cell_index];

  // The UE SRS configuration is taken from a base configuration, saved in the GNB. The UE specific parameters will be
  // added later on in this function.
  cell_cfg_ded.ul_config->init_ul_bwp.srs_cfg.emplace(ue_du_cell.default_srs_cfg);

  srs_config& ue_srs_cfg = cell_cfg_ded.ul_config->init_ul_bwp.srs_cfg.value();

  // Find the best resource ID this UE, according to the class policy.
  const auto opt_srs_res_it = std::min_element(ue_du_cell.srs_res_usage.begin(),
                                               ue_du_cell.srs_res_usage.end(),
                                               [](const unsigned lhs, const unsigned rhs) { return lhs < rhs; });
  ocudu_assert(opt_srs_res_it != ue_du_cell.srs_res_usage.end(), "No SRS resource returned from a non-emtpy set");

  auto opt_res_idx = std::distance(ue_du_cell.srs_res_usage.begin(), opt_srs_res_it);

  const auto& du_res_it = ue_du_cell.get_du_srs_res_cfg(static_cast<unsigned>(opt_res_idx));
  ocudu_assert(du_res_it != ue_du_cell.cell_srs_res_list.end(), "The provided cell-ID is invalid");
  const auto& du_res = *du_res_it;

  // Update the SRS configuration with the parameters that are specific to this resource and for this UE.
  auto& only_ue_srs_res = ue_srs_cfg.srs_res_list.front();
  ue_du_cell.fill_srs_res_parameters(only_ue_srs_res, du_res);
  ue_du_cell.fill_srs_res_set(ue_srs_cfg.srs_res_set_list, only_ue_srs_res.id.ue_res_id);

  // Update the counter of UEs using this resource.
  ++ue_du_cell.srs_res_usage[opt_res_idx];

  return true;
}

void srs_resource_manager_aperiodic::cell_context::fill_srs_res_parameters(srs_config::srs_resource& res_out,
                                                                           const du_srs_resource&    res_in) const
{
  // NOTE: given that there is only 1 SRS resource per UE, we can assume that the SRS resource ID is 0.
  res_out.id.cell_res_id = res_in.cell_res_id;
  res_out.id.ue_res_id   = static_cast<srs_config::srs_res_id>(0U);
  ocudu_assert(cell_cfg.ul_carrier.nof_ant == 1 or cell_cfg.ul_carrier.nof_ant == 2 or cell_cfg.ul_carrier.nof_ant == 4,
               "The number of UL antenna ports is not valid");
  res_out.nof_ports                    = srs_config::srs_resource::nof_srs_ports::port1;
  res_out.tx_comb.size                 = cell_cfg.init_bwp.srs_cfg.tx_comb;
  res_out.tx_comb.tx_comb_offset       = res_in.tx_comb_offset.value();
  res_out.tx_comb.tx_comb_cyclic_shift = res_in.cs;
  res_out.freq_domain_pos              = res_in.freq_dom_position;
  res_out.res_mapping.start_pos        = NOF_OFDM_SYM_PER_SLOT_NORMAL_CP - res_in.symbols.start() - 1;
  res_out.res_mapping.nof_symb         = static_cast<srs_nof_symbols>(res_in.symbols.length());
  res_out.sequence_id                  = res_in.sequence_id;

  // Update the SRS configuration with the parameters that are common to the cell.
  res_out.freq_hop.c_srs = srs_common_params.c_srs;
  // We assume that the frequency hopping is disabled. Refer to Section 6.4.1.4.3, TS 38.211.
  res_out.freq_hop.b_srs    = 0U;
  res_out.freq_hop.b_hop    = 0U;
  res_out.freq_domain_shift = srs_common_params.freq_shift;
}

void srs_resource_manager_aperiodic::cell_context::fill_srs_res_set(srs_set_t&             srs_res_set_list,
                                                                    srs_config::srs_res_id res_id) const
{
  // Update the parameters.
  auto& srs_res_set = srs_res_set_list.front();
  srs_res_set.p0    = srs_common_params.p0;

  srs_res_set.id = static_cast<srs_config::srs_res_set_id>(0);
  // [Implementation-defined] Only 1 SRS resource per set.
  srs_res_set.srs_res_id_list.front() = res_id;
  auto& aperiodic_set_type = std::get<srs_config::srs_resource_set::aperiodic_resource_type>(srs_res_set.res_type);
  aperiodic_set_type.slot_offset.emplace(slot_offset);
  // We use the following map to activate the SRS sets (ref to Table 7.3.1.1.2-24, TS 38.212).
  // SRS resource set 0 -> aperiodic_srs_res_trigger 1.
  aperiodic_set_type.aperiodic_srs_res_trigger = static_cast<uint8_t>(srs_res_set.id) + 1U;
}

void srs_resource_manager_aperiodic::dealloc_resources(ue_cell_config& ue_cell_cfg)
{
  auto& cell_cfg_ded = ue_cell_cfg.serv_cell_cfg;
  // This is the cell index inside the DU.
  auto& ue_du_cell = cells[cell_cfg_ded.cell_index];

  if (not cell_cfg_ded.ul_config->init_ul_bwp.srs_cfg.has_value()) {
    return;
  }

  const auto& ue_srs_cfg = cell_cfg_ded.ul_config->init_ul_bwp.srs_cfg.value();

  for (const auto& srs_res : ue_srs_cfg.srs_res_list) {
    const unsigned res_id_to_deallocate = srs_res.id.cell_res_id;

    ocudu_assert(res_id_to_deallocate < ue_du_cell.srs_res_usage.size(),
                 "The slot resource counter is expected to be non-zero");
    // Update the used_not_full slot vector.
    ocudu_assert(ue_du_cell.srs_res_usage[res_id_to_deallocate] != 0,
                 "The slot resource counter is expected to be non-zero");
    --ue_du_cell.srs_res_usage[res_id_to_deallocate];
  }

  // Reset the SRS configuration in this UE. This makes sure the DU will exit this function immediately when it gets
  // called again for the same UE (upon destructor's call).
  cell_cfg_ded.ul_config->init_ul_bwp.srs_cfg.reset();
}
