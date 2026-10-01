// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/support/resource_usage/power_consumption.h"
#include <string>
#include <vector>

namespace ocudu {
namespace resource_usage_utils {

/// Reads power consumption via the hwmon sysfs interface, summing all power channels of a hwmon device.
class hwmon_power_reader : public power_consumption_reader
{
public:
  explicit hwmon_power_reader(std::vector<std::string> power_input_paths_);

  // See interface for documentation.
  std::optional<double> read_power_watts() override;

private:
  /// Paths of the sysfs files reporting power in micro Watts.
  std::vector<std::string> power_input_paths;
};

/// Creates a hwmon power reader if a supported hwmon device is present in the system.
std::unique_ptr<power_consumption_reader> build_hwmon_power_reader(ocudulog::basic_logger& logger);

} // namespace resource_usage_utils
} // namespace ocudu
