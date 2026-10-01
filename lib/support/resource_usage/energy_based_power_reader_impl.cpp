// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "energy_based_power_reader_impl.h"
#include "ocudu/support/ocudu_assert.h"

using namespace ocudu;
using namespace resource_usage_utils;

energy_based_power_reader::energy_based_power_reader(std::unique_ptr<energy_consumption_reader> energy_reader_) :
  energy_reader(std::move(energy_reader_))
{
  ocudu_assert(energy_reader, "Energy consumption reader must be non-null");
  last_snapshot = energy_usage_now();
}

std::optional<double> energy_based_power_reader::read_power_watts()
{
  energy_snapshot current_snapshot = energy_usage_now();

  // In the following calculations we use only package consumption.
  uint64_t consumed_energy =
      calculate_energy_diff(current_snapshot.probe.package_consumed_uj, last_snapshot.probe.package_consumed_uj);

  std::chrono::microseconds time_diff =
      std::chrono::duration_cast<std::chrono::microseconds>(current_snapshot.probe_time - last_snapshot.probe_time);

  last_snapshot = current_snapshot;

  if (time_diff.count() == 0) {
    return std::nullopt;
  }
  return static_cast<double>(consumed_energy) / static_cast<double>(time_diff.count());
}

energy_based_power_reader::energy_snapshot energy_based_power_reader::energy_usage_now() const
{
  energy_snapshot current_snapshot;
  current_snapshot.probe_time = std::chrono::steady_clock::now();
  current_snapshot.probe      = energy_reader->read_consumed_energy();

  return current_snapshot;
}
