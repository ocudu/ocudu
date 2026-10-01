// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/ocudulog/ocudulog.h"
#include <optional>

namespace ocudu {
namespace resource_usage_utils {

using energy_probe_time_point = std::chrono::steady_clock::time_point;

/// Energy consumption expressed in micro Joules.
struct energy_consumption {
  /// Energy consumed by CPU cores.
  uint64_t cpu_core_consumed_uj;
  /// Energy consumed by package (it can include CPU core, GPU and other components).
  uint64_t package_consumed_uj;
};

/// Calculates a diff of micro Joules taking into account possible wrap around.
uint64_t calculate_energy_diff(uint64_t current_uj, uint64_t previous_uj);

/// Interface for reading energy consumption.
class energy_consumption_reader
{
public:
  /// Default destructor.
  virtual ~energy_consumption_reader() = default;

  /// Returns consumed energy in micro Joules.
  virtual energy_consumption read_consumed_energy() const = 0;
};

/// Interface for reading power consumption.
class power_consumption_reader
{
public:
  /// Default destructor.
  virtual ~power_consumption_reader() = default;

  /// Returns the power consumption in Watts measured since the previous call, or std::nullopt if not available.
  virtual std::optional<double> read_power_watts() = 0;
};

/// Builds a power consumption reader available in the system.
std::unique_ptr<power_consumption_reader> build_power_consumption_reader(ocudulog::basic_logger& logger);

} // namespace resource_usage_utils
} // namespace ocudu
