// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "apps/units/o_cu_cp/cu_cp/cu_cp_unit_config.h"
#include "apps/units/o_cu_cp/cu_cp/cu_cp_unit_config_validator.h"
#include "tests/ocudu_test_requirements.h"
#include <gtest/gtest.h>

using namespace ocudu;

namespace {

/// Builds a report configuration for a signal-level event, carrying the parameters that event takes.
cu_cp_unit_report_config make_signal_level_report_config(ocucp::rrc_event_id::event_id_t event,
                                                         const std::string&              report_type)
{
  cu_cp_unit_report_config cfg;
  cfg.report_cfg_id               = 1;
  cfg.report_type                 = report_type;
  cfg.report_interval_ms          = 1024;
  cfg.event_triggered_report_type = event;
  cfg.meas_trigger_quantity       = "rsrp";
  cfg.hysteresis_db               = 0;
  cfg.time_to_trigger_ms          = 100;

  if (event == ocucp::rrc_event_id::event_id_t::a3 || event == ocucp::rrc_event_id::event_id_t::a6) {
    cfg.meas_trigger_quantity_offset_db = 3;
  } else {
    cfg.meas_trigger_quantity_threshold_db = -100;
  }
  if (event == ocucp::rrc_event_id::event_id_t::a5) {
    cfg.meas_trigger_quantity_threshold_2_db = -90;
  }
  return cfg;
}

/// Builds a report configuration for a distance event, carrying the parameters that event takes.
cu_cp_unit_report_config make_distance_report_config(ocucp::rrc_event_id::event_id_t event,
                                                     const std::string&              report_type)
{
  cu_cp_unit_report_config cfg;
  cfg.report_cfg_id                = 1;
  cfg.report_type                  = report_type;
  cfg.report_interval_ms           = 1024;
  cfg.event_triggered_report_type  = event;
  cfg.distance_thresh_from_ref1_km = 5.0;
  cfg.distance_thresh_from_ref2_km = 3.0;
  cfg.hysteresis_location_km       = 0.1;
  cfg.time_to_trigger_ms           = 100;
  if (event == ocucp::rrc_event_id::event_id_t::d1) {
    cfg.ref_location1 = reference_location{48.135, 11.582};
    cfg.ref_location2 = reference_location{48.200, 11.650};
  }
  return cfg;
}

/// Validates a configuration holding a single report configuration.
bool validate(const cu_cp_unit_report_config& report_cfg)
{
  cu_cp_unit_config cfg;
  cfg.mobility_config.report_configs.push_back(report_cfg);
  return validate_cu_cp_unit_config(cfg);
}

} // namespace

/// TS 38.331 sec. 6.3.2 gives CondTriggerConfig condEventA3, A4, A5, D1, D2 and T1 alone.
TEST(cu_cp_unit_config_validator_test, conditional_trigger_accepts_the_events_the_spec_defines_for_it)
{
  OCUDU_TEST_REQUIREMENTS("CU-NTN-MOB-3");

  for (auto event : {ocucp::rrc_event_id::event_id_t::a3,
                     ocucp::rrc_event_id::event_id_t::a4,
                     ocucp::rrc_event_id::event_id_t::a5}) {
    EXPECT_TRUE(validate(make_signal_level_report_config(event, "cond_trigger"))) << "event " << to_string(event);
  }
  for (auto event : {ocucp::rrc_event_id::event_id_t::d1, ocucp::rrc_event_id::event_id_t::d2}) {
    EXPECT_TRUE(validate(make_distance_report_config(event, "cond_trigger"))) << "event " << to_string(event);
  }
}

TEST(cu_cp_unit_config_validator_test, conditional_trigger_rejects_the_events_the_spec_does_not_define_for_it)
{
  OCUDU_TEST_REQUIREMENTS("CU-NTN-MOB-3");

  for (auto event : {ocucp::rrc_event_id::event_id_t::a1,
                     ocucp::rrc_event_id::event_id_t::a2,
                     ocucp::rrc_event_id::event_id_t::a6}) {
    EXPECT_FALSE(validate(make_signal_level_report_config(event, "cond_trigger"))) << "event " << to_string(event);
  }
}

/// Only D1 is offered as a measurement report; T1 and D2 appear under CondTriggerConfig alone.
TEST(cu_cp_unit_config_validator_test, event_triggered_report_accepts_d1_but_not_the_other_distance_or_time_events)
{
  OCUDU_TEST_REQUIREMENTS("CU-NTN-MOB-2");

  EXPECT_TRUE(validate(make_distance_report_config(ocucp::rrc_event_id::event_id_t::d1, "event_triggered")));
  EXPECT_FALSE(validate(make_distance_report_config(ocucp::rrc_event_id::event_id_t::d2, "event_triggered")));
}

/// TS 38.331 sec. 6.3.2 encodes an event-triggered distance threshold from step 1, so 40 m has no encoding there.
TEST(cu_cp_unit_config_validator_test, an_event_triggered_distance_threshold_below_one_step_is_rejected)
{
  OCUDU_TEST_REQUIREMENTS("CU-NTN-MOB-2");

  cu_cp_unit_report_config cfg = make_distance_report_config(ocucp::rrc_event_id::event_id_t::d1, "event_triggered");
  cfg.distance_thresh_from_ref1_km = 0.04; // 40 m, under one 50 m step
  EXPECT_FALSE(validate(cfg));

  cfg.distance_thresh_from_ref1_km = 0.05; // exactly one step
  EXPECT_TRUE(validate(cfg));
}

/// A conditional trigger encodes the same threshold from step 0, so it reaches one step below a measurement report.
TEST(cu_cp_unit_config_validator_test, a_conditional_distance_threshold_of_zero_is_accepted)
{
  OCUDU_TEST_REQUIREMENTS("CU-NTN-MOB-3");

  cu_cp_unit_report_config cfg     = make_distance_report_config(ocucp::rrc_event_id::event_id_t::d1, "cond_trigger");
  cfg.distance_thresh_from_ref1_km = 0.0;
  cfg.distance_thresh_from_ref2_km = 0.0;
  EXPECT_TRUE(validate(cfg));

  cfg.distance_thresh_from_ref1_km = -0.01;
  EXPECT_FALSE(validate(cfg)) << "a negative distance has no encoding";
}

/// D1 stops at 65525 steps and D2 at 65535, so their upper bounds differ.
TEST(cu_cp_unit_config_validator_test, distance_thresholds_respect_the_per_event_upper_bound)
{
  OCUDU_TEST_REQUIREMENTS("CU-NTN-MOB-2", "CU-NTN-MOB-3");

  cu_cp_unit_report_config d1_cfg = make_distance_report_config(ocucp::rrc_event_id::event_id_t::d1, "cond_trigger");
  d1_cfg.distance_thresh_from_ref1_km = 3276.25;
  EXPECT_TRUE(validate(d1_cfg));
  d1_cfg.distance_thresh_from_ref1_km = 3276.75;
  EXPECT_FALSE(validate(d1_cfg)) << "D1 stops at 65525 steps";

  cu_cp_unit_report_config d2_cfg = make_distance_report_config(ocucp::rrc_event_id::event_id_t::d2, "cond_trigger");
  d2_cfg.distance_thresh_from_ref1_km = 3276.75;
  EXPECT_TRUE(validate(d2_cfg)) << "D2 reaches 65535 steps";
}

/// The location hysteresis is counted in 10 m steps, up to 32768.
TEST(cu_cp_unit_config_validator_test, location_hysteresis_respects_its_upper_bound)
{
  OCUDU_TEST_REQUIREMENTS("CU-NTN-MOB-3");

  cu_cp_unit_report_config cfg = make_distance_report_config(ocucp::rrc_event_id::event_id_t::d1, "cond_trigger");
  cfg.hysteresis_location_km   = 327.68;
  EXPECT_TRUE(validate(cfg));

  cfg.hysteresis_location_km = 327.69;
  EXPECT_FALSE(validate(cfg));
}
