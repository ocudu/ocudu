// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/adt/byte_buffer.h"
#include "ocudu/asn1/rrc_nr/sys_info.h"
#include "ocudu/rrc/rrc_config.h"
#include "ocudu/rrc/rrc_du_factory.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocucp;

namespace {

byte_buffer pack_raw_ref_time_r16(uint32_t days, uint32_t secs, uint16_t ms, uint32_t ten_ns)
{
  asn1::rrc_nr::ref_time_r16_s rrc_ref_time;
  rrc_ref_time.ref_days_r16             = days;
  rrc_ref_time.ref_seconds_r16          = secs;
  rrc_ref_time.ref_milli_seconds_r16    = ms;
  rrc_ref_time.ref_ten_nano_seconds_r16 = ten_ns;

  byte_buffer   buf;
  asn1::bit_ref bref{buf};
  EXPECT_EQ(rrc_ref_time.pack(bref), asn1::OCUDUASN_SUCCESS);
  return buf;
}

} // namespace

class rrc_du_ref_time_r16_test : public ::testing::Test
{
protected:
  std::unique_ptr<rrc_du> rrc = create_rrc_du(rrc_cfg_t{});
};

TEST_F(rrc_du_ref_time_r16_test, local_clock_decodes_as_unix_epoch_relative)
{
  // 20089 days, 0 s, 0 ms, 0 (10 ns) = 1735689600 s since the Unix epoch.
  byte_buffer encoded = pack_raw_ref_time_r16(20089U, 0U, 0U, 0U);

  std::optional<std::chrono::system_clock::time_point> decoded =
      rrc->get_ref_time_r16(encoded, /*is_local_clock=*/true);
  ASSERT_TRUE(decoded.has_value());
  EXPECT_EQ(*decoded, std::chrono::system_clock::time_point{std::chrono::seconds{1735689600LL}});
}

TEST_F(rrc_du_ref_time_r16_test, gps_epoch_time_adds_gps_offset)
{
  // 16432 days, 0 s, 0 ms, 0 (10 ns) = 1419724800 s since the GPS epoch = 1735689600 s since the Unix epoch.
  byte_buffer encoded = pack_raw_ref_time_r16(16432U, 0U, 0U, 0U);

  std::optional<std::chrono::system_clock::time_point> decoded =
      rrc->get_ref_time_r16(encoded, /*is_local_clock=*/false);
  ASSERT_TRUE(decoded.has_value());
  EXPECT_EQ(*decoded, std::chrono::system_clock::time_point{std::chrono::seconds{1735689600LL}});
}

TEST_F(rrc_du_ref_time_r16_test, subsecond_component_is_preserved)
{
  // 123 ms + 45678 * 10 ns = 123456780 ns.
  byte_buffer encoded = pack_raw_ref_time_r16(20089U, 0U, 123U, 45678U);

  std::optional<std::chrono::system_clock::time_point> decoded =
      rrc->get_ref_time_r16(encoded, /*is_local_clock=*/true);
  ASSERT_TRUE(decoded.has_value());
  EXPECT_EQ(*decoded,
            std::chrono::system_clock::time_point{std::chrono::seconds{1735689600LL} +
                                                  std::chrono::nanoseconds{123456780LL}});
}
