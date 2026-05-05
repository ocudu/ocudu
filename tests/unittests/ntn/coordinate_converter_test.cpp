// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/ntn/converters/coordinate_converter.h"
#include "lib/ntn/coordinates_types.h"
#include "ocudu/support/test_utils.h"
#include <gtest/gtest.h>
#include <vector>

using namespace ocudu;
using namespace ocudu_ntn;

TEST(test_converters, geodetic_2_ecef_test)
{
  double                                                     tolerance  = 1e-3; // 0.1cm
  std::vector<std::pair<geodetic_coordinates, state_vector>> test_cases = {
      {{52.52, 13.405, 34}, {{3783265.1801, 901649.7966, 5038246.0811}, {0, 0, 0}}},     // Berlin
      {{52.2297, 21.0122, 100}, {{3654528.3007, 1403734.875, 5018577.4313}, {0, 0, 0}}}, // Warsaw
      {{41.3874, 2.1686, 12}, {{4788832.1502, 181340.1164, 4194805.8875}, {0, 0, 0}}}    // Barcelona
  };

  for (const auto& [geodetic_coords, expected_ecef_coords] : test_cases) {
    state_vector ecef_rv = coordinate_converter::geodetic_to_ecef(geodetic_coords);
    ASSERT_NEAR(ecef_rv.position.x, expected_ecef_coords.position.x, tolerance);
    ASSERT_NEAR(ecef_rv.position.y, expected_ecef_coords.position.y, tolerance);
    ASSERT_NEAR(ecef_rv.position.z, expected_ecef_coords.position.z, tolerance);
  }
}

TEST(test_converters, ecef_2_geodetic_test)
{
  double                                                     tolerance  = 1e-2; // 0.01 deg
  std::vector<std::pair<state_vector, geodetic_coordinates>> test_cases = {
      {{{1334000.5447, -4654052.1292, 4138306.7614}, {0, 0, 0}}, {40.7128, -74.0060, 10}},   // New York
      {{{2755226.9798, -4475424.0484, -3601780.7280}, {0, 0, 0}}, {-34.6037, -58.3821, 25}}, // Buenos Aires
      {{{-4646053.4551, 2553207.5419, -3534374.0598}, {0, 0, 0}}, {-33.8688, 151.2093, 3}}   // Sydney
  };

  for (const auto& [ecef_rv, expected_geo_coords] : test_cases) {
    geodetic_coordinates geo_coords = coordinate_converter::ecef_to_geodetic(ecef_rv);
    ASSERT_NEAR(geo_coords.latitude_deg, expected_geo_coords.latitude_deg, tolerance);
    ASSERT_NEAR(geo_coords.longitude_deg, expected_geo_coords.longitude_deg, tolerance);
    ASSERT_NEAR(geo_coords.altitude_m, expected_geo_coords.altitude_m, tolerance);
  }
}

TEST(test_converters, geodetic_2_ecef_2_geodetic_test)
{
  double                                                     tolerance  = 1e-3; // 0.1cm
  std::vector<std::pair<geodetic_coordinates, state_vector>> test_cases = {
      {{52.52, 13.405, 34}, {{3783265.1801, 901649.7966, 5038246.0811}, {0, 0, 0}}},         // Berlin
      {{52.2297, 21.0122, 100}, {{3654528.3007, 1403734.875, 5018577.4313}, {0, 0, 0}}},     // Warsaw
      {{41.3874, 2.1686, 12}, {{4788832.1502, 181340.1164, 4194805.8875}, {0, 0, 0}}},       // Barcelona
      {{40.7128, -74.0060, 10}, {{1334000.5447, -4654052.1292, 4138306.7614}, {0, 0, 0}}},   // New York
      {{-34.6037, -58.3821, 25}, {{2755226.9798, -4475424.0484, -3601780.7280}, {0, 0, 0}}}, // Buenos Aires
      {{-33.8688, 151.2093, 3}, {{-4646053.4551, 2553207.5419, -3534374.0598}, {0, 0, 0}}}   // Sydney
  };

  for (const auto& [gold_geo_coords, expected_ecef_coords] : test_cases) {
    state_vector ecef_rv = coordinate_converter::geodetic_to_ecef(gold_geo_coords);
    ASSERT_NEAR(ecef_rv.position.x, expected_ecef_coords.position.x, tolerance);
    ASSERT_NEAR(ecef_rv.position.y, expected_ecef_coords.position.y, tolerance);
    ASSERT_NEAR(ecef_rv.position.z, expected_ecef_coords.position.z, tolerance);
    // back to geodetic
    geodetic_coordinates geo_coords = coordinate_converter::ecef_to_geodetic(ecef_rv);
    ASSERT_NEAR(geo_coords.latitude_deg, gold_geo_coords.latitude_deg, tolerance);
    ASSERT_NEAR(geo_coords.longitude_deg, gold_geo_coords.longitude_deg, tolerance);
    ASSERT_NEAR(geo_coords.altitude_m, gold_geo_coords.altitude_m, tolerance);
  }
}
