// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/rrc/rrc_du.h"
#include "ocudu/rrc/rrc_metrics.h"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <map>

namespace ocudu::ocucp {

inline std::chrono::milliseconds get_current_time()
{
  return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch());
}

struct rrc_connection_establishment_metrics {
  rrc_connection_counter_with_cause      attempted_rrc_connection_establishments;
  rrc_connection_counter_with_cause      successful_rrc_connection_establishments;
  rrc_connection_fail_counter_with_cause failed_rrc_connection_establishments;
};

struct rrc_connection_reestablishment_metrics {
  unsigned attempted_rrc_connection_reestablishments                     = 0;
  unsigned successful_rrc_connection_reestablishments_with_ue_context    = 0;
  unsigned successful_rrc_connection_reestablishments_without_ue_context = 0;
};

struct rrc_connection_resume_metrics {
  rrc_connection_resume_counter_with_cause attempted_rrc_connection_resumes;
  rrc_connection_resume_counter_with_cause successful_rrc_connection_resumes;
  rrc_connection_resume_counter_with_cause successful_rrc_connection_resumes_with_fallback;
  rrc_connection_resume_counter_with_cause rrc_connection_resumes_followed_by_network_release;
  rrc_connection_resume_counter_with_cause attempted_rrc_connection_resumes_followed_by_rrc_setup;
};

class rrc_du_metrics_aggregator : public rrc_du_metrics_collector
{
public:
  rrc_du_metrics_aggregator();
  ~rrc_du_metrics_aggregator() = default;

  void aggregate_successful_rrc_setup();

  void aggregate_successful_rrc_release(bool is_inactive = false);

  void aggregate_successful_rrc_inactive();

  void aggregate_successful_rrc_resume();

  void aggregate_attempted_connection_establishment(establishment_cause_t cause);

  void aggregate_successful_connection_establishment(establishment_cause_t cause);

  void aggregate_failed_connection_establishment(establishment_fail_cause_t cause);

  void aggregate_attempted_connection_reestablishment();

  void aggregate_successful_connection_reestablishment(bool with_ue_context);

  void aggregate_attempted_connection_resume(resume_cause_t cause);

  void aggregate_successful_connection_resume(resume_cause_t cause);

  void aggregate_successful_connection_resume_with_fallback(resume_cause_t cause);

  void aggregate_connection_resume_followed_by_network_release(resume_cause_t cause);

  void aggregate_attempted_connection_resume_followed_by_rrc_setup(resume_cause_t cause);

  void collect_metrics(rrc_du_metrics& metrics) override;

private:
  /// Aggregator for RRC connection related metrics.
  struct rrc_connection_metrics_aggregator {
    rrc_connection_metrics_aggregator() = default;

    void reset()
    {
      rrc_connections_with_time.clear();
      rrc_connections_with_time.emplace(get_current_time(), current_rrc_connections);
    }

    void add_rrc_connection()
    {
      std::chrono::milliseconds now = get_current_time();
      current_rrc_connections++;
      rrc_connections_with_time[now] = current_rrc_connections;
    }

    void remove_rrc_connection()
    {
      std::chrono::milliseconds now = get_current_time();
      current_rrc_connections--;
      rrc_connections_with_time[now] = current_rrc_connections;
    }

    unsigned get_mean_nof_rrc_connections()
    {
      if (rrc_connections_with_time.size() < 2) {
        // No need to calculate weighted mean.
        return rrc_connections_with_time.begin()->second;
      }

      if (rrc_connections_with_time.size() == 2 && rrc_connections_with_time.begin()->second == 0) {
        // Only one measurement has been made (the map is initialized with a zero value).
        return rrc_connections_with_time.end()->second;
      }

      // Add current value to the map to count all actual measurements (the last value of the map will be ignored).
      auto now                       = get_current_time();
      rrc_connections_with_time[now] = current_rrc_connections;

      long long weighted_sum = 0;
      long long weight_sum   = 0;

      auto it         = rrc_connections_with_time.begin();
      auto prev_time  = it->first;
      auto prev_value = it->second;
      ++it;

      for (; it != rrc_connections_with_time.end(); ++it) {
        auto current_time  = it->first;
        auto current_value = it->second;

        // Calculate time difference (weight).
        auto time_diff = (current_time - prev_time).count();

        // Update weighted sum and total weight.
        weighted_sum += static_cast<long long>(prev_value) * time_diff;
        weight_sum += time_diff;

        // Update previous time and value.
        prev_time  = current_time;
        prev_value = current_value;
      }

      // Avoid division by zero.
      if (weight_sum == 0) {
        return 0;
      }

      return std::floor(static_cast<double>(weighted_sum) / weight_sum);
    }

    unsigned get_max_nof_rrc_connections()
    {
      // If the map is empty, no RRC connections have been established.
      if (rrc_connections_with_time.empty()) {
        return 0;
      }

      auto max_it = std::max_element(rrc_connections_with_time.begin(),
                                     rrc_connections_with_time.end(),
                                     [](const auto& lhs, const auto& rhs) { return lhs.second < rhs.second; });

      return max_it->second;
    }

    std::map<std::chrono::milliseconds, unsigned> rrc_connections_with_time;
    unsigned                                      current_rrc_connections = 0;
  };

  rrc_connection_metrics_aggregator      connection_metrics;
  rrc_connection_metrics_aggregator      inactive_connection_metrics;
  rrc_connection_establishment_metrics   connection_establishment_metrics;
  rrc_connection_reestablishment_metrics connection_reestablishment_metrics;
  rrc_connection_resume_metrics          connection_resume_metrics;
};

} // namespace ocudu::ocucp
