// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/support/tracing/resource_usage.h"
#include <chrono>
#include <string>

namespace ocudu {

/// Task executor metrics.
struct executor_metrics {
  /// Executor name.
  std::string name;
  /// Number of times executor ran a task.
  unsigned nof_executes;
  /// Number of times a task execution was deferred.
  unsigned nof_defers;
  /// Average task enqueueing latency in microseconds.
  std::chrono::microseconds avg_enqueue_latency_us;
  /// Maximum task enqueueing latency in microseconds.
  std::chrono::microseconds max_enqueue_latency_us;
  /// Average task duration in microseconds.
  std::chrono::microseconds avg_task_us;
  /// Maximum task duration in microseconds.
  std::chrono::microseconds max_task_us;
  /// Total rusage accumulated during the report period.
  resource_usage::diff total_rusg;
  /// CPU utilization.
  float cpu_load;
};

} // namespace ocudu
