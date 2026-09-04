// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/du_types.h"

namespace ocudu {

struct ran_cell_config;
struct ue_cell_config;

/// This abstract class defines the methods that the Configured Grant resource manager must implement. The
/// implementation of this class defines different policies for the CG resource allocation.
class configured_grant_rrm
{
public:
  virtual ~configured_grant_rrm() = default;

  /// \brief Register a cell with the CG resource manager.
  virtual void add_cell(du_cell_index_t cell_idx, const ran_cell_config& cell_cfg) {}

  /// \brief Deregister a cell from the CG resource manager.
  virtual void rem_cell(du_cell_index_t cell_idx) {}

  /// \brief Builds the CG configuration for a given UE and ensures the cell has enough CG resources to accommodate this
  /// UE.
  ///
  /// \return true if the UE can be accommodate or if the Configured grant is set in by the user.
  /// \remark For CG type 1, this function allocates the CG resources to the UE; these resources are taken from a common
  /// pool.
  virtual bool build_ue_cg_config(ue_cell_config& ue_cell_cfg) = 0;

  /// \brief Reset the UE's CG configuration Grant resources for a given UE and return the used resource to the common
  /// pool.
  /// \remark For CG type 1, this function return the used resource to the common pool.
  virtual void reset_ue_cg_config(ue_cell_config& ue_cell_cfg) = 0;
};

} // namespace ocudu
