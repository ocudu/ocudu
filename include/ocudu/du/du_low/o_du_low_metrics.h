// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/du/du_low/du_low_metrics.h"

namespace ocudu {
namespace odu {

/// O-RAN DU low metrics.
struct o_du_low_metrics {
  /// DU low metrics.
  du_low_metrics du_lo_metrics;
};

} // namespace odu
} // namespace ocudu
