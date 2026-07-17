// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/rrc/ue/rrc_measurement_types_asn1_converters.h"
#include <array>
#include <chrono>
#include <ctime>
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocucp;

// ============================================================================
// Direct ASN1 encoding tests for conditional-trigger event types.
// These tests call cond_trigger_cfg_to_rrc_asn1() directly, without the full
// RRC UE machinery.
// ============================================================================

/// cond_event_a3 encodes a3_offset, hysteresis, and time_to_trigger.
TEST(cond_trigger_asn1, cond_event_a3_encodes_correctly)
{
  rrc_cond_trigger_cfg cfg;
  cfg.rs_type = rrc_nr_rs_type::ssb;

  rrc_meas_trigger_quant offset;
  offset.rsrp = 6;

  cfg.cond_event_id.id                                 = rrc_event_id::event_id_t::a3;
  cfg.cond_event_id.hysteresis                         = 4;
  cfg.cond_event_id.time_to_trigger                    = 80;
  cfg.cond_event_id.meas_trigger_quant_thres_or_offset = offset;

  auto asn1_cfg = cond_trigger_cfg_to_rrc_asn1(cfg);

  ASSERT_EQ(asn1_cfg.cond_event_id.type(),
            asn1::rrc_nr::cond_trigger_cfg_r16_s::cond_event_id_c_::types::cond_event_a3);
  const auto& ev = asn1_cfg.cond_event_id.cond_event_a3();
  EXPECT_EQ(ev.a3_offset.rsrp(), 6);
  EXPECT_EQ(ev.hysteresis, 4);
  EXPECT_EQ(ev.time_to_trigger.to_number(), 80u);
}

/// cond_event_a4 encodes a4_thres_r17, hysteresis_r17, and time_to_trigger_r17.
TEST(cond_trigger_asn1, cond_event_a4_encodes_correctly)
{
  rrc_cond_trigger_cfg cfg;
  cfg.rs_type = rrc_nr_rs_type::ssb;

  rrc_meas_trigger_quant thres;
  thres.rsrp = 110;

  cfg.cond_event_id.id                                 = rrc_event_id::event_id_t::a4;
  cfg.cond_event_id.hysteresis                         = 6;
  cfg.cond_event_id.time_to_trigger                    = 100;
  cfg.cond_event_id.meas_trigger_quant_thres_or_offset = thres;

  auto asn1_cfg = cond_trigger_cfg_to_rrc_asn1(cfg);

  ASSERT_EQ(asn1_cfg.cond_event_id.type(),
            asn1::rrc_nr::cond_trigger_cfg_r16_s::cond_event_id_c_::types::cond_event_a4_r17);
  const auto& ev = asn1_cfg.cond_event_id.cond_event_a4_r17();
  EXPECT_EQ(ev.a4_thres_r17.rsrp(), 110);
  EXPECT_EQ(ev.hysteresis_r17, 6);
  EXPECT_EQ(ev.time_to_trigger_r17.to_number(), 100u);
}

/// cond_event_a5 encodes a5_thres1, a5_thres2, hysteresis, and time_to_trigger.
TEST(cond_trigger_asn1, cond_event_a5_encodes_correctly)
{
  rrc_cond_trigger_cfg cfg;
  cfg.rs_type = rrc_nr_rs_type::ssb;

  rrc_meas_trigger_quant thres1;
  thres1.rsrp = 10;

  rrc_meas_trigger_quant thres2;
  thres2.rsrp = 20;

  cfg.cond_event_id.id                                 = rrc_event_id::event_id_t::a5;
  cfg.cond_event_id.hysteresis                         = 5;
  cfg.cond_event_id.time_to_trigger                    = 160;
  cfg.cond_event_id.meas_trigger_quant_thres_or_offset = thres1;
  cfg.cond_event_id.meas_trigger_quant_thres_2         = thres2;

  auto asn1_cfg = cond_trigger_cfg_to_rrc_asn1(cfg);

  ASSERT_EQ(asn1_cfg.cond_event_id.type(),
            asn1::rrc_nr::cond_trigger_cfg_r16_s::cond_event_id_c_::types::cond_event_a5);
  const auto& ev = asn1_cfg.cond_event_id.cond_event_a5();
  EXPECT_EQ(ev.a5_thres1.rsrp(), 10);
  EXPECT_EQ(ev.a5_thres2.rsrp(), 20);
  EXPECT_EQ(ev.hysteresis, 5);
  EXPECT_EQ(ev.time_to_trigger.to_number(), 160u);
}

/// cond_event_d1 encodes distance thresholds (50 m steps), ref locations (6 bytes each),
/// hysteresis (10 m steps), and time_to_trigger.
TEST(cond_trigger_asn1, cond_event_d1_encodes_correctly)
{
  rrc_cond_trigger_cfg cfg;
  cfg.rs_type = rrc_nr_rs_type::ssb;

  cfg.cond_event_id.id                        = rrc_event_id::event_id_t::d1;
  cfg.cond_event_id.distance_thresh_from_ref1 = 5000; // 5000 m / 50 = 100 ASN1 steps
  cfg.cond_event_id.distance_thresh_from_ref2 = 3000; // 3000 m / 50 = 60  ASN1 steps
  cfg.cond_event_id.ref_location1             = reference_location{48.135, 11.582};
  cfg.cond_event_id.ref_location2             = reference_location{48.200, 11.650};
  cfg.cond_event_id.hysteresis_location       = 100; // 100 m / 10 = 10 ASN1 steps
  cfg.cond_event_id.time_to_trigger           = 100;

  auto asn1_cfg = cond_trigger_cfg_to_rrc_asn1(cfg);

  ASSERT_EQ(asn1_cfg.cond_event_id.type(),
            asn1::rrc_nr::cond_trigger_cfg_r16_s::cond_event_id_c_::types::cond_event_d1_r17);
  const auto& ev = asn1_cfg.cond_event_id.cond_event_d1_r17();
  EXPECT_EQ(ev.distance_thresh_from_ref1_r17, 100);
  EXPECT_EQ(ev.distance_thresh_from_ref2_r17, 60);
  EXPECT_EQ(ev.ref_location1_r17.length(), 6u);
  EXPECT_EQ(ev.ref_location2_r17.length(), 6u);
  EXPECT_EQ(ev.hysteresis_location_r17, 10);
  EXPECT_EQ(ev.time_to_trigger_r17.to_number(), 100u);
}

/// Verifies the Ellipsoid-Point byte encoding used by cond_trigger_cfg_to_rrc_asn1():
///   lat_enc = floor(|lat| * 8388607 / 90) clamped to [0..8388607], unsigned 23-bit
///   lon_raw = floor( lon  * 16777215 / 360) clamped to [-8388608..8388607]
///   lon_enc = lon_raw - (-8388608) = lon_raw + 8388608, packed on 24 bits
/// Layout (6 bytes, MSB first): [1-bit sign][23-bit lat][24-bit lon_enc]
TEST(cond_trigger_asn1, cond_event_d1_ref_location_bytes)
{
  struct test_vector {
    reference_location     loc;
    std::array<uint8_t, 6> expected;
    const char*            label;
  };

  // clang-format off
  const test_vector vectors[] = {
      // Equator / Greenwich meridian: lon=0 maps to lon_enc=8388608 (0x800000).
      {{  0.0,    0.0}, {0x00, 0x00, 0x00, 0x80, 0x00, 0x00}, "equator/Greenwich"},
      // North Pole: lat=0x7fffff, lon=0 -> lon_enc=0x800000.
      {{ 90.0,    0.0}, {0x7f, 0xff, 0xff, 0x80, 0x00, 0x00}, "North Pole"},
      // South Pole: sign bit=1, lat=0x7fffff, lon=0 -> lon_enc=0x800000.
      {{-90.0,    0.0}, {0xff, 0xff, 0xff, 0x80, 0x00, 0x00}, "South Pole"},
      // lon=-180 deg: lon_raw=-8388608 -> lon_enc=0x000000.
      {{  0.0, -180.0}, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, "lon=-180"},
      // lat=45, lon=90: lat_enc=floor(45*8388607/90)=0x3fffff,
      //                 lon_raw=floor(90*16777215/360)=0x3fffff, lon_enc=0xbfffff.
      {{ 45.0,   90.0}, {0x3f, 0xff, 0xff, 0xbf, 0xff, 0xff}, "lat=45 lon=90"},
  };
  // clang-format on

  for (const auto& v : vectors) {
    rrc_cond_trigger_cfg cfg;
    cfg.rs_type                                 = rrc_nr_rs_type::ssb;
    cfg.cond_event_id.id                        = rrc_event_id::event_id_t::d1;
    cfg.cond_event_id.distance_thresh_from_ref1 = 5000;
    cfg.cond_event_id.distance_thresh_from_ref2 = 3000;
    cfg.cond_event_id.ref_location1             = v.loc;
    cfg.cond_event_id.ref_location2             = v.loc;
    cfg.cond_event_id.hysteresis_location       = 0;
    cfg.cond_event_id.time_to_trigger           = 0;

    auto        asn1_cfg = cond_trigger_cfg_to_rrc_asn1(cfg);
    const auto& ev       = asn1_cfg.cond_event_id.cond_event_d1_r17();

    std::vector<uint8_t> actual(ev.ref_location1_r17.begin(), ev.ref_location1_r17.end());
    EXPECT_EQ(actual, std::vector<uint8_t>(v.expected.begin(), v.expected.end())) << v.label;
  }
}

/// cond_event_t1 encodes t1_thres_r17 (10 ms units since 1900) and dur_r17 (100 ms steps).
TEST(cond_trigger_asn1, cond_event_t1_encodes_correctly)
{
  // 2025-01-01T00:00:00 UTC = 1735689600 seconds since Unix epoch.
  constexpr time_t   t_unix          = 1735689600;
  constexpr int64_t  ms_1970         = static_cast<int64_t>(t_unix) * 1000LL;
  constexpr int64_t  ms_1900_to_1970 = 2208988800LL * 1000LL;
  constexpr uint64_t expected_t1     = static_cast<uint64_t>((ms_1970 + ms_1900_to_1970) / 10);

  rrc_cond_trigger_cfg cfg;
  cfg.rs_type = rrc_nr_rs_type::ssb;

  cfg.cond_event_id.id       = rrc_event_id::event_id_t::t1;
  cfg.cond_event_id.t1_thres = std::chrono::system_clock::from_time_t(t_unix);
  cfg.cond_event_id.duration = 500; // 500 ms / 100 = 5 ASN1 steps

  auto asn1_cfg = cond_trigger_cfg_to_rrc_asn1(cfg);

  ASSERT_EQ(asn1_cfg.cond_event_id.type(),
            asn1::rrc_nr::cond_trigger_cfg_r16_s::cond_event_id_c_::types::cond_event_t1_r17);
  const auto& ev = asn1_cfg.cond_event_id.cond_event_t1_r17();
  EXPECT_EQ(ev.t1_thres_r17, expected_t1);
  EXPECT_EQ(ev.dur_r17, 5);
}

/// cond_event_d2 encodes distance thresholds (50 m steps), hysteresis (10 m steps),
/// and time_to_trigger.
TEST(cond_trigger_asn1, cond_event_d2_encodes_correctly)
{
  rrc_cond_trigger_cfg cfg;
  cfg.rs_type = rrc_nr_rs_type::ssb;

  cfg.cond_event_id.id                        = rrc_event_id::event_id_t::d2;
  cfg.cond_event_id.distance_thresh_from_ref1 = 10000; // 10000 m / 50 = 200 ASN1 steps
  cfg.cond_event_id.distance_thresh_from_ref2 = 8000;  // 8000  m / 50 = 160 ASN1 steps
  cfg.cond_event_id.hysteresis_location       = 200;   // 200   m / 10 = 20  ASN1 steps
  cfg.cond_event_id.time_to_trigger           = 160;

  auto asn1_cfg = cond_trigger_cfg_to_rrc_asn1(cfg);

  ASSERT_EQ(asn1_cfg.cond_event_id.type(),
            asn1::rrc_nr::cond_trigger_cfg_r16_s::cond_event_id_c_::types::cond_event_d2_r18);
  const auto& ev = asn1_cfg.cond_event_id.cond_event_d2_r18();
  EXPECT_EQ(ev.distance_thresh_from_ref1_r18, 200u);
  EXPECT_EQ(ev.distance_thresh_from_ref2_r18, 160u);
  EXPECT_EQ(ev.hysteresis_location_r18, 20);
  EXPECT_EQ(ev.time_to_trigger_r18.to_number(), 160u);
}
