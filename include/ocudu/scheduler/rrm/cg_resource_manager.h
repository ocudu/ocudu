// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/du_types.h"

namespace ocudu {

struct ran_cell_config;
struct ue_cell_config;

/// This abstract class defines the methods that the Configured Grant resource manager must implement. The
/// implementation of this class defines different policies for the CG resource allocation.
class cg_resource_manager
{
public:
  virtual ~cg_resource_manager() = default;

  /// \brief Register a cell with the CG resource manager.
  virtual void add_cell(du_cell_index_t cell_idx, const ran_cell_config& cell_cfg) {}

  /// \brief Deregister a cell from the CG resource manager.
  virtual void rem_cell(du_cell_index_t cell_idx) {}

  /// \brief Allocate Configured Grant resources for a given UE. The resources are stored in the UE's cell
  /// configuration. The function allocates the UE the resources from a common pool.
  /// \return true if allocation is successful or if the Configured grant resource allocation was not requested (i.e.,
  /// not set in by the user).
  virtual bool alloc_resources(ue_cell_config& ue_cell_cfg) = 0;

  /// \brief Deallocate the Configured Grant resources for a given UE and return the used resource to the common pool.
  virtual void dealloc_resources(ue_cell_config& ue_cell_cfg) = 0;
};

} // namespace ocudu
