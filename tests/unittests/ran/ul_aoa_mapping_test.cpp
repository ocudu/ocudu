// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/ran/positioning/ul_aoa_mapping.h"
#include <gtest/gtest.h>

using namespace ocudu;

/// The azimuth reported value counts 0.1 degree steps from -180 degrees, as per TS 38.133, Table 13.4.1-1.
TEST(ul_aoa_mapping_test, azimuth_maps_to_the_interval_starting_at_minus_180_degrees)
{
  EXPECT_EQ(0, azimuth_aoa_to_reported_value(-180.0F));
  EXPECT_EQ(0, azimuth_aoa_to_reported_value(-179.95F));
  EXPECT_EQ(1, azimuth_aoa_to_reported_value(-179.9F));
  EXPECT_EQ(1799, azimuth_aoa_to_reported_value(-0.05F));
  EXPECT_EQ(1800, azimuth_aoa_to_reported_value(0.0F));
  EXPECT_EQ(3034, azimuth_aoa_to_reported_value(123.4F));
  EXPECT_EQ(3599, azimuth_aoa_to_reported_value(179.95F));
  EXPECT_EQ(3599, azimuth_aoa_to_reported_value(std::nextafter(180.0F, 0.0F)));
}

#ifdef ASSERTS_ENABLED
/// Azimuth angles outside [-180, 180) degrees break the precondition.
TEST(ul_aoa_mapping_test, azimuth_outside_the_range_asserts)
{
  ASSERT_DEATH(azimuth_aoa_to_reported_value(180.0F), R"(outside \[-180, 180\))");
  ASSERT_DEATH(azimuth_aoa_to_reported_value(-180.05F), R"(outside \[-180, 180\))");
  ASSERT_DEATH(azimuth_aoa_to_reported_value(270.0F), R"(outside \[-180, 180\))");
}
#endif // ASSERTS_ENABLED

/// The zenith reported value counts 0.1 degree steps from 0 degrees, as per TS 38.133, Table 13.4.1-2.
TEST(ul_aoa_mapping_test, zenith_maps_to_the_interval_starting_at_0_degrees)
{
  EXPECT_EQ(0, zenith_aoa_to_reported_value(0.0F));
  EXPECT_EQ(0, zenith_aoa_to_reported_value(0.05F));
  EXPECT_EQ(567, zenith_aoa_to_reported_value(56.7F));
  EXPECT_EQ(1799, zenith_aoa_to_reported_value(179.95F));
  EXPECT_EQ(1799, zenith_aoa_to_reported_value(std::nextafter(180.0F, 0.0F)));
}

#ifdef ASSERTS_ENABLED
/// Zenith angles outside [0, 180) degrees break the precondition.
TEST(ul_aoa_mapping_test, zenith_outside_the_range_asserts)
{
  ASSERT_DEATH(zenith_aoa_to_reported_value(-5.0F), R"(outside \[0, 180\))");
  ASSERT_DEATH(zenith_aoa_to_reported_value(180.0F), R"(outside \[0, 180\))");
  ASSERT_DEATH(zenith_aoa_to_reported_value(200.0F), R"(outside \[0, 180\))");
}
#endif // ASSERTS_ENABLED
