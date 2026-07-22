// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/srs/srs_properties.h"
#include "ocudu/scheduler/scheduler_configurator.h"
#include <vector>

namespace ocudu {

struct ran_cell_config;

/// \brief Test double that allocates periodic SRS resources to UEs from a pool of valid UL-slot offsets, mirroring
/// how \c pucch_resource_manager allocates PUCCH resources, so tests can host many UEs with distinct periodic SRS
/// occasions without colliding in the same slot. Single-cell only.
class srs_resource_manager
{
public:
  /// \brief Construct the resource pool for the given cell.
  /// \remark The cell must have periodic SRS enabled (see \c srs_builder_params::srs_type_enabled).
  explicit srs_resource_manager(const ran_cell_config& cell_cfg);

  /// \brief Allocate a periodic SRS resource for a given UE. The resource is stored in the UE's cell configuration.
  /// \return true if allocation was successful.
  bool alloc_resources(ue_cell_config& ue_cell_cfg);

  /// \brief Deallocate the periodic SRS resource previously given to a UE. The offset is returned back to the pool.
  void dealloc_resources(ue_cell_config& ue_cell_cfg);

  /// Gets the current number of free periodic SRS offsets.
  unsigned get_nof_free_offsets() const { return free_offsets.size(); }

private:
  const srs_periodicity srs_period;
  std::vector<uint16_t> free_offsets;
};

} // namespace ocudu
