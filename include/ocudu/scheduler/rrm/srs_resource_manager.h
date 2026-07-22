// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/du_types.h"

namespace ocudu {

struct ran_cell_config;
struct ue_cell_config;

/// This abstract class defines the methods that the SRS resource manager must implement. The implementation of this
/// class defines different policies for the SRS allocation.
class srs_resource_manager
{
public:
  virtual ~srs_resource_manager() = default;

  /// Add resources for a new DU cell.
  virtual void add_cell(du_cell_index_t cell_idx, const ran_cell_config& cell_cfg) = 0;

  /// \brief Allocate SRS resources for a given UE. The resources are stored in the UE's cell configuration.
  /// \return true if allocation was successful.
  virtual bool alloc_resources(ue_cell_config& ue_cell_cfg) = 0;

  /// \brief Deallocate the SRS resources for a given UE and return the used resource to the common pool.
  virtual void dealloc_resources(ue_cell_config& ue_cell_cfg) = 0;

  /// Gets the current number of free SRS resource ID and offset pairs.
  virtual unsigned get_nof_srs_free_res_offsets(du_cell_index_t cell_idx) const = 0;
};

} // namespace ocudu
