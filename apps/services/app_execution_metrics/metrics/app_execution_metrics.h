// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "apps/services/metrics/metrics_consumer.h"
#include "apps/services/metrics/metrics_properties.h"
#include "apps/services/metrics/metrics_set.h"
#include "ocudu/adt/span.h"
#include "ocudu/ocudulog/logger.h"
#include "ocudu/support/executors/metrics/executor_metrics.h"
#include "ocudu/support/executors/task_executor.h"

namespace ocudu {

/// Executors metrics properties implementation.
class executor_metrics_properties_impl : public app_services::metrics_properties
{
public:
  std::string_view name() const override { return "Executor metrics"; }
};

class executor_metrics_impl : public app_services::metrics_set
{
  executor_metrics_properties_impl properties;
  executor_metrics                 metrics;

public:
  explicit executor_metrics_impl(const executor_metrics& metrics_) : metrics(metrics_) {}

  // See interface for documentation.
  const app_services::metrics_properties& get_properties() const override { return properties; }

  const executor_metrics& get_metrics() const { return metrics; }
};

} // namespace ocudu
