// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/upper/upper_phy_metrics.h"

namespace ocudu {
namespace odu {

/// DU low metrics.
struct du_low_metrics {
  /// Sector metrics.
  upper_phy_metrics upper_metrics;
};

} // namespace odu
} // namespace ocudu
