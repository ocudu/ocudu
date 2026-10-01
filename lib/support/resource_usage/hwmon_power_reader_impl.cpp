// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "hwmon_power_reader_impl.h"
#include <algorithm>
#include <array>
#include <charconv>
#include <filesystem>
#include <fstream>
#include <regex>
#include <string_view>

using namespace ocudu;
using namespace resource_usage_utils;

static constexpr auto hwmon_class_path = "/sys/class/hwmon";

/// Names of the supported hwmon devices whose power channels report the power consumption of the CPU package.
static constexpr std::array<std::string_view, 1> supported_hwmon_names = {"apm_xgene"};
static constexpr float                           uw_per_watt           = 1000000.f;

/// Reads the first line of a sysfs file. Returns an empty string on failure.
static std::string read_sysfs_line(const std::string& path)
{
  std::ifstream file(path);
  std::string   line;
  if (file.is_open()) {
    std::getline(file, line);
  }
  return line;
}

/// Reads an unsigned integer value from a sysfs file.
static std::optional<uint64_t> read_uint64_from_sysfs(const std::string& path)
{
  std::string value = read_sysfs_line(path);

  uint64_t result = 0;
  auto [ptr, ec]  = std::from_chars(value.data(), value.data() + value.size(), result);
  if (value.empty() || ec != std::errc() || ptr != (value.data() + value.size())) {
    return std::nullopt;
  }
  return result;
}

/// Returns true if the given hwmon device name is supported.
static bool is_supported_hwmon(std::string_view name)
{
  return std::find(supported_hwmon_names.begin(), supported_hwmon_names.end(), name) != supported_hwmon_names.end();
}

/// Returns the channel prefix (e.g. "power1") if the given file name is a power input attribute (e.g. "power1_input").
static std::optional<std::string> get_power_channel(const std::string& file_name)
{
  static const std::regex power_input_regex("(power[0-9]+)_input");

  std::smatch match;
  if (!std::regex_match(file_name, match, power_input_regex)) {
    return std::nullopt;
  }
  return match[1].str();
}

/// Returns the readable power channel paths of the given hwmon device, logging their labels.
static std::vector<std::string> find_power_input_paths(ocudulog::basic_logger&      logger,
                                                       const std::filesystem::path& hwmon_path)
{
  std::vector<std::string> channels;
  std::error_code          ec;
  for (const auto& entry : std::filesystem::directory_iterator(hwmon_path, ec)) {
    if (auto channel = get_power_channel(entry.path().filename().string())) {
      channels.push_back(*channel);
    }
  }
  std::sort(channels.begin(), channels.end());

  std::vector<std::string> paths;
  for (const auto& channel : channels) {
    std::string base_path = (hwmon_path / channel).string();
    // Prefer the averaged power when the driver provides it.
    std::string path = base_path + "_average";
    if (!read_uint64_from_sysfs(path)) {
      path = base_path + "_input";
    }
    std::optional<uint64_t> power_uw = read_uint64_from_sysfs(path);
    if (!power_uw) {
      logger.warning("Energy consumption utils: failed to read hwmon power channel '{}'.", path);
      continue;
    }

    logger.info("Energy consumption utils: using hwmon power channel '{}' (label='{}', power={:.2f} Watts).",
                path,
                read_sysfs_line(base_path + "_label"),
                static_cast<double>(*power_uw) / uw_per_watt);
    paths.push_back(std::move(path));
  }
  return paths;
}

hwmon_power_reader::hwmon_power_reader(std::vector<std::string> power_input_paths_) :
  power_input_paths(std::move(power_input_paths_))
{
}

std::optional<double> hwmon_power_reader::read_power_watts()
{
  uint64_t total_power_uw = 0;
  for (const auto& path : power_input_paths) {
    std::optional<uint64_t> power_uw = read_uint64_from_sysfs(path);
    if (!power_uw) {
      return std::nullopt;
    }
    total_power_uw += *power_uw;
  }
  return static_cast<double>(total_power_uw) / uw_per_watt;
}

std::unique_ptr<power_consumption_reader> resource_usage_utils::build_hwmon_power_reader(ocudulog::basic_logger& logger)
{
  std::error_code ec;
  for (const auto& entry : std::filesystem::directory_iterator(hwmon_class_path, ec)) {
    const std::filesystem::path& hwmon_path = entry.path();
    std::string                  name       = read_sysfs_line((hwmon_path / "name").string());
    if (!is_supported_hwmon(name)) {
      continue;
    }

    std::vector<std::string> paths = find_power_input_paths(logger, hwmon_path);
    if (paths.empty()) {
      continue;
    }

    logger.info("Energy consumption utils: using hwmon device '{}' at '{}'.", name, hwmon_path.string());
    return std::make_unique<hwmon_power_reader>(std::move(paths));
  }

  logger.warning("Energy consumption utils: supported hwmon power sensors are not available.");
  return nullptr;
}
