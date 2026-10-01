// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/support/resource_grid.h"
#include "ocudu/phy/support/resource_grid_context.h"
#include "ocudu/phy/support/shared_resource_grid.h"
#include <vector>

namespace ocudu {

/// Resource grid pool interface.
class resource_grid_pool
{
public:
  /// Default destructor.
  virtual ~resource_grid_pool() = default;

  /// \brief Allocates a resource grid for the given context.
  ///
  /// Attempts to allocate a resource grid based on the provided allocation context. If the system is under high load,
  /// the allocation might fail. When allocation fails, the reason for the failure is logged to the \e PHY logger
  /// channel.
  ///
  /// \param [in] slot The slot for resource grid allocation.
  /// \return A valid shared resource grid if the allocation is successful; otherwise, an invalid shared resource grid.
  virtual shared_resource_grid allocate_resource_grid(slot_point slot) = 0;
};

} // namespace ocudu
