// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/support/io/unique_fd.h"
#include "ocudu/support/resource_usage/power_consumption.h"

namespace ocudu {
namespace resource_usage_utils {

/// Reads RAPL energy data via perf_event_open() syscall.
/// Requires CAP_PERFMON capability and Linux kernel >= 5.8.
class perf_event_powercap_reader : public energy_consumption_reader
{
public:
  perf_event_powercap_reader(unique_fd pkg_fd, double pkg_scale, unique_fd core_fd, double core_scale);

  // See interface for documentation.
  energy_consumption read_consumed_energy() const override;

private:
  /// Read a raw perf counter value and convert to micro Joules using the given scale factor.
  uint64_t read_raw_uj(const unique_fd& fd, double scale) const;

  unique_fd pkg_fd;
  double    pkg_scale;
  unique_fd core_fd;
  double    core_scale;
};

/// Creates a perf_event RAPL reader if supported by the system (requires CAP_PERFMON, kernel >= 5.8).
std::unique_ptr<energy_consumption_reader> build_perf_event_reader(ocudulog::basic_logger& logger);

} // namespace resource_usage_utils
} // namespace ocudu
