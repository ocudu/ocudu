// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/ntn/converters/reference_frame_converter.h"
#include "lib/ntn/coordinates_types.h"
#include "ocudu/support/test_utils.h"
#include "fmt/chrono.h"
#include <cmath>
#include <gtest/gtest.h>
#include <iomanip>
#include <string>
#include <vector>

using namespace ocudu;
using namespace ocudu_ntn;

static std::chrono::system_clock::time_point string_to_timepoint(const std::string& input)
{
  std::tm            tm = {};
  std::istringstream ss(input);
  ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");
  if (ss.fail()) {
    throw std::runtime_error("Failed to parse time string");
  }
  std::time_t time = timegm(&tm);
  return std::chrono::system_clock::from_time_t(time);
}

TEST(test_converters, ecef_2_eci_test)
{
  double pos_tolerance = 1e-3; // m -> 0.1cm
  double vel_tolerance = 1e-6; // m/s
  double pos_error     = 0;
  double vel_error     = 0;

  std::vector<std::tuple<std::string, state_vector, state_vector>> test_cases = {
      {"2025-06-24T08:49:09",
       {{2388595.31462629, -6443404.17018643, 9540.82104068}, {3798.13162655, 1424.36001124, 6105.64102076}},
       {{2388595.31462629, -6443404.17018643, 9540.82104068}, {4267.99207801, 1598.53913197, 6105.64102076}}},
      {"2025-06-24T08:50:09",
       {{2611463.66175304, -6344787.43848636, 375586.49406261}, {3628.72726962, 1861.91229016, 6091.38301467}},
       {{2611463.66175304, -6344787.43848636, 375586.49406261}, {4091.39647544, 2052.34322739, 6091.38301467}}},
      {"2025-06-24T08:50:39",
       {{2718992.89574091, -6285679.38519413, 558085.41058848}, {3539.39590614, 2078.3264241, 6074.10316297}},
       {{2718992.89574091, -6285679.38519413, 558085.41058848}, {3997.75488466, 2276.59851689, 6074.10316297}}},
      {"2025-06-24T09:03:30",
       {{-30766715.00178485, -28920664.97841574, -1726488.83031733}, {-10.02423634, 34.7572502, -385.66582933}},
       {{-30766715.00178485, -28920664.97841574, -1726488.83031733}, {2098.90395508, -2208.78703459, -385.66582933}}},
      {"2025-06-24T09:04:10",
       {{-30767113.42311617, -28919273.63016415, -1741908.21591293}, {-9.86597378, 34.77340666, -385.2995744}},
       {{-30767113.42311617, -28919273.63016415, -1741908.21591293}, {2098.96075892, -2208.79993148, -385.2995744}}},
      {"2025-06-24T09:04:50",
       {{-30767505.51211233, -28917881.65316424, -1757312.8909617}, {-9.70760663, 34.78865427, -384.93006262}},
       {{-30767505.51211233, -28917881.65316424, -1757312.8909617}, {2099.01762151, -2208.81327544, -384.93006262}}},
  };

  for (const auto& [utc_time, ecef_rv, expected_eci_rv] : test_cases) {
    auto                      tp = string_to_timepoint(utc_time);
    reference_frame_converter converter(tp);
    state_vector              eci_rv  = converter.ecef_to_eci(ecef_rv);
    state_vector              rv_diff = eci_rv - expected_eci_rv;
    pos_error                         = norm(rv_diff.position);
    vel_error                         = norm(rv_diff.velocity);
    ASSERT_NEAR(pos_error, 0.0, pos_tolerance);
    ASSERT_NEAR(vel_error, 0.0, vel_tolerance);
  }
}

TEST(test_converters, eci_2_ecef_test)
{
  double pos_tolerance = 1e-3; // m -> 0.1cm
  double vel_tolerance = 1e-6; // m/s
  double pos_error     = 0;
  double vel_error     = 0;

  std::vector<std::tuple<std::string, state_vector, state_vector>> test_cases = {
      {"2025-06-24T08:54:52",
       {{2226701.89125344, -6501126.33412133, 9539.45337909}, {4306.63616141, 1491.29763946, 6105.6410234}},
       {{2226701.89125344, -6501126.33412133, 9539.45337909}, {3832.56654329, 1328.92397357, 6105.6410234}}},
      {"2025-06-24T08:55:32",
       {{2377939.40981165, -6442060.92620855, 253674.9999991}, {4198.13259986, 1797.36909924, 6099.14621378}},
       {{2377939.40981165, -6442060.92620855, 253674.9999991}, {3728.37009930, 1623.96701935, 6099.14621378}}},
      {"2025-06-24T08:56:12",
       {{2524896.36235891, -6371242.46068505, 497309.92802598}, {4083.16801025, 2100.60395027, 6080.61379233}},
       {{2524896.36235891, -6371242.46068505, 497309.92802598}, {3618.56967374, 1916.48560019, 6080.61379233}}},
      {"2025-06-24T09:06:40",
       {{-31116832.85041035, -28543058.72227244, -1735357.0244879}, {2071.88355398, -2234.19021641, -385.45588635}},
       {{-31116832.85041035, -28543058.72227244, -1735357.0244879}, {-9.50915444, 34.88506505, -385.45588635}}},
      {"2025-06-24T09:07:03",
       {{-31117050.86242437, -28542255.8475248, -1744220.12113877}, {2071.91613484, -2234.19812262, -385.24461063}},
       {{-31117050.86242437, -28542255.8475248, -1744220.12113877}, {-9.41802703, 34.89305653, -385.24461063}}},
      {"2025-06-24T09:07:26",
       {{-31117266.77824242, -28541452.79221216, -1753078.34765178}, {2071.9487332, -2234.20617688, -385.03225827}},
       {{-31117266.77824242, -28541452.79221216, -1753078.34765178}, {-9.32686895, 34.9007471, -385.03225827}}},
  };

  for (const auto& [utc_time, eci_rv, expected_ecef_rv] : test_cases) {
    auto                      tp = string_to_timepoint(utc_time);
    reference_frame_converter converter(tp);
    state_vector              ecef_rv = converter.eci_to_ecef(eci_rv);
    state_vector              rv_diff = ecef_rv - expected_ecef_rv;
    pos_error                         = norm(rv_diff.position);
    vel_error                         = norm(rv_diff.velocity);
    ASSERT_NEAR(pos_error, 0.0, pos_tolerance);
    ASSERT_NEAR(vel_error, 0.0, vel_tolerance);
  }
}

TEST(test_converters, ecef_2_eci_2_ecef_test)
{
  double pos_tolerance = 1e-3; // m -> 0.1cm
  double vel_tolerance = 1e-6; // m/s
  double pos_error     = 0;
  double vel_error     = 0;

  std::vector<std::tuple<std::string, state_vector, state_vector>> test_cases = {
      {"2025-06-24T08:49:09",
       {{2388595.31462629, -6443404.17018643, 9540.82104068}, {3798.13162655, 1424.36001124, 6105.64102076}},
       {{2388595.31462629, -6443404.17018643, 9540.82104068}, {4267.99207801, 1598.53913197, 6105.64102076}}},
      {"2025-06-24T08:50:09",
       {{2611463.66175304, -6344787.43848636, 375586.49406261}, {3628.72726962, 1861.91229016, 6091.38301467}},
       {{2611463.66175304, -6344787.43848636, 375586.49406261}, {4091.39647544, 2052.34322739, 6091.38301467}}},
      {"2025-06-24T08:50:39",
       {{2718992.89574091, -6285679.38519413, 558085.41058848}, {3539.39590614, 2078.3264241, 6074.10316297}},
       {{2718992.89574091, -6285679.38519413, 558085.41058848}, {3997.75488466, 2276.59851689, 6074.10316297}}}};

  for (const auto& [utc_time, gold_ecef_rv, expected_eci_rv] : test_cases) {
    auto                      tp = string_to_timepoint(utc_time);
    reference_frame_converter converter(tp);
    state_vector              eci_rv  = converter.ecef_to_eci(gold_ecef_rv);
    state_vector              rv_diff = eci_rv - expected_eci_rv;
    pos_error                         = norm(rv_diff.position);
    vel_error                         = norm(rv_diff.velocity);
    ASSERT_NEAR(pos_error, 0.0, pos_tolerance);
    ASSERT_NEAR(vel_error, 0.0, vel_tolerance);
    // back to ecef
    state_vector ecef_rv = converter.eci_to_ecef(eci_rv);
    rv_diff              = ecef_rv - gold_ecef_rv;
    pos_error            = norm(rv_diff.position);
    vel_error            = norm(rv_diff.velocity);
    ASSERT_NEAR(pos_error, 0.0, pos_tolerance);
    ASSERT_NEAR(vel_error, 0.0, vel_tolerance);
  }
}
