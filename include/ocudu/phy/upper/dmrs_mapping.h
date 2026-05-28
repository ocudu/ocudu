// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief DM-RS related utility functions for PDSCH and PUSCH.

#pragma once

#include "ocudu/phy/support/re_pattern.h"
#include "ocudu/ran/dmrs/dmrs.h"

namespace ocudu {

/// Maximum number of ports for DM-RS type 1.
static constexpr unsigned DMRS_MAX_PORTS_TYPE1 = 8;

/// Maximum number of ports for DM-RS type 2.
static constexpr unsigned DMRS_MAX_PORTS_TYPE2 = 12;

/// Returns the maximum number of ports supported by the given DM-RS type.
constexpr unsigned dmrs_max_ports_type(dmrs_config_type type)
{
  ocudu_assert(type != dmrs_config_type::not_set, "Invalid DM-RS type.");
  return (type == dmrs_config_type::type1) ? DMRS_MAX_PORTS_TYPE1 : DMRS_MAX_PORTS_TYPE2;
}

/// Defines the maximum number of ports the DM-RS can be mapped to.
constexpr unsigned DMRS_MAX_NPORTS =
    std::max(dmrs_max_ports_type(dmrs_config_type::type1), dmrs_max_ports_type(dmrs_config_type::type2));

/// \brief Gets a DM-RS position mask within a resource block.
///
/// \param[in] type                        DM-RS type.
/// \param[in] nof_cdm_groups_without_data Number of CDM groups without data.
/// \return A boolen mask of the REs within a PRB that contain DM-RS.
inline re_prb_mask get_dmrs_prb_mask(dmrs_config_type type, unsigned nof_cdm_groups_without_data)
{
  ocudu_assert(type != dmrs_config_type::not_set, "Invalid DM-RS type.");

  if (type == dmrs_config_type::type1) {
    bounded_bitset<2> cdm_groups_pattern(2);
    cdm_groups_pattern.fill(0, nof_cdm_groups_without_data);
    bounded_bitset<6> groups(6);
    groups.fill(0, 6, true);
    return groups.kronecker_product<2>(cdm_groups_pattern);
  }

  bounded_bitset<6> cdm_groups_pattern(6);
  cdm_groups_pattern.fill(0, 2 * nof_cdm_groups_without_data);
  bounded_bitset<2> groups(2);
  groups.fill(0, 2, true);
  return groups.kronecker_product<6>(cdm_groups_pattern);
}

/// \brief Gets a DM-RS transmission pattern.
///
/// It creates an RE pattern that describes the reserved resource elements for DM-RS in PDSCH or PUSCH transmissions.
///
/// \param[in] type                        DM-RS type.
/// \param[in] bwp_start_rb                Indicates the start of the BWP.
/// \param[in] bwp_size_rb                 Indicates the size of the BWP.
/// \param[in] nof_cdm_groups_without_data Indicates the number of CDM groups without data for the transmission.
/// \param[in] symbol_mask                 Indicates, with a mask, the symbols carrying DM-RS.
/// \return An RE pattern describing the reserved elements for DMRS.
inline re_pattern get_dmrs_pattern(dmrs_config_type        type,
                                   unsigned                bwp_start_rb,
                                   unsigned                bwp_size_rb,
                                   unsigned                nof_cdm_groups_without_data,
                                   const symbol_slot_mask& symbol_mask)
{
  ocudu_assert(type != dmrs_config_type::not_set, "Invalid DM-RS type.");

  // Temporal pattern.
  re_pattern dmrs_pattern;

  // Validate symbol mask size.
  ocudu_assert(symbol_mask.size() <= dmrs_pattern.symbols.size(),
               "The symbol mask size {} exceeds the maximum size {}.",
               symbol_mask.size(),
               dmrs_pattern.symbols.size());

  // Generate DMRS pattern.
  dmrs_pattern.crb_mask.resize(bwp_start_rb + bwp_size_rb);
  dmrs_pattern.crb_mask.fill(bwp_start_rb, bwp_start_rb + bwp_size_rb);
  dmrs_pattern.re_mask = get_dmrs_prb_mask(type, nof_cdm_groups_without_data);
  dmrs_pattern.symbols = symbol_mask;

  return dmrs_pattern;
}

} // namespace ocudu
