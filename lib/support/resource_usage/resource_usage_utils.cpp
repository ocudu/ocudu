// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/support/resource_usage/resource_usage_utils.h"
#include <fstream>
#include <sstream>
#include <string>

using namespace ocudu;
using namespace resource_usage_utils;

/// Reads the current resident set size of the calling process (in kilobytes) from procfs.
/// Unlike getrusage's ru_maxrss, this reflects the live memory footprint and can decrease over time.
static long current_rss_kb()
{
  std::ifstream status_file("/proc/self/status");
  std::string   line;
  while (std::getline(status_file, line)) {
    if (line.compare(0, 6, "VmRSS:") == 0) {
      std::istringstream iss(line.substr(6));
      long               kb = -1;
      iss >> kb;
      return kb;
    }
  }
  return -1;
}

/// Converts rusage struct to the snapshot.
static cpu_snapshot to_snapshot(const ::rusage& rusg)
{
  cpu_snapshot s;
  s.tp          = rusage_meas_clock::now();
  s.user_time   = std::chrono::seconds{rusg.ru_utime.tv_sec} + std::chrono::microseconds{rusg.ru_utime.tv_usec};
  s.system_time = std::chrono::seconds{rusg.ru_stime.tv_sec} + std::chrono::microseconds{rusg.ru_stime.tv_usec};
  s.current_rss = current_rss_kb();

  return s;
}

expected<cpu_snapshot, int> resource_usage_utils::cpu_usage_now(rusage_measurement_type type)
{
  ::rusage ret;
  if (::getrusage(static_cast<__rusage_who_t>(type), &ret) == 0) {
    return to_snapshot(ret);
  }
  return make_unexpected(errno);
}

measurements resource_usage_utils::operator+(const measurements& lhs, const measurements& rhs)
{
  measurements sum;
  sum.duration    = lhs.duration + lhs.duration;
  sum.user_time   = lhs.user_time + rhs.user_time;
  sum.system_time = lhs.system_time + lhs.system_time;
  sum.current_rss = std::max(lhs.current_rss, rhs.current_rss);

  return sum;
}

resource_usage_metrics resource_usage_utils::res_usage_measurements_to_metrics(measurements              measurements,
                                                                               std::chrono::microseconds period)
{
  static constexpr unsigned BYTES_IN_KB = 1024;

  auto total_cpu_time_used = measurements.user_time + measurements.system_time;

  resource_usage_metrics metrics = {};
  metrics.cpu_stats.cpu_usage_percentage =
      static_cast<double>(total_cpu_time_used.count()) / static_cast<double>(period.count());
  metrics.cpu_stats.cpu_utilization_nof_cpus =
      static_cast<double>(total_cpu_time_used.count()) / static_cast<double>(measurements.duration.count());
  metrics.memory_stats.memory_usage = units::bytes(BYTES_IN_KB * measurements.current_rss);
  return metrics;
}
