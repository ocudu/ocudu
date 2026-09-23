// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "tests/ocudu_test_requirements.h"
#include "ocudu/lpp/reference_location.h"
#include <gtest/gtest.h>

using namespace ocudu;

namespace {

/// One LSB of the TS 23.032 coding, in degrees.
constexpr double lat_lsb = 90.0 / 8388608.0;   // 2^23
constexpr double lon_lsb = 360.0 / 16777216.0; // 2^24

reference_location round_trip(double latitude, double longitude)
{
  const std::optional<reference_location> decoded =
      lpp::unpack_reference_location(lpp::pack_reference_location({latitude, longitude}));
  EXPECT_TRUE(decoded.has_value()) << "Failed to decode a location this same module packed";
  return decoded.value_or(reference_location{});
}

/// The coding floors, so a decoded coordinate names the lower edge of its quantisation cell: it never exceeds the
/// value that was encoded, and is never more than one LSB below it.
void expect_floored_to(double decoded, double original, double lsb)
{
  EXPECT_LE(decoded, original) << "The coding floors, so the decoded value must not exceed the original";
  EXPECT_LT(original - decoded, lsb) << "The decoded value is more than one LSB below the original";
}

} // namespace

TEST(reference_location_test, northern_eastern_position_survives_a_round_trip)
{
  OCUDU_TEST_REQUIREMENTS("CU-NTN-LOC-2");

  const reference_location decoded = round_trip(52.5, 13.4);

  expect_floored_to(decoded.latitude, 52.5, lat_lsb);
  expect_floored_to(decoded.longitude, 13.4, lon_lsb);
}

TEST(reference_location_test, southern_western_position_survives_a_round_trip)
{
  OCUDU_TEST_REQUIREMENTS("CU-NTN-LOC-2");

  const reference_location decoded = round_trip(-33.87, -70.66);

  // The latitude sign is coded separately from the magnitude, so flooring applies to the magnitude.
  expect_floored_to(-decoded.latitude, 33.87, lat_lsb);
  expect_floored_to(decoded.longitude, -70.66, lon_lsb);
}

TEST(reference_location_test, equator_and_prime_meridian_decode_exactly)
{
  OCUDU_TEST_REQUIREMENTS("CU-NTN-LOC-2");

  const reference_location decoded = round_trip(0.0, 0.0);

  EXPECT_DOUBLE_EQ(decoded.latitude, 0.0);
  EXPECT_DOUBLE_EQ(decoded.longitude, 0.0);
}

TEST(reference_location_test, range_extremes_stay_within_the_encodable_range)
{
  OCUDU_TEST_REQUIREMENTS("CU-NTN-LOC-2");

  // degreesLatitude is 23-bit unsigned, so the code for 90 degrees folds into the maximum one and decodes to exactly
  // one LSB below the pole. The sign is coded separately, so both poles fold the same way.
  EXPECT_DOUBLE_EQ(round_trip(90.0, 0.0).latitude, 90.0 - lat_lsb);
  EXPECT_DOUBLE_EQ(round_trip(-90.0, 0.0).latitude, -(90.0 - lat_lsb));

  // degreesLongitude is 24-bit two's complement, so -180 is representable exactly but +180 folds like the poles.
  EXPECT_DOUBLE_EQ(round_trip(0.0, -180.0).longitude, -180.0);
  EXPECT_DOUBLE_EQ(round_trip(0.0, 180.0).longitude, 180.0 - lon_lsb);
}

TEST(reference_location_test, a_truncated_buffer_is_rejected)
{
  OCUDU_TEST_REQUIREMENTS("CU-NTN-LOC-2");

  byte_buffer packed = lpp::pack_reference_location({52.5, 13.4});
  ASSERT_EQ(packed.length(), 6) << "An Ellipsoid-Point is 1 sign bit plus 23 and 24 coordinate bits";

  // Drop the last octet, leaving the longitude short of its 24 bits.
  packed.trim_tail(1);
  EXPECT_FALSE(lpp::unpack_reference_location(packed).has_value());
}

TEST(reference_location_test, an_oversized_buffer_is_rejected)
{
  OCUDU_TEST_REQUIREMENTS("CU-NTN-LOC-2");

  // Decoding the first 48 bits of a longer buffer would yield a plausible but wrong position.
  byte_buffer packed = lpp::pack_reference_location({52.5, 13.4});
  ASSERT_TRUE(packed.append(0x00));

  EXPECT_FALSE(lpp::unpack_reference_location(packed).has_value());
}

TEST(reference_location_test, an_empty_buffer_is_rejected)
{
  OCUDU_TEST_REQUIREMENTS("CU-NTN-LOC-2");

  EXPECT_FALSE(lpp::unpack_reference_location(byte_buffer{}).has_value());
}
