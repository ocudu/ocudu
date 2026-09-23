// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/rrc/ue/rrc_measurement_types_asn1_converters.h"
#include "tests/ocudu_test_requirements.h"
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
  OCUDU_TEST_REQUIREMENTS("CU-NTN-MOB-3");

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

/// Verifies the TS 23.032 sec. 6.1 Ellipsoid-Point byte encoding used by cond_trigger_cfg_to_rrc_asn1():
///   lat_enc = floor(|lat| * 2^23 / 90) clamped to [0..8388607], unsigned 23-bit
///   lon_raw = floor( lon  * 2^24 / 360) clamped to [-8388608..8388607]
///   lon_enc = lon_raw - (-8388608) = lon_raw + 8388608, packed on 24 bits
/// Layout (6 bytes, MSB first): [1-bit sign][23-bit lat][24-bit lon_enc]
TEST(cond_trigger_asn1, cond_event_d1_ref_location_bytes)
{
  OCUDU_TEST_REQUIREMENTS("CU-NTN-MOB-3");

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
      // lat=45, lon=90 (exact mid-scale): lat_enc=floor(45*2^23/90)=0x400000,
      //                 lon_raw=floor(90*2^24/360)=0x400000, lon_enc=0xc00000.
      {{ 45.0,   90.0}, {0x40, 0x00, 0x00, 0xc0, 0x00, 0x00}, "lat=45 lon=90"},
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
  OCUDU_TEST_REQUIREMENTS("CU-NTN-MOB-3");

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

// ============================================================================
// Event D1 as a measurement report trigger. TS 38.331 offers eventD1 under
// EventTriggerConfig as well as CondTriggerConfig.
// ============================================================================

/// Builds a report configuration with every field set: rrc_event_trigger_cfg has no default member initialisers and
/// the converter reads the reporting fields whatever the event is.
static rrc_event_trigger_cfg make_event_trigger_cfg()
{
  return rrc_event_trigger_cfg{.report_add_neigh_meas_present = true,
                               .event_id                      = {},
                               .rs_type                       = rrc_nr_rs_type::ssb,
                               .report_interv                 = 1024,
                               .report_amount                 = -1,
                               .report_quant_cell     = rrc_meas_report_quant{.rsrp = true, .rsrq = true, .sinr = true},
                               .max_report_cells      = 4,
                               .report_quant_rs_idxes = std::nullopt,
                               .max_nrof_rs_idxes_to_report = std::nullopt,
                               .include_beam_meass          = true,
                               .t312                        = std::nullopt};
}

/// event_d1 encodes the same distance fields as its conditional counterpart, plus report_on_leave.
TEST(event_trigger_asn1, event_d1_encodes_correctly)
{
  OCUDU_TEST_REQUIREMENTS("CU-NTN-MOB-2");

  rrc_event_trigger_cfg cfg = make_event_trigger_cfg();

  cfg.event_id.id                        = rrc_event_id::event_id_t::d1;
  cfg.event_id.report_on_leave           = true;
  cfg.event_id.distance_thresh_from_ref1 = 5000; // 5000 m / 50 = 100 ASN1 steps
  cfg.event_id.distance_thresh_from_ref2 = 3000; // 3000 m / 50 = 60  ASN1 steps
  cfg.event_id.ref_location1             = reference_location{48.135, 11.582};
  cfg.event_id.ref_location2             = reference_location{48.200, 11.650};
  cfg.event_id.hysteresis_location       = 100; // 100 m / 10 = 10 ASN1 steps
  cfg.event_id.time_to_trigger           = 100;

  auto asn1_cfg = event_triggered_report_cfg_to_rrc_asn1(cfg);

  ASSERT_EQ(asn1_cfg.event_id.type(), asn1::rrc_nr::event_trigger_cfg_s::event_id_c_::types::event_d1_r17);
  const auto& ev = asn1_cfg.event_id.event_d1_r17();
  EXPECT_EQ(ev.distance_thresh_from_ref1_r17, 100);
  EXPECT_EQ(ev.distance_thresh_from_ref2_r17, 60);
  EXPECT_EQ(ev.ref_location1_r17.length(), 6u);
  EXPECT_EQ(ev.ref_location2_r17.length(), 6u);
  EXPECT_TRUE(ev.report_on_leave_r17);
  EXPECT_EQ(ev.hysteresis_location_r17, 10);
  EXPECT_EQ(ev.time_to_trigger_r17.to_number(), 100u);
}

/// The same event encodes identically whether it triggers a report or a conditional handover, so a configuration can
/// be moved between the two without changing what the UE is asked to measure.
TEST(event_trigger_asn1, event_d1_matches_its_conditional_counterpart)
{
  OCUDU_TEST_REQUIREMENTS("CU-NTN-MOB-2", "CU-NTN-MOB-3");

  rrc_event_id event_id;
  event_id.id                        = rrc_event_id::event_id_t::d1;
  event_id.distance_thresh_from_ref1 = 5000;
  event_id.distance_thresh_from_ref2 = 3000;
  event_id.ref_location1             = reference_location{48.135, 11.582};
  event_id.ref_location2             = reference_location{48.200, 11.650};
  event_id.hysteresis_location       = 100;
  event_id.time_to_trigger           = 100;

  rrc_event_trigger_cfg report_cfg = make_event_trigger_cfg();
  report_cfg.event_id              = event_id;

  rrc_cond_trigger_cfg cond_cfg;
  cond_cfg.rs_type       = rrc_nr_rs_type::ssb;
  cond_cfg.cond_event_id = event_id;

  // The converters return by value, so the results are held: a reference returned by a member function of a temporary
  // does not extend its lifetime.
  const auto  asn1_report_cfg = event_triggered_report_cfg_to_rrc_asn1(report_cfg);
  const auto  asn1_cond_cfg   = cond_trigger_cfg_to_rrc_asn1(cond_cfg);
  const auto& report_ev       = asn1_report_cfg.event_id.event_d1_r17();
  const auto& cond_ev         = asn1_cond_cfg.cond_event_id.cond_event_d1_r17();

  EXPECT_EQ(report_ev.distance_thresh_from_ref1_r17, cond_ev.distance_thresh_from_ref1_r17);
  EXPECT_EQ(report_ev.distance_thresh_from_ref2_r17, cond_ev.distance_thresh_from_ref2_r17);
  EXPECT_EQ(report_ev.ref_location1_r17.to_string(), cond_ev.ref_location1_r17.to_string());
  EXPECT_EQ(report_ev.ref_location2_r17.to_string(), cond_ev.ref_location2_r17.to_string());
  EXPECT_EQ(report_ev.hysteresis_location_r17, cond_ev.hysteresis_location_r17);
  EXPECT_EQ(report_ev.time_to_trigger_r17.to_number(), cond_ev.time_to_trigger_r17.to_number());
}

/// The A-events of a measurement report fall into three shapes: a threshold (A1/A2/A4), an offset with a cell list
/// (A3/A6), and two thresholds (A5). One of each covers every branch of the converter.
TEST(event_trigger_asn1, event_a1_encodes_its_threshold)
{
  rrc_event_trigger_cfg cfg = make_event_trigger_cfg();

  rrc_meas_trigger_quant thres;
  thres.rsrp = 40;

  cfg.event_id.id                                 = rrc_event_id::event_id_t::a1;
  cfg.event_id.report_on_leave                    = true;
  cfg.event_id.hysteresis                         = 4;
  cfg.event_id.time_to_trigger                    = 80;
  cfg.event_id.meas_trigger_quant_thres_or_offset = thres;

  auto asn1_cfg = event_triggered_report_cfg_to_rrc_asn1(cfg);

  ASSERT_EQ(asn1_cfg.event_id.type(), asn1::rrc_nr::event_trigger_cfg_s::event_id_c_::types::event_a1);
  const auto& ev = asn1_cfg.event_id.event_a1();
  EXPECT_EQ(ev.a1_thres.rsrp(), 40);
  EXPECT_TRUE(ev.report_on_leave);
  EXPECT_EQ(ev.hysteresis, 4);
  EXPECT_EQ(ev.time_to_trigger.to_number(), 80u);
}

TEST(event_trigger_asn1, event_a3_encodes_its_offset_and_cell_list)
{
  rrc_event_trigger_cfg cfg = make_event_trigger_cfg();

  rrc_meas_trigger_quant offset;
  offset.rsrp = 6;

  cfg.event_id.id                                 = rrc_event_id::event_id_t::a3;
  cfg.event_id.hysteresis                         = 4;
  cfg.event_id.time_to_trigger                    = 80;
  cfg.event_id.meas_trigger_quant_thres_or_offset = offset;
  cfg.event_id.use_allowed_cell_list              = true;

  auto asn1_cfg = event_triggered_report_cfg_to_rrc_asn1(cfg);

  ASSERT_EQ(asn1_cfg.event_id.type(), asn1::rrc_nr::event_trigger_cfg_s::event_id_c_::types::event_a3);
  const auto& ev = asn1_cfg.event_id.event_a3();
  EXPECT_EQ(ev.a3_offset.rsrp(), 6);
  EXPECT_TRUE(ev.use_allowed_cell_list);
}

/// An absent use_allowed_cell_list encodes as false rather than leaving the field unset.
TEST(event_trigger_asn1, event_a6_defaults_the_cell_list_to_false)
{
  rrc_event_trigger_cfg cfg = make_event_trigger_cfg();

  rrc_meas_trigger_quant offset;
  offset.rsrp = 6;

  cfg.event_id.id                                 = rrc_event_id::event_id_t::a6;
  cfg.event_id.time_to_trigger                    = 80;
  cfg.event_id.meas_trigger_quant_thres_or_offset = offset;
  cfg.event_id.use_allowed_cell_list              = std::nullopt;

  auto asn1_cfg = event_triggered_report_cfg_to_rrc_asn1(cfg);

  ASSERT_EQ(asn1_cfg.event_id.type(), asn1::rrc_nr::event_trigger_cfg_s::event_id_c_::types::event_a6);
  EXPECT_FALSE(asn1_cfg.event_id.event_a6().use_allowed_cell_list);
}

TEST(event_trigger_asn1, event_a5_encodes_both_thresholds)
{
  rrc_event_trigger_cfg cfg = make_event_trigger_cfg();

  rrc_meas_trigger_quant thres1;
  thres1.rsrp = 40;
  rrc_meas_trigger_quant thres2;
  thres2.rsrq = 20;

  cfg.event_id.id                                 = rrc_event_id::event_id_t::a5;
  cfg.event_id.hysteresis                         = 4;
  cfg.event_id.time_to_trigger                    = 80;
  cfg.event_id.meas_trigger_quant_thres_or_offset = thres1;
  cfg.event_id.meas_trigger_quant_thres_2         = thres2;

  auto asn1_cfg = event_triggered_report_cfg_to_rrc_asn1(cfg);

  ASSERT_EQ(asn1_cfg.event_id.type(), asn1::rrc_nr::event_trigger_cfg_s::event_id_c_::types::event_a5);
  const auto& ev = asn1_cfg.event_id.event_a5();
  EXPECT_EQ(ev.a5_thres1.rsrp(), 40);
  EXPECT_EQ(ev.a5_thres2.rsrq(), 20);
}
