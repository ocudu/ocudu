// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "apps/helpers/metrics/json_generators/cu_cp/cu_cp_json_helper.h"
#include <gtest/gtest.h>

using namespace ocudu;

TEST(cu_cp_json_helper_test, when_ngap_metrics_are_reported_then_ng_connection_metrics_are_in_the_json)
{
  cu_cp_metrics_report report;
  ngap_info&           ngap                                                    = report.ngaps.emplace_back();
  ngap.metrics.nof_ue_associated_logical_ng_connection_establishment_attempts  = 3;
  ngap.metrics.nof_ue_associated_logical_ng_connection_establishment_successes = 2;

  nlohmann::json json = app_helpers::json_generators::generate(report);

  const nlohmann::json& ng_connection = json["cu-cp"]["ngaps"]["ngap"][0]["ue_associated_logical_ng_connection"];
  ASSERT_EQ(ng_connection["nof_ue_associated_logical_ng_connection_establishment_attempts"], 3);
  ASSERT_EQ(ng_connection["nof_ue_associated_logical_ng_connection_establishment_successes"], 2);
}

TEST(cu_cp_json_helper_test, when_rrc_establishments_fail_then_the_failures_are_in_the_json)
{
  cu_cp_metrics_report           report;
  cu_cp_metrics_report::du_info& du = report.dus.emplace_back();
  du.rrc_metrics.failed_rrc_connection_establishments.increase(establishment_fail_cause_t::no_reply);

  nlohmann::json json = app_helpers::json_generators::generate(report);

  const nlohmann::json& failed =
      json["cu-cp"]["rrcs"]["du"][0]["rrc_connection_establishment"]["failed_rrc_connection_establishments"];
  ASSERT_EQ(failed[fmt::format("{}", establishment_fail_cause_t::no_reply)], 1);
  ASSERT_EQ(failed[fmt::format("{}", establishment_fail_cause_t::network_reject)], 0);
}

TEST(cu_cp_json_helper_test, when_idle_state_rrc_releases_are_reported_then_they_are_in_the_json)
{
  cu_cp_metrics_report           report;
  cu_cp_metrics_report::du_info& du              = report.dus.emplace_back();
  du.rrc_metrics.idle_state_rrc_release_messages = 2;

  nlohmann::json json = app_helpers::json_generators::generate(report);

  ASSERT_EQ(json["cu-cp"]["rrcs"]["du"][0]["rrc_connection_establishment"]["idle_state_rrc_release_messages"], 2);
}
