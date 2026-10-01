// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/support/resource_usage/power_consumption.h"

namespace ocudu {
namespace resource_usage_utils {

/// This class reads energy consumption using Powercap RAPL (Run Time Average Power Limiting) sysfs.
class sysfs_powercap_reader : public energy_consumption_reader
{
public:
  // See interface for documentation.
  energy_consumption read_consumed_energy() const override;

private:
  /// Converts string read from sysfs to uint64.
  uint64_t parse_uint64(const std::string& value) const;

  /// Read value from the given file and store its value in the provided variable.
  void read_value_from_sysfs(const std::string& sysfs_path, uint64_t& value_uj) const;
};

/// Creates Powercap RAPL sysfs reader if it is supported by the system.
std::unique_ptr<energy_consumption_reader> build_sysfs_powercap_reader(ocudulog::basic_logger& logger);

} // namespace resource_usage_utils
} // namespace ocudu
