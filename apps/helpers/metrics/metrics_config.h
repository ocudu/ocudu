// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include <cstdint>
#include <string>

namespace ocudu {
namespace app_helpers {

/// Metrics configuration structure.
struct metrics_config {
  /// Enables the metrics in the log.
  bool enable_log_metrics = false;
  /// Enables the metrics in JSON format.
  bool enable_json_metrics = false;
  /// Enable verbose metrics.
  bool enable_verbose = false;

  /// Returns true if the metrics are enabled, otherwise false.
  bool enabled() const { return enable_log_metrics || enable_json_metrics; }
};

} // namespace app_helpers
} // namespace ocudu
