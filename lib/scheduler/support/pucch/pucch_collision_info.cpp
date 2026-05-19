// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "pucch_collision_info.h"

using namespace ocudu;

bool pucch_grants::overlaps(const pucch_grants& other) const
{
  // Check if the first grant overlaps with any of the other's grants.
  if (first_hop.overlaps(other.first_hop) or (other.second_hop.has_value() and first_hop.overlaps(*other.second_hop))) {
    return true;
  }
  // Check if the second grant (if any) overlaps with any of the other's grants.
  if (second_hop.has_value() and (second_hop->overlaps(other.first_hop) or
                                  (other.second_hop.has_value() and second_hop->overlaps(*other.second_hop)))) {
    return true;
  }
  return false;
}

/// Constructs resource_info for a common PUCCH resource.
pucch_collision_info::pucch_collision_info(const pucch_default_resource& res,
                                           unsigned                      r_pucch,
                                           const bwp_configuration&      bwp_cfg)
{
  // Compute PRB_first_hop and PRB_second_hop as per Section 9.2.1, TS 38.213.
  const auto prbs =
      get_pucch_default_prb_index(r_pucch, res.rb_bwp_offset, res.cs_indexes.size(), bwp_cfg.crbs.length());

  format             = res.format;
  multiplexing_index = res.cs_indexes[get_pucch_default_cyclic_shift(r_pucch, res.cs_indexes.size())];
  grants             = {
                  .first_hop =
          grant_info(bwp_cfg.scs,
                                 {res.first_symbol_index, res.first_symbol_index + res.nof_symbols / 2},
                     prb_to_crb(bwp_cfg, prb_interval::start_and_len(prbs.first, pucch_constants::f0::NOF_RBS))),
                  .second_hop =
          grant_info(bwp_cfg.scs,
                                 {res.first_symbol_index + res.nof_symbols / 2, res.first_symbol_index + res.nof_symbols},
                     prb_to_crb(bwp_cfg, prb_interval::start_and_len(prbs.second, pucch_constants::f0::NOF_RBS))),
  };
}

/// Constructs resource_info for a dedicated PUCCH resource.
pucch_collision_info::pucch_collision_info(const pucch_resource& res, const bwp_configuration& bwp_cfg)
{
  format = res.format();

  // Compute multiplexing index.
  switch (res.format()) {
    case pucch_format::FORMAT_0: {
      const auto& f0_params = std::get<pucch_resource::f0_config>(res.format_params);
      multiplexing_index    = f0_params.initial_cyclic_shift;
    } break;
    case pucch_format::FORMAT_1: {
      // For PUCCH Format 1, two sequences are orthogonal unless both the initial cyclic shift and the time domain OCC
      // index are the same.
      const auto& f1_params = std::get<pucch_resource::f1_config>(res.format_params);
      multiplexing_index    = f1_params.initial_cyclic_shift + f1_params.time_domain_occ * pucch_constants::f1::NOF_ICS;
    } break;
    case pucch_format::FORMAT_4: {
      // For PUCCH Format 4, the OCC index is mapped to a cyclic shift value, as per Table 6.4.1.3.3.1-1, TS 38.211.
      // Thus, resources with different OCC indices will never collide, even if they have different OCC lengths.
      // Therefore, we can use the OCC index directly as the multiplexing index.
      const auto& f4_params = std::get<pucch_resource::f4_config>(res.format_params);
      multiplexing_index    = static_cast<unsigned>(f4_params.occ_index);
    } break;
    default:
      // Non multiplexed formats.
      multiplexing_index = 0;
      break;
  }

  // Compute time-frequency grants.
  if (res.second_hop_prb.has_value()) {
    // Intra-slot frequency hopping.
    const unsigned half = res.syms.length() / 2;
    grants              = {.first_hop =
                               grant_info{bwp_cfg.scs, {res.syms.start(), res.syms.start() + half}, prb_to_crb(bwp_cfg, res.prbs())},
                           .second_hop = grant_info{
                  bwp_cfg.scs,
                               {res.syms.start() + half, res.syms.stop()},
                  prb_to_crb(bwp_cfg, prb_interval::start_and_len(*res.second_hop_prb, res.prbs().length()))}};
  } else {
    // No intra-slot frequency hopping.
    grants = {
        .first_hop  = grant_info{bwp_cfg.scs, res.syms, prb_to_crb(bwp_cfg, res.prbs())},
        .second_hop = std::nullopt,
    };
  }
}

bool pucch_collision_info::collides(const pucch_collision_info& other) const
{
  if (not grants.overlaps(other.grants)) {
    // Resources that do not overlap in time and frequency do not collide.
    return false;
  }

  if (format != other.format) {
    // Resources with different formats always collide if they overlap in time and frequency.
    return true;
  }

  if (grants != other.grants) {
    // We can only make sure resources have orthogonal sequences if they have the same time/frequency allocation.
    return true;
  }

  // Resources with the same format and time/frequency grants only collide if they have the same multiplexing index.
  // Note: resources with Format 2/3 always collide as they are not multiplexed (multiplexing index is always 0).
  return multiplexing_index == other.multiplexing_index;
}
