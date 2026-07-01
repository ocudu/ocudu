// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/support/units.h"

namespace ocudu {

/// CPU usage of a measured block, e.g. some processing chain executed in a UNIX thread, or the CPU usage of the entire
/// UNIX process.
struct cpu_metrics {
  float cpu_usage_percentage;
  float cpu_utilization_nof_cpus;
};

/// Memory consumption of a UNIX process.
struct memory_metrics {
  /// Current memory usage (RSS) of the process.
  units::bytes memory_usage;
  /// Current memory usage of the process, as a percentage of the total system memory.
  float memory_usage_percentage;
};

/// Aggregates CPU usage of a measured block and memory consumption of the UNIX process.
struct resource_usage_metrics {
  cpu_metrics    cpu_stats;
  memory_metrics memory_stats;
  double         power_usage_watts;
};

} // namespace ocudu
