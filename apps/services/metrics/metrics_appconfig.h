// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

namespace ocudu {
namespace app_services {

/// Metrics service application configuration.
struct metrics_appconfig {
  /// Application usage report period in milliseconds.
  unsigned app_usage_report_period = 1000;
};

} // namespace app_services
} // namespace ocudu
