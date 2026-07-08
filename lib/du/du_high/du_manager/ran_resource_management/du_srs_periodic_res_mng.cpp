// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "du_srs_periodic_res_mng.h"
#include "du_srs_manager_helpers.h"
#include "du_ue_resource_config.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/ran/srs/srs_bandwidth_configuration.h"
#include "ocudu/scheduler/config/serving_cell_config_factory.h"

using namespace ocudu;
using namespace odu;

static bool is_ul_slot(unsigned offset, const std::optional<tdd_ul_dl_config_common>& tdd_cfg)
{
  if (not tdd_cfg.has_value()) {
    return true;
  }
  const unsigned slot_index = offset % (NOF_SUBFRAMES_PER_FRAME * get_nof_slots_per_subframe(tdd_cfg.value().ref_scs));
  return has_active_tdd_ul_symbols(tdd_cfg.value(), slot_index);
}

static bool is_partially_ul_slot(unsigned offset, const std::optional<tdd_ul_dl_config_common>& tdd_cfg)
{
  if (not tdd_cfg.has_value()) {
    return false;
  }
  const unsigned slot_index = offset % (NOF_SUBFRAMES_PER_FRAME * get_nof_slots_per_subframe(tdd_cfg.value().ref_scs));
  return is_tdd_partial_ul_slot(tdd_cfg.value(), slot_index);
}

du_srs_policy_max_ul_rate::cell_context::cell_context(const du_cell_config& cfg) :
  cell_cfg(cfg),
  tdd_ul_dl_cfg_common(cfg.ran.tdd_cfg),
  default_srs_cfg(config_helpers::make_default_ue_cell_config(cell_cfg.ran)
                      .serv_cell_cfg.ul_config.value()
                      .init_ul_bwp.srs_cfg.value())
{
  ocudu_assert(cfg.ran.init_bwp.srs_cfg.srs_type_enabled != srs_type::aperiodic, "Invalid SRS type");
}

du_srs_policy_max_ul_rate::du_srs_policy_max_ul_rate(span<const du_cell_config> cell_cfg_list_) :
  cells(cell_cfg_list_.begin(), cell_cfg_list_.end())
{
  for (auto& cell : cells) {
    ocudu_assert(cell.cell_cfg.ran.init_bwp.srs_cfg.srs_type_enabled != srs_type::aperiodic,
                 "Request to build aperiodic SRS configuration, but periodic parameters have been provided");

    if (cell.cell_cfg.ran.init_bwp.srs_cfg.srs_type_enabled == srs_type::disabled) {
      continue;
    }

    // If the C_SRS is not set as an input parameter, then we compute C_SRS so that the SRS uses the maximum allowed
    // number of RBs and is located at the center of the RB interval available for the SRS (see below).
    if (cell.cell_cfg.ran.init_bwp.srs_cfg.c_srs.has_value()) {
      cell.srs_common_params.c_srs      = cell.cell_cfg.ran.init_bwp.srs_cfg.c_srs.value();
      cell.srs_common_params.freq_shift = cell.cell_cfg.ran.init_bwp.srs_cfg.freq_domain_shift.value();

      // The user has manually configured the SRS bandwidth; warn if it overlaps with the common PUCCH resources, as
      // this would starve those resources of RBs.
      const crb_interval srs_avail_crbs = du_srs_mng_details::compute_srs_available_crbs(
          cell.cell_cfg.ran.ul_cfg_common.init_ul_bwp.generic_params.crbs,
          cell.cell_cfg.ran.ul_cfg_common.init_ul_bwp.pucch_cfg_common->pucch_resource_common);
      constexpr uint8_t                      b_srs_0    = 0;
      const std::optional<srs_configuration> srs_bw_cfg = srs_configuration_get(cell.srs_common_params.c_srs, b_srs_0);
      ocudu_assert(srs_bw_cfg.has_value(), "Invalid SRS bandwidth configuration");
      const crb_interval srs_crbs{cell.srs_common_params.freq_shift,
                                  cell.srs_common_params.freq_shift + srs_bw_cfg.value().m_srs};
      if (not srs_avail_crbs.contains(srs_crbs)) {
        ocudulog::fetch_basic_logger("DU-MNG").warning(
            "The manually configured SRS bandwidth crbs={} overlaps with the common PUCCH resources; this may "
            "starve the common PUCCH resources of RBs. Consider restricting the SRS to crbs={}",
            srs_crbs,
            srs_avail_crbs);
      }
    } else {
      // Restrict the SRS bandwidth to the RBs in between the 2 common PUCCH resource blocks, so that the SRS
      // doesn't starve the common PUCCH resources of RBs.
      const crb_interval srs_avail_crbs = du_srs_mng_details::compute_srs_available_crbs(
          cell.cell_cfg.ran.ul_cfg_common.init_ul_bwp.generic_params.crbs,
          cell.cell_cfg.ran.ul_cfg_common.init_ul_bwp.pucch_cfg_common->pucch_resource_common);
      // \c compute_c_srs() picks the C_SRS whose corresponding \f$m_{SRS,0}\f$ (i.e., the SRS bandwidth in RBs) is
      // the largest value that still fits within \c srs_avail_crbs, instead of the whole UL BWP; this is what caps
      // the SRS bandwidth to the gap between the 2 common PUCCH resource blocks.
      const std::optional<unsigned> c_srs = du_srs_mng_details::compute_c_srs(srs_avail_crbs.length());
      ocudu_assert(c_srs.has_value(), "SRS parameters didn't provide a valid C_SRS value");
      cell.srs_common_params.c_srs = c_srs.value();
      // When computed automatically, \c freqDomainShift is set so that the SRS is placed at the center of the
      // available RB interval. As per TS 38.211, Section 6.4.1.4.3, if \f$n_{shift} >= BWP_RB_start\f$, the reference
      // point for the SRS subcarriers is the CRB idx 0, else it's the BWP_RB_start; in here, we implicitly assume
      // \f$n_{shift} >= BWP_RB_start\f$.
      cell.srs_common_params.freq_shift =
          du_srs_mng_details::compute_srs_rb_start(c_srs.value(), srs_avail_crbs.length()) + srs_avail_crbs.start();
    }

    cell.srs_common_params.p0 = cell.cell_cfg.ran.init_bwp.srs_cfg.p0;

    // TODO: evaluate whether we need to consider the case of multiple cells.
    cell.cell_srs_res_list = generate_cell_srs_list(cell.cell_cfg);

    const auto srs_period_slots = static_cast<unsigned>(cell.cell_cfg.ran.init_bwp.srs_cfg.srs_period_prohib_time);
    // Reserve the size of the vector and set the SRS counter of each offset to 0.
    cell.slot_resource_cnt.reserve(srs_period_slots);
    cell.slot_resource_cnt.assign(srs_period_slots, 0U);
    cell.srs_res_offset_free_list.reserve(MAX_NOF_CELL_SRS_RES);
    cell.nof_res_per_symb_interval =
        static_cast<unsigned>(cell.cell_cfg.ran.init_bwp.srs_cfg.tx_comb) *
        static_cast<unsigned>(cell.cell_cfg.ran.init_bwp.srs_cfg.cyclic_shift_reuse_factor) *
        static_cast<unsigned>(cell.cell_cfg.ran.init_bwp.srs_cfg.sequence_id_reuse_factor);

    for (unsigned offset = 0; offset != srs_period_slots; ++offset) {
      // We don't generate more than the maximum number of SRS resources per cell.
      if (cell.srs_res_offset_free_list.size() >= MAX_NOF_CELL_SRS_RES) {
        break;
      }

      // Verify whether the offset maps to a partially- or fully-UL slot.
      if (not is_ul_slot(offset, cell.tdd_ul_dl_cfg_common)) {
        continue;
      }

      for (auto& res : cell.cell_srs_res_list) {
        // Handle TDD and FDD configurations separately, as we treat partially-UL slots differently from
        // fully-UL slots.
        if (is_partially_ul_slot(offset, cell.tdd_ul_dl_cfg_common)) {
          // For partially-UL slots, we need to check if the SRS can be placed in the UL symbols of the slot.
          const auto&    tdd_cfg    = cell.tdd_ul_dl_cfg_common.value();
          const unsigned slot_index = offset % (NOF_SUBFRAMES_PER_FRAME * get_nof_slots_per_subframe(tdd_cfg.ref_scs));
          // As per Section 6.4.1.4.1, TS 38.211, the SRS resources can only be placed in the last 6 symbols of the
          // slot.
          static constexpr unsigned max_srs_symbols = 6U;
          const unsigned            nof_ul_symbols_for_srs =
              std::min(get_active_tdd_ul_symbols(tdd_cfg, slot_index, cyclic_prefix::NORMAL).length(), max_srs_symbols);
          if (res.symbols.start() < NOF_OFDM_SYM_PER_SLOT_NORMAL_CP - nof_ul_symbols_for_srs) {
            continue;
          }
        }
        // NOTE: for TDD, the offset that are not UL slots are skipped above.
        // FDD case and TDD case for fully-UL slot.
        else if (res.symbols.start() <
                 NOF_OFDM_SYM_PER_SLOT_NORMAL_CP - cell.cell_cfg.ran.init_bwp.srs_cfg.max_nof_symbols.value()) {
          continue;
        }
        cell.srs_res_offset_free_list.emplace_back(res.cell_res_id, offset);
      }
    }
  }
}

bool du_srs_policy_max_ul_rate::alloc_resources(cell_group_config& cell_grp_cfg)
{
  // First, check there are available SRS resources in each UE's cell.
  bool alloc_succeeded = true;
  for (auto cell_cfg_ded_entry : cell_grp_cfg.cells) {
    auto&       cell_cfg_ded = cell_cfg_ded_entry.second.serv_cell_cfg;
    const auto& ue_du_cell   = cells[cell_cfg_ded.cell_index];

    ocudu_assert(cell_cfg_ded.ul_config.has_value() and not cell_cfg_ded.ul_config->init_ul_bwp.srs_cfg.has_value(),
                 "UE UL config should be non-empty but with an empty SRS config");

    cell_cfg_ded.ul_config->init_ul_bwp.srs_cfg.emplace(ue_du_cell.default_srs_cfg);

    // If periodic SRS is not enabled on this cell, don't allocate anything and continue to the next cell.
    if (ue_du_cell.cell_cfg.ran.init_bwp.srs_cfg.srs_type_enabled == srs_type::disabled) {
      continue;
    }

    const auto& free_srs_list = ue_du_cell.srs_res_offset_free_list;
    // Verify whether there are SRS resources to allocate a new UE.
    if (free_srs_list.empty()) {
      // If the allocation failed, reset the SRS configuration.
      cell_cfg_ded.ul_config->init_ul_bwp.srs_cfg.reset();
      alloc_succeeded = false;
      break;
    }
  }

  // Reset the UE configuration in each cell before existing with failure.
  if (not alloc_succeeded) {
    for (auto cell_cfg_ded_entry : cell_grp_cfg.cells) {
      auto& cell_cfg_ded = cell_cfg_ded_entry.second.serv_cell_cfg;
      cell_cfg_ded.ul_config->init_ul_bwp.srs_cfg.reset();
    }
    return false;
  }

  // From this point on, the allocation is expected to succeed, as there are SRS resources available in each cell.
  for (auto cell_cfg_ded_entry : cell_grp_cfg.cells) {
    auto& ue_cell_cfg = cell_cfg_ded_entry.second.serv_cell_cfg;
    auto& ue_du_cell  = cells[ue_cell_cfg.cell_index];

    // The UE SRS configuration is taken from a base configuration, saved in the GNB. The UE specific parameters will be
    // added later on in this function.
    // NOTE: This config could be as well for non-periodic SRS, therefore emplace anyway.
    ue_cell_cfg.ul_config->init_ul_bwp.srs_cfg.emplace(ue_du_cell.default_srs_cfg);

    // If periodic SRS is not enabled on this cell, don't allocate anything and continue to the next cell.
    if (ue_du_cell.cell_cfg.ran.init_bwp.srs_cfg.srs_type_enabled == srs_type::disabled) {
      continue;
    }

    srs_config& ue_srs_cfg    = ue_cell_cfg.ul_config->init_ul_bwp.srs_cfg.value();
    auto&       free_srs_list = ue_du_cell.srs_res_offset_free_list;

    // Find the best resource ID and offset for this UE, according to the class policy.
    auto srs_res_id_offset = ue_du_cell.find_optimal_ue_srs_resource();
    ocudu_assert(srs_res_id_offset != free_srs_list.end(), "No SRS resource returned from a non-emtpy set");

    const auto& du_res_it = ue_du_cell.get_du_srs_res_cfg(srs_res_id_offset->first);
    ocudu_assert(du_res_it != ue_du_cell.cell_srs_res_list.end(), "The provided cell-ID is invalid");

    const auto& du_res = *du_res_it;

    // Update the SRS configuration with the parameters that are specific to this resource and for this UE.
    auto&          only_ue_srs_res = ue_srs_cfg.srs_res_list.front();
    const unsigned srs_offset      = srs_res_id_offset->second;
    // NOTE: given that there is only 1 SRS resource per UE, we can assume that the SRS resource ID is 0.
    only_ue_srs_res.id.cell_res_id = du_res.cell_res_id;
    only_ue_srs_res.id.ue_res_id   = static_cast<srs_config::srs_res_id>(0U);
    ocudu_assert(ue_du_cell.cell_cfg.ran.ul_carrier.nof_ant == 1 or ue_du_cell.cell_cfg.ran.ul_carrier.nof_ant == 2 or
                     ue_du_cell.cell_cfg.ran.ul_carrier.nof_ant == 4,
                 "The number of UL antenna ports is not valid");
    only_ue_srs_res.nof_ports                    = srs_config::srs_resource::nof_srs_ports::port1;
    only_ue_srs_res.tx_comb.size                 = ue_du_cell.cell_cfg.ran.init_bwp.srs_cfg.tx_comb;
    only_ue_srs_res.tx_comb.tx_comb_offset       = du_res.tx_comb_offset.value();
    only_ue_srs_res.tx_comb.tx_comb_cyclic_shift = du_res.cs;
    only_ue_srs_res.freq_domain_pos              = du_res.freq_dom_position;
    only_ue_srs_res.res_mapping.start_pos        = NOF_OFDM_SYM_PER_SLOT_NORMAL_CP - du_res.symbols.start() - 1;
    only_ue_srs_res.res_mapping.nof_symb         = static_cast<srs_nof_symbols>(du_res.symbols.length());
    only_ue_srs_res.sequence_id                  = du_res.sequence_id;

    // Update the SRS configuration with the parameters that are common to the cell.
    only_ue_srs_res.freq_hop.c_srs = ue_du_cell.srs_common_params.c_srs;
    // We assume that the frequency hopping is disabled and that the SRS occupies all possible RBs within the BWP. Refer
    // to Section 6.4.1.4.3, TS 38.211.
    only_ue_srs_res.freq_hop.b_srs    = 0U;
    only_ue_srs_res.freq_hop.b_hop    = 0U;
    only_ue_srs_res.freq_domain_shift = ue_du_cell.srs_common_params.freq_shift;

    only_ue_srs_res.periodicity_and_offset.emplace(srs_config::srs_periodicity_and_offset{
        .period = ue_du_cell.cell_cfg.ran.init_bwp.srs_cfg.srs_period_prohib_time,
        .offset = static_cast<uint16_t>(srs_offset)});

    // Update the SRS resource set with the SRS id.
    ue_srs_cfg.srs_res_set_list.front().srs_res_id_list.front() = only_ue_srs_res.id.ue_res_id;

    // Update the SRS resource set with the p0.
    ue_srs_cfg.srs_res_set_list.front().p0 = ue_du_cell.srs_common_params.p0;

    // Remove the allocated SRS resource from the free list.
    free_srs_list.erase(srs_res_id_offset);

    // Update the SRS resource per slot counter.
    ++ue_du_cell.slot_resource_cnt[srs_offset];
  }

  return true;
}

std::vector<du_srs_policy_max_ul_rate::cell_context::pair_res_id_offset>::const_iterator
du_srs_policy_max_ul_rate::cell_context::find_optimal_ue_srs_resource()
{
  // The weights assigned here can be set to arbitrarily value, as long as:
  // - symbol_weight_base is greater than 0;
  // - reuse_slot_discount is less than symbol_weight_base;
  // - max_weight > symbol_weight_base * (srs_builder_params::max_nof_symbols / srs_builder_params::nof_symbols).
  static constexpr unsigned max_weight         = 100U;
  static constexpr unsigned symbol_weight_base = 10U;
  // We give a discount to the symbol weight if the offset is already used but not full.
  static constexpr unsigned partial_symb_interval_discount = symbol_weight_base / 2U;

  const auto weight_function = [&](const pair_res_id_offset& srs_res) {
    if (cell_cfg.ran.tdd_cfg.has_value() and is_partially_ul_slot(srs_res.second, cell_cfg.ran.tdd_cfg.value())) {
      return 0U;
    }

    // SRS res config.
    const auto srs_res_cfg_it = get_du_srs_res_cfg(srs_res.first);

    if (srs_res_cfg_it == cell_srs_res_list.end()) {
      return max_weight;
    }

    // Give priority to the last symbol intervals within a slot. This reduces the space used for the SRS in a slot.
    const unsigned symb_weight =
        symbol_weight_base *
        ((NOF_OFDM_SYM_PER_SLOT_NORMAL_CP - srs_res_cfg_it->symbols.start()) / srs_res_cfg_it->symbols.length());

    // We consider a discount if the offset is already used but not full; this way, we give an incentive
    // to the SRS resources not to be allocated on a new slot, to avoid taking PUSCH symbols on a new
    // slot.
    const unsigned reuse_slot_discount =
        offset_interval_used_not_full(srs_res.second) ? partial_symb_interval_discount : 0U;

    return symb_weight - reuse_slot_discount;
  };

  auto optimal_res_it =
      std::min_element(srs_res_offset_free_list.begin(),
                       srs_res_offset_free_list.end(),
                       [&weight_function](const pair_res_id_offset& lhs, const pair_res_id_offset& rhs) {
                         return weight_function(lhs) < weight_function(rhs);
                       });

  return optimal_res_it;
}

void du_srs_policy_max_ul_rate::dealloc_resources(cell_group_config& cell_grp_cfg)
{
  for (auto cell_cfg_ded_entry : cell_grp_cfg.cells) {
    auto& cell_cfg_ded = cell_cfg_ded_entry.second.serv_cell_cfg;
    // This is the cell index inside the DU.
    auto& ue_du_cell = cells[cell_cfg_ded.cell_index];

    ocudu_assert(ue_du_cell.cell_cfg.ran.init_bwp.srs_cfg.srs_type_enabled != srs_type::aperiodic,
                 "This function is not compatible with aperiodic SRS");

    if (ue_du_cell.cell_cfg.ran.init_bwp.srs_cfg.srs_type_enabled != srs_type::periodic or
        not cell_cfg_ded.ul_config->init_ul_bwp.srs_cfg.has_value()) {
      continue;
    }

    const auto& ue_srs_cfg    = cell_cfg_ded.ul_config->init_ul_bwp.srs_cfg.value();
    auto&       free_srs_list = ue_du_cell.srs_res_offset_free_list;

    for (const auto& srs_res : ue_srs_cfg.srs_res_list) {
      const unsigned offset_to_deallocate = srs_res.periodicity_and_offset.value().offset;
      free_srs_list.emplace_back(srs_res.id.cell_res_id, offset_to_deallocate);

      // Update the used_not_full slot vector.gnb
      ocudu_assert(ue_du_cell.slot_resource_cnt[offset_to_deallocate] != 0,
                   "The slot resource counter is expected to be non-zero");
      --ue_du_cell.slot_resource_cnt[offset_to_deallocate];
    }

    // Reset the SRS configuration in this UE. This makes sure the DU will exit this function immediately when it gets
    // called again for the same UE (upon destructor's call).
    cell_cfg_ded.ul_config->init_ul_bwp.srs_cfg.reset();
  }
}
