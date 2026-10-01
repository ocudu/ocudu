// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/du/du_high/o_du_high_metrics.h"
#include "ocudu/du/du_low/o_du_low_metrics.h"
#include <optional>

namespace ocudu {
namespace odu {

/// O-RAN DU metrics.
struct o_du_metrics {
  o_du_high_metrics               high;
  std::optional<o_du_low_metrics> low;
};

} // namespace odu
} // namespace ocudu
