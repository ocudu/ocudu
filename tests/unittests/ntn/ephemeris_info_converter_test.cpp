// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/ntn/converters/ephemeris_info_converter.h"
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

std::vector<std::tuple<std::string, state_vector, state_vector, orbital_elements>> test_cases = {
    {"2025-06-24T09:56:06",
     {{426298.38780814, -6858652.52514232, 9543.75174417}, {4047.63303185, 266.96864565, 6105.64101511}},
     {{426298.38780814, -6858652.52514232, 9543.75174417}, {4547.7738715, 298.05481495, 6105.64101511}},
     {6877288.146125763,
      0.0011959644075247937,
      0.9295784053891714,
      4.77342729482894,
      0.8582612758585331,
      5.428463295733399}}, // LEO
    {"2025-06-24T10:05:05",
     {{156461.00865337, -6870106.60784671, 9541.60256166}, {4054.99738691, 107.71034985, 6105.64101926}},
     {{156461.00865337, -6870106.60784671, 9541.60256166}, {4555.97347145, 119.11966676, 6105.64101926}},
     {6877288.14613685,
      0.0011959639962290003,
      0.9295784053900793,
      4.734122790332842,
      0.8582609750543692,
      5.428463205786013}}, // LEO
    {"2025-06-24T10:08:20",
     {{-37503343.02913827, -19400141.56092045, -1761936.81616018}, {2.23911112e-01, 3.61092234e+01, -3.84824389e+02}},
     {{-37503343.02913827, -19400141.56092045, -1761936.81616018}, {1414.90457236, -2698.67773412, -384.82438876}},
     {42265116.092437394,
      0.00025944641683577183,
      0.13244745668928243,
      0.15881196577965917,
      4.623911838194668,
      5.122540519142772}}, // GEO
    {"2025-06-24T10:12:48",
     {{-37875288.29121766, -18663567.84665889, -1761937.13881396}, {0.92949129, 36.09800434, -384.82479912}},
     {{-37875288.29121766, -18663567.84665889, -1761937.13881396}, {1361.89834915, -2725.81163, -384.82479912}},
     {42265116.09214217,
      0.00025944659083169117,
      0.13244758696156164,
      0.1392695527848184,
      4.623909661517107,
      5.122542431510426}}, // GEO
};

TEST(test_converters, ecef_rv_2_oe_test)
{
  double sma_tolerance = 1e-2; // 1cm
  double tolerance     = 1e-6;
  double pos_tolerance = 1e-3; // m -> 0.1cm
  double vel_tolerance = 1e-6; // m/s
  double pos_error     = 0;
  double vel_error     = 0;

  for (const auto& [utc_time, ecef_rv, expected_eci_rv, expected_oe] : test_cases) {
    auto                      tp = string_to_timepoint(utc_time);
    reference_frame_converter ref_frame_converter(tp);
    state_vector              eci_rv = ref_frame_converter.ecef_to_eci(ecef_rv);

    state_vector rv_diff = eci_rv - expected_eci_rv;
    pos_error            = norm(rv_diff.position);
    vel_error            = norm(rv_diff.velocity);
    ASSERT_NEAR(pos_error, 0.0, pos_tolerance);
    ASSERT_NEAR(vel_error, 0.0, vel_tolerance);

    orbital_elements oe = ephemeris_info_converter::eci_to_orbital(eci_rv);
    ASSERT_NEAR(oe.semi_major_axis, expected_oe.semi_major_axis, sma_tolerance);
    ASSERT_NEAR(oe.eccentricity, expected_oe.eccentricity, tolerance);
    ASSERT_NEAR(oe.inclination, expected_oe.inclination, tolerance);
    ASSERT_NEAR(oe.longitude, expected_oe.longitude, tolerance);
    ASSERT_NEAR(oe.periapsis, expected_oe.periapsis, tolerance);
    ASSERT_NEAR(oe.mean_anomaly, expected_oe.mean_anomaly, tolerance);

    state_vector eci_rv_again = ephemeris_info_converter::orbital_to_eci(oe);
    rv_diff                   = eci_rv_again - expected_eci_rv;
    pos_error                 = norm(rv_diff.position);
    vel_error                 = norm(rv_diff.velocity);
    ASSERT_NEAR(pos_error, 0.0, pos_tolerance);
    ASSERT_NEAR(vel_error, 0.0, vel_tolerance);
  }
}

TEST(test_converters, oe_2_ecef_rvs_test)
{
  double sma_tolerance = 1e-2; // 1cm
  double tolerance     = 1e-6;
  double pos_tolerance = 1e-3; // m -> 0.1cm
  double vel_tolerance = 1e-6; // m/s
  double pos_error     = 0;
  double vel_error     = 0;

  for (const auto& [utc_time, expected_ecef_rv, expected_eci_rv, oe_gold] : test_cases) {
    state_vector eci_rv  = ephemeris_info_converter::orbital_to_eci(oe_gold);
    state_vector rv_diff = eci_rv - expected_eci_rv;
    pos_error            = norm(rv_diff.position);
    vel_error            = norm(rv_diff.velocity);
    ASSERT_NEAR(pos_error, 0.0, pos_tolerance);
    ASSERT_NEAR(vel_error, 0.0, vel_tolerance);

    auto                      tp = string_to_timepoint(utc_time);
    reference_frame_converter ref_frame_converter(tp);
    state_vector              ecef_rv = ref_frame_converter.eci_to_ecef(eci_rv);
    rv_diff                           = ecef_rv - expected_ecef_rv;
    pos_error                         = norm(rv_diff.position);
    vel_error                         = norm(rv_diff.velocity);
    ASSERT_NEAR(pos_error, 0.0, pos_tolerance);
    ASSERT_NEAR(vel_error, 0.0, vel_tolerance);

    state_vector eci_rv_again = ref_frame_converter.ecef_to_eci(ecef_rv);
    rv_diff                   = eci_rv_again - expected_eci_rv;
    pos_error                 = norm(rv_diff.position);
    vel_error                 = norm(rv_diff.velocity);
    ASSERT_NEAR(pos_error, 0.0, pos_tolerance);
    ASSERT_NEAR(vel_error, 0.0, vel_tolerance);

    orbital_elements oe = ephemeris_info_converter::eci_to_orbital(eci_rv_again);
    ASSERT_NEAR(oe.semi_major_axis, oe_gold.semi_major_axis, sma_tolerance);
    ASSERT_NEAR(oe.eccentricity, oe_gold.eccentricity, tolerance);
    ASSERT_NEAR(oe.inclination, oe_gold.inclination, tolerance);
    ASSERT_NEAR(oe.longitude, oe_gold.longitude, tolerance);
    ASSERT_NEAR(oe.periapsis, oe_gold.periapsis, tolerance);
    ASSERT_NEAR(oe.mean_anomaly, oe_gold.mean_anomaly, tolerance);
  }
}
