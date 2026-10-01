// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/support/executors/metrics/executor_metrics.h"

namespace ocudu {

/// Executors metrics.
class executor_metrics_impl final
{
  executor_metrics metrics;

public:
  explicit executor_metrics_impl(const executor_metrics& metrics_) : metrics(metrics_) {}

  const executor_metrics& get_metrics() const { return metrics; }
};

} // namespace ocudu
