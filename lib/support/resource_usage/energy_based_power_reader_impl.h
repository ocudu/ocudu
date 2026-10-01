// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/support/resource_usage/power_consumption.h"

namespace ocudu {
namespace resource_usage_utils {

/// Computes the average power consumption from the energy consumed between two consecutive reads.
class energy_based_power_reader : public power_consumption_reader
{
public:
  explicit energy_based_power_reader(std::unique_ptr<energy_consumption_reader> energy_reader_);

  // See interface for documentation.
  std::optional<double> read_power_watts() override;

private:
  /// Helper struct used to store energy consumption at a given point of time.
  struct energy_snapshot {
    energy_consumption      probe;
    energy_probe_time_point probe_time;
  };

  /// Returns a snapshot of the energy consumption.
  energy_snapshot energy_usage_now() const;

  std::unique_ptr<energy_consumption_reader> energy_reader;
  /// The last taken snapshot of the energy consumption.
  energy_snapshot last_snapshot;
};

} // namespace resource_usage_utils
} // namespace ocudu
