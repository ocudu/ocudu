// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/du/du_high/du_manager/du_procedure_metrics_report.h"
#include "ocudu/f1ap/du/f1ap_du_metrics_collector.h"
#include "ocudu/mac/mac_metrics.h"
#include "ocudu/scheduler/scheduler_metrics.h"
#include <chrono>

namespace ocudu {
namespace odu {

/// \brief Report of the DU metrics.
struct du_metrics_report {
  unsigned                                           version = 0;
  std::chrono::time_point<std::chrono::steady_clock> start_time;
  std::chrono::milliseconds                          period;
  std::optional<f1ap_metrics_report>                 f1ap;
  std::optional<mac_metric_report>                   mac;
  std::optional<du_procedure_metrics_report>         du_proc;
};

} // namespace odu
} // namespace ocudu
