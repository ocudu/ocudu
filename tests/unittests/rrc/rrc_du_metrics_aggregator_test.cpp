// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/rrc/metrics/rrc_du_metrics_aggregator.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocucp;

TEST(rrc_du_metrics_aggregator_test, when_resume_cause_is_unknown_then_only_the_unknown_counter_increases)
{
  rrc_du_metrics_aggregator aggregator;

  aggregator.aggregate_attempted_connection_resume(resume_cause_t::unknown);

  rrc_du_metrics metrics;
  aggregator.collect_metrics(metrics);

  ASSERT_EQ(metrics.attempted_rrc_connection_resumes.get_count(resume_cause_t::unknown), 1);
  for (unsigned i = 0, e = metrics.successful_rrc_connection_resumes.size(); i != e; ++i) {
    ASSERT_EQ(metrics.successful_rrc_connection_resumes.get_count(i), 0) << "cause index " << i;
  }
}

TEST(rrc_du_metrics_aggregator_test, when_one_rrc_connection_is_added_then_mean_and_max_are_one)
{
  rrc_du_metrics_aggregator aggregator;

  aggregator.aggregate_successful_rrc_setup();

  rrc_du_metrics metrics;
  aggregator.collect_metrics(metrics);

  ASSERT_EQ(metrics.mean_nof_rrc_connections, 1);
  ASSERT_EQ(metrics.max_nof_rrc_connections, 1);
}

TEST(rrc_du_metrics_aggregator_test, when_rrc_connection_setup_times_are_added_then_mean_and_max_are_reported)
{
  rrc_du_metrics_aggregator aggregator;

  aggregator.aggregate_rrc_connection_setup_time(std::chrono::milliseconds{10});
  aggregator.aggregate_rrc_connection_setup_time(std::chrono::milliseconds{30});

  rrc_du_metrics metrics;
  aggregator.collect_metrics(metrics);

  ASSERT_EQ(metrics.mean_rrc_connection_setup_time, std::chrono::milliseconds{20});
  ASSERT_EQ(metrics.max_rrc_connection_setup_time, std::chrono::milliseconds{30});

  // The next reporting period starts without RRC connection setup times.
  aggregator.collect_metrics(metrics);

  ASSERT_FALSE(metrics.mean_rrc_connection_setup_time.has_value());
  ASSERT_FALSE(metrics.max_rrc_connection_setup_time.has_value());
}
