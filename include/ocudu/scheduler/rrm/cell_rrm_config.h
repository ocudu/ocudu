// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/pucch/pucch_configuration.h"
#include "ocudu/support/ocudu_assert.h"
#include <vector>

namespace ocudu {

struct ran_cell_config;

/// PUCCH resources that the RRM allocates to the UEs of a cell.
struct cell_pucch_rrm_config {
  /// UE-dedicated PUCCH resources of the cell.
  std::vector<pucch_resource> resources;

  /// Gets a UE-dedicated PUCCH resource by its resource ID.
  const pucch_resource& get(pucch_res_id_t res_id) const
  {
    ocudu_assert(res_id.ded_id() < resources.size(),
                 "PUCCH cell resource ID {} exceeds the size of the dedicated cell resource list ({})",
                 res_id.ded_id(),
                 resources.size());
    return resources[res_id.ded_id()];
  }
};

/// \brief Cell resources that the RRM allocates to the UEs of a cell.
///
/// The RRM generates it when it creates the cell. The DU keeps it to fill the RRC messages, and the scheduler gets it
/// in the cell configuration request.
struct cell_rrm_config {
  cell_pucch_rrm_config pucch;
};

/// Generates the cell resources that the RRM allocates to the UEs of a cell.
cell_rrm_config make_cell_rrm_config(const ran_cell_config& cell_cfg);

} // namespace ocudu
