// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/pucch/pucch_configuration.h"
#include "ocudu/ran/pucch/pucch_constants.h"
#include "ocudu/scheduler/config/ran_cell_config.h"
#include <vector>

namespace ocudu {

/// This structure defines all the UE dedicated DL resources available in a given BWP.
struct cell_dl_bwp_res_config {
  /// List of possible UE-dedicated PDCCH configs.
  std::vector<pdcch_config> ded_pdcchs;

  bool operator==(const cell_dl_bwp_res_config& other) const { return ded_pdcchs == other.ded_pdcchs; }
};

/// This structure defines all the PUCCH resources available in a given BWP.
struct cell_pucch_res_config {
  /// List of common PUCCH resources, indexed by \f$r_{PUCCH}\f$ (TS 38.213 Section 9.2.1).
  std::array<pucch_resource, pucch_constants::MAX_NOF_CELL_COMMON_PUCCH_RESOURCES> common;
  /// List of all supported UE-dedicated PUCCH resources.
  std::vector<pucch_resource> dedicated;

  bool operator==(const cell_pucch_res_config& other) const
  {
    return common == other.common and dedicated == other.dedicated;
  }

  /// Returns the total number of PUCCH resources in the cell.
  unsigned nof_total_res() const { return common.size() + dedicated.size(); }

  /// Gets the common PUCCH resource indexed by \f$r_{PUCCH}\f$.
  const pucch_resource& get_cmn(unsigned r_pucch) const
  {
    ocudu_assert(r_pucch < common.size(), "Invalid r_pucch for common PUCCH resource: {}", r_pucch);
    return common[r_pucch];
  }

  /// Gets the dedicated PUCCH resource indexed by its resource ID.
  const pucch_resource& get_ded(pucch_res_id_t res_id) const
  {
    ocudu_assert(res_id.ded().cell_res_id < dedicated.size(),
                 "PUCCH cell resource ID {} exceeds the size of the dedicated cell resource list ({})",
                 res_id.ded().cell_res_id,
                 dedicated.size());
    return dedicated[res_id.ded().cell_res_id];
  }
};

/// \brief Cell-wide UL resources available in a given BWP.
struct cell_ul_bwp_res_config {
  cell_pucch_res_config pucch;

  bool operator==(const cell_ul_bwp_res_config& other) const { return pucch == other.pucch; }
};

/// \brief List of all configured RAN resources associated with a given cell and BWP.
///
/// This list contains both common and the UE-dedicated resources that UEs may use for a given BWP.
struct cell_bwp_res_config {
  cell_dl_bwp_res_config dl;
  cell_ul_bwp_res_config ul;

  bool operator==(const cell_bwp_res_config& other) const { return ul == other.ul; }
};

// Generate the cell-common BWP configuration from the cell configuration.
cell_bwp_res_config make_cell_bwp_res_config(const ran_cell_config& cell_cfg);

} // namespace ocudu
