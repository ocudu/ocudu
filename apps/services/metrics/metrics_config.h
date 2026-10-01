// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "apps/services/metrics/metrics_consumer.h"
#include "apps/services/metrics/metrics_producer.h"
#include "ocudu/adt/span.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/support/synchronization/stop_event.h"
#include <memory>
#include <string>
#include <vector>

namespace ocudu {

class task_executor;

namespace app_services {

/// \brief Metrics callback.
///
/// This callback is used to notify metrics to the given metrics consumer running in a different execution context
/// defined by the given task executor. The implementation of the callback must make sure that the metrics object lives
/// longer than the handling of the metrics by the consumer.
using metrics_callback = std::function<
    void(const metrics_set&, span<metrics_consumer*>, task_executor&, ocudulog::basic_logger&, stop_event_token)>;

/// Metrics configuration.
struct metrics_config {
  /// Metric name.
  std::string metric_name;
  /// Callback to be executed when received this type of metrics. This callback is used to notify the consumers of new
  /// metrics using a different execution context.
  metrics_callback callback;
  /// Metric producer.
  std::vector<std::unique_ptr<metrics_producer>> producers;
  /// Metric consumer.
  std::vector<std::unique_ptr<metrics_consumer>> consumers;
};

} // namespace app_services
} // namespace ocudu
