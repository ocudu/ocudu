// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
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
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-MOB-15");

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
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-MOB-15");

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
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-MOB-15");

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
  OCUDU_TEST_REQUIREMENTS("CU-NTN-MOB-3", "MVP-FUNC-MOB-15");

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
  OCUDU_TEST_REQUIREMENTS("CU-NTN-MOB-3", "MVP-FUNC-MOB-15");

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
  OCUDU_TEST_REQUIREMENTS("CU-NTN-MOB-3", "MVP-FUNC-MOB-15");

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
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-MOB-15");

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

  rrc_event_id event_id{};
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

// ============================================================================
// Event-based measurement reporting for events A1-A6 (CU-GEN-6).
// Each event is configured with distinct values and converted into a full reportConfigNR, which is packed and
// unpacked, so that the configuration the UE receives is valid ASN.1, and then checked field by field.
// ============================================================================

namespace {

using asn1_event_type = asn1::rrc_nr::event_trigger_cfg_s::event_id_c_::types;

struct event_a1_a6_test_params {
  rrc_event_id::event_id_t id;
  asn1_event_type          asn1_type;
};

/// Prints the event name in the test name, e.g. a_events/event_trigger_a1_a6_asn1.report_config_packs_and_unpacks/a1.
std::string event_param_name(const ::testing::TestParamInfo<event_a1_a6_test_params>& info)
{
  return to_string(info.param.id);
}

/// Distinct values for every field, so that a field written to the wrong place is caught. Both A5 thresholds use the
/// same quantity, as the event compares the serving and the neighbour cell in the configured trigger quantity.
constexpr uint8_t  thres1_rsrp     = 40;
constexpr uint8_t  thres2_rsrp     = 20;
constexpr uint8_t  offset_rsrp     = 6;
constexpr uint8_t  hysteresis      = 4;
constexpr uint16_t time_to_trigger = 80;

/// Fills the event fields TS 38.331 defines for the given A-event: a threshold for A1/A2/A4, an offset for A3/A6 and
/// two thresholds for A5. useAllowedCellList only exists for A3-A6.
rrc_event_trigger_cfg make_a_event_trigger_cfg(rrc_event_id::event_id_t id)
{
  rrc_event_trigger_cfg cfg = make_event_trigger_cfg();

  cfg.event_id.id              = id;
  cfg.event_id.report_on_leave = true;
  cfg.event_id.hysteresis      = hysteresis;
  cfg.event_id.time_to_trigger = time_to_trigger;

  rrc_meas_trigger_quant thres_or_offset;
  if (id == rrc_event_id::event_id_t::a3 or id == rrc_event_id::event_id_t::a6) {
    thres_or_offset.rsrp = offset_rsrp;
  } else {
    thres_or_offset.rsrp = thres1_rsrp;
  }
  cfg.event_id.meas_trigger_quant_thres_or_offset = thres_or_offset;

  if (id == rrc_event_id::event_id_t::a5) {
    rrc_meas_trigger_quant thres2;
    thres2.rsrp                             = thres2_rsrp;
    cfg.event_id.meas_trigger_quant_thres_2 = thres2;
  }

  if (id != rrc_event_id::event_id_t::a1 and id != rrc_event_id::event_id_t::a2) {
    cfg.event_id.use_allowed_cell_list = true;
  }

  return cfg;
}

/// Checks the fields that all A-events share.
template <typename Event>
void check_common_event_fields(const Event& ev)
{
  EXPECT_TRUE(ev.report_on_leave);
  EXPECT_EQ(ev.hysteresis, hysteresis);
  EXPECT_EQ(ev.time_to_trigger.to_number(), time_to_trigger);
}

/// Checks the event-specific fields of the encoded event against the values set by make_a_event_trigger_cfg().
void check_event_fields(const asn1::rrc_nr::event_trigger_cfg_s::event_id_c_& ev, rrc_event_id::event_id_t id)
{
  switch (id) {
    case rrc_event_id::event_id_t::a1:
      check_common_event_fields(ev.event_a1());
      EXPECT_EQ(ev.event_a1().a1_thres.rsrp(), thres1_rsrp);
      break;
    case rrc_event_id::event_id_t::a2:
      check_common_event_fields(ev.event_a2());
      EXPECT_EQ(ev.event_a2().a2_thres.rsrp(), thres1_rsrp);
      break;
    case rrc_event_id::event_id_t::a3:
      check_common_event_fields(ev.event_a3());
      EXPECT_EQ(ev.event_a3().a3_offset.rsrp(), offset_rsrp);
      EXPECT_TRUE(ev.event_a3().use_allowed_cell_list);
      break;
    case rrc_event_id::event_id_t::a4:
      check_common_event_fields(ev.event_a4());
      EXPECT_EQ(ev.event_a4().a4_thres.rsrp(), thres1_rsrp);
      EXPECT_TRUE(ev.event_a4().use_allowed_cell_list);
      break;
    case rrc_event_id::event_id_t::a5:
      check_common_event_fields(ev.event_a5());
      EXPECT_EQ(ev.event_a5().a5_thres1.rsrp(), thres1_rsrp);
      EXPECT_EQ(ev.event_a5().a5_thres2.rsrp(), thres2_rsrp);
      EXPECT_TRUE(ev.event_a5().use_allowed_cell_list);
      break;
    case rrc_event_id::event_id_t::a6:
      check_common_event_fields(ev.event_a6());
      EXPECT_EQ(ev.event_a6().a6_offset.rsrp(), offset_rsrp);
      EXPECT_TRUE(ev.event_a6().use_allowed_cell_list);
      break;
    default:
      FAIL() << "Not an A-event";
  }
}

class event_trigger_a1_a6_asn1 : public ::testing::TestWithParam<event_a1_a6_test_params>
{};

const std::array<event_a1_a6_test_params, 6> a_event_params = {{
    {rrc_event_id::event_id_t::a1, asn1_event_type::event_a1},
    {rrc_event_id::event_id_t::a2, asn1_event_type::event_a2},
    {rrc_event_id::event_id_t::a3, asn1_event_type::event_a3},
    {rrc_event_id::event_id_t::a4, asn1_event_type::event_a4},
    {rrc_event_id::event_id_t::a5, asn1_event_type::event_a5},
    {rrc_event_id::event_id_t::a6, asn1_event_type::event_a6},
}};

} // namespace

/// The report configuration of every A-event is encoded as an event-triggered reportConfigNR that is valid ASN.1: it
/// packs, unpacks to the same event with the same parameters and packs back to the same bytes.
TEST_P(event_trigger_a1_a6_asn1, report_config_packs_and_unpacks)
{
  OCUDU_TEST_REQUIREMENTS("CU-GEN-6");

  const event_a1_a6_test_params& params = GetParam();

  const auto asn1_report_cfg = report_cfg_nr_to_rrc_asn1(rrc_report_cfg_nr{make_a_event_trigger_cfg(params.id)});

  byte_buffer   packed;
  asn1::bit_ref bref{packed};
  ASSERT_EQ(asn1_report_cfg.pack(bref), asn1::OCUDUASN_SUCCESS);

  asn1::rrc_nr::report_cfg_nr_s unpacked;
  asn1::cbit_ref                cbref{packed};
  ASSERT_EQ(unpacked.unpack(cbref), asn1::OCUDUASN_SUCCESS);

  ASSERT_EQ(unpacked.report_type.type(), asn1::rrc_nr::report_cfg_nr_s::report_type_c_::types::event_triggered);
  const auto& ev = unpacked.report_type.event_triggered().event_id;
  ASSERT_EQ(ev.type(), params.asn1_type);
  check_event_fields(ev, params.id);

  byte_buffer   repacked;
  asn1::bit_ref rebref{repacked};
  ASSERT_EQ(unpacked.pack(rebref), asn1::OCUDUASN_SUCCESS);
  EXPECT_EQ(packed, repacked);
}

INSTANTIATE_TEST_SUITE_P(a_events, event_trigger_a1_a6_asn1, ::testing::ValuesIn(a_event_params), event_param_name);
