// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "perf_event_powercap_reader_impl.h"
#include <charconv>
#include <fstream>
#include <linux/perf_event.h>
#include <sstream>
#include <sys/syscall.h>

using namespace ocudu;
using namespace resource_usage_utils;

static constexpr auto powercap_pmu_type_path   = "/sys/bus/event_source/devices/power/type";
static constexpr auto powercap_pkg_event_path  = "/sys/bus/event_source/devices/power/events/energy-pkg";
static constexpr auto powercap_pkg_scale_path  = "/sys/bus/event_source/devices/power/events/energy-pkg.scale";
static constexpr auto powercap_core_event_path = "/sys/bus/event_source/devices/power/events/energy-cores";
static constexpr auto powercap_core_scale_path = "/sys/bus/event_source/devices/power/events/energy-cores.scale";

/// Thin syscall wrapper for perf_event_open(2). Returns the file descriptor on success, -1 on failure.
static int perf_event_open_syscall(struct perf_event_attr* attr, pid_t pid, int cpu, int group_fd, unsigned long flags)
{
  return static_cast<int>(::syscall(SYS_perf_event_open, attr, pid, cpu, group_fd, flags));
}

/// Read the first line from a sysfs file. Returns the line on success, an empty string on failure.
static std::string read_sysfs_line(const char* path)
{
  std::ifstream f(path);
  std::string   line;
  if (f.is_open()) {
    std::getline(f, line);
  }
  return line;
}

/// Parses a floating-point scale value from sysfs. Returns true on success, false otherwise.
static bool parse_scale(const std::string& line, double& scale)
{
  std::istringstream stream(line);
  stream >> scale;
  return stream && stream.eof();
}

/// Parse "event=0xNN" string and extract the hex value into config. Returns true on success, false otherwise.
static bool parse_event_config(const std::string& line, uint64_t& config)
{
  const auto pos = line.find("event=0x");
  if (pos == std::string::npos) {
    return false;
  }
  const char* start = line.c_str() + pos + 8; // skip "event=0x"
  auto [ptr, ec]    = std::from_chars(start, line.c_str() + line.size(), config, 16);
  return ec == std::errc();
}

/// Open a perf RAPL fd for the given PMU type and event config on CPU 0. Returns the fd on success, -1 on failure.
static int open_rapl_perf_fd(uint32_t pmu_type, uint64_t event_config)
{
  struct perf_event_attr attr = {};
  attr.type                   = pmu_type;
  attr.size                   = sizeof(attr);
  attr.config                 = event_config;
  attr.disabled               = 0;
  attr.inherit                = 0;

  // pid=-1 (all tasks), cpu=0 (package 0), group_fd=-1, flags=0.
  return perf_event_open_syscall(&attr, -1, 0, -1, 0);
}

perf_event_powercap_reader::perf_event_powercap_reader(unique_fd pkg_fd_,
                                                       double    pkg_scale_,
                                                       unique_fd core_fd_,
                                                       double    core_scale_) :
  pkg_fd(std::move(pkg_fd_)), pkg_scale(pkg_scale_), core_fd(std::move(core_fd_)), core_scale(core_scale_)
{
}

/// Reads a raw perf counter value from fd and converts it to micro Joules using scale. Returns 0 on failure.
uint64_t perf_event_powercap_reader::read_raw_uj(const unique_fd& fd, double scale) const
{
  uint64_t raw = 0;
  if (!fd.is_open() || ::read(fd.value(), &raw, sizeof(raw)) != static_cast<ssize_t>(sizeof(raw))) {
    return 0;
  }
  // scale is in Joules per raw unit — convert to micro Joules.
  return static_cast<uint64_t>(static_cast<double>(raw) * scale * 1e6);
}

/// Reads the package and core energy counters. Returns an energy_consumption with values in micro Joules.
energy_consumption perf_event_powercap_reader::read_consumed_energy() const
{
  energy_consumption probe   = {};
  probe.package_consumed_uj  = read_raw_uj(pkg_fd, pkg_scale);
  probe.cpu_core_consumed_uj = core_fd.is_open() ? read_raw_uj(core_fd, core_scale) : 0;
  return probe;
}

/// Creates a perf_event RAPL reader. Returns a valid reader on success, nullptr if the system does not support it.
std::unique_ptr<energy_consumption_reader> resource_usage_utils::build_perf_event_reader(ocudulog::basic_logger& logger)
{
  // Read RAPL PMU type.
  const std::string pmu_type_str = read_sysfs_line(powercap_pmu_type_path);
  if (pmu_type_str.empty()) {
    logger.warning("Energy consumption utils: perf RAPL PMU not available.");
    return nullptr;
  }

  uint32_t pmu_type = 0;
  {
    auto [ptr, ec] = std::from_chars(pmu_type_str.data(), pmu_type_str.data() + pmu_type_str.size(), pmu_type);
    if (ec != std::errc()) {
      logger.warning("Energy consumption utils: failed to parse perf RAPL PMU type.");
      return nullptr;
    }
  }

  // Read package event config and scale.
  uint64_t pkg_event_config = 0;
  if (!parse_event_config(read_sysfs_line(powercap_pkg_event_path), pkg_event_config)) {
    logger.warning("Energy consumption utils: perf RAPL package event config not available.");
    return nullptr;
  }
  double pkg_scale = 0.0;
  if (!parse_scale(read_sysfs_line(powercap_pkg_scale_path), pkg_scale)) {
    logger.warning("Energy consumption utils: failed to parse perf RAPL package scale.");
    return nullptr;
  }

  // Open package perf fd.
  unique_fd pkg_fd(open_rapl_perf_fd(pmu_type, pkg_event_config));
  if (!pkg_fd.is_open()) {
    logger.warning(
        "Energy consumption utils: failed to open perf RAPL fd — requires CAP_PERFMON and Linux kernel >= 5.8.");
    return nullptr;
  }

  // Optionally open core energy fd (not available on all CPUs).
  unique_fd core_fd;
  double    core_scale = 0.0;
  {
    uint64_t core_event_config = 0;
    if (parse_event_config(read_sysfs_line(powercap_core_event_path), core_event_config)) {
      if (parse_scale(read_sysfs_line(powercap_core_scale_path), core_scale)) {
        core_fd = unique_fd(open_rapl_perf_fd(pmu_type, core_event_config));
      }
    }
  }

  return std::make_unique<perf_event_powercap_reader>(std::move(pkg_fd), pkg_scale, std::move(core_fd), core_scale);
}
