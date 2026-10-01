// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/support/resource_grid.h"
#include "ocudu/ran/slot_point.h"

namespace ocudu {

/// Describes the transmission and reception context.
struct resource_grid_context {
  /// Provides the slot context within the system frame.
  slot_point slot;
  /// Provides the sector identifier.
  unsigned sector;
};

} // namespace ocudu
