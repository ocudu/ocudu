// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/slot_point_extended.h"
#include <chrono>

namespace ocudu {

/// Describes the context of the current timing boundary.
struct upper_phy_timing_context {
  /// Indicates the current slot.
  slot_point_extended slot;
  /// Indicates the system time point associated to the current slot.
  std::chrono::time_point<std::chrono::system_clock> time_point;
};

} // namespace ocudu
