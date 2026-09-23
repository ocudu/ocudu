// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "apps/units/o_cu_cp/cu_cp/cu_cp_config_translators.h"
#include "apps/units/o_cu_cp/cu_cp/cu_cp_unit_config.h"
#include "tests/ocudu_test_requirements.h"
#include <gtest/gtest.h>
#include <variant>

using namespace ocudu;

namespace {

/// Builds a report configuration for event D1, carrying the parameters that event takes and no others.
cu_cp_unit_report_config make_d1_report_config(const std::string& report_type)
{
  cu_cp_unit_report_config cfg;
  cfg.report_cfg_id                = 1;
  cfg.report_type                  = report_type;
  cfg.report_interval_ms           = 1024;
  cfg.event_triggered_report_type  = ocucp::rrc_event_id::event_id_t::d1;
  cfg.distance_thresh_from_ref1_km = 5.0;
  cfg.distance_thresh_from_ref2_km = 3.0;
  cfg.ref_location1                = reference_location{48.135, 11.582};
  cfg.ref_location2                = reference_location{48.200, 11.650};
  cfg.hysteresis_location_km       = 0.1;
  cfg.time_to_trigger_ms           = 100;
  return cfg;
}

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

  // TS 38.331 gives A3 and A6 an offset and the rest a threshold; A5 takes a second one.
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

/// Translates a single report configuration and returns what the CU-CP receives for it.
ocucp::rrc_report_cfg_nr translate(const cu_cp_unit_report_config& report_cfg)
{
  cu_cp_unit_config cfg;
  cfg.mobility_config.report_configs.push_back(report_cfg);

  const ocucp::cu_cp_configuration out = generate_cu_cp_config(cfg);
  EXPECT_EQ(out.mobility.meas_mgr_config.report_config_ids.size(), 1);
  return out.mobility.meas_mgr_config.report_config_ids.begin()->second;
}

/// Checks the distance parameters of event D1, whichever configuration carries it.
void expect_d1_parameters(const ocucp::rrc_event_id& event)
{
  EXPECT_EQ(event.id, ocucp::rrc_event_id::event_id_t::d1);
  ASSERT_TRUE(event.distance_thresh_from_ref1.has_value());
  ASSERT_TRUE(event.distance_thresh_from_ref2.has_value());
  EXPECT_EQ(event.distance_thresh_from_ref1.value(), 5000); // 5 km in meters
  EXPECT_EQ(event.distance_thresh_from_ref2.value(), 3000); // 3 km in meters
  ASSERT_TRUE(event.ref_location1.has_value());
  ASSERT_TRUE(event.ref_location2.has_value());
  EXPECT_DOUBLE_EQ(event.ref_location1->latitude, 48.135);
  EXPECT_DOUBLE_EQ(event.ref_location2->latitude, 48.200);
  ASSERT_TRUE(event.hysteresis_location.has_value());
  EXPECT_EQ(event.hysteresis_location.value(), 100); // 0.1 km in meters
  EXPECT_EQ(event.time_to_trigger, 100);
}

} // namespace

/// A D1 configuration carries no T1 threshold or duration, so requiring them would reject a valid configuration.
TEST(cu_cp_config_translators_test, d1_conditional_trigger_needs_no_time_based_parameters)
{
  OCUDU_TEST_REQUIREMENTS("CU-NTN-MOB-3");

  const ocucp::rrc_report_cfg_nr report_cfg = translate(make_d1_report_config("cond_trigger"));

  ASSERT_TRUE(std::holds_alternative<ocucp::rrc_cond_trigger_cfg>(report_cfg));
  expect_d1_parameters(std::get<ocucp::rrc_cond_trigger_cfg>(report_cfg).cond_event_id);
}

/// TS 38.331 sec. 5.5.4.15 offers event D1 under EventTriggerConfig, so the same configuration asked for as a
/// measurement report reaches the CU-CP as an event trigger carrying the same distances.
TEST(cu_cp_config_translators_test, d1_event_triggered_report_carries_the_distance_parameters)
{
  OCUDU_TEST_REQUIREMENTS("CU-NTN-MOB-2");

  const ocucp::rrc_report_cfg_nr report_cfg = translate(make_d1_report_config("event_triggered"));

  ASSERT_TRUE(std::holds_alternative<ocucp::rrc_event_trigger_cfg>(report_cfg));
  const auto& event_trigger = std::get<ocucp::rrc_event_trigger_cfg>(report_cfg);
  expect_d1_parameters(event_trigger.event_id);
  EXPECT_EQ(event_trigger.report_interv, 1024);
}

/// A signal-level event keeps reaching the CU-CP as an event trigger, with no distance parameters. A3 and A6 take an
/// offset, which TS 38.331 encodes in 0.5 dB steps rather than as an absolute level.
TEST(cu_cp_config_translators_test, a3_event_triggered_report_carries_no_distance_parameters)
{
  cu_cp_unit_report_config cfg;
  cfg.report_cfg_id                   = 1;
  cfg.report_type                     = "event_triggered";
  cfg.report_interval_ms              = 1024;
  cfg.event_triggered_report_type     = ocucp::rrc_event_id::event_id_t::a3;
  cfg.meas_trigger_quantity           = "rsrp";
  cfg.meas_trigger_quantity_offset_db = 3;
  cfg.hysteresis_db                   = 0;
  cfg.time_to_trigger_ms              = 100;

  const ocucp::rrc_report_cfg_nr report_cfg = translate(cfg);

  ASSERT_TRUE(std::holds_alternative<ocucp::rrc_event_trigger_cfg>(report_cfg));
  const auto& event = std::get<ocucp::rrc_event_trigger_cfg>(report_cfg).event_id;
  EXPECT_EQ(event.id, ocucp::rrc_event_id::event_id_t::a3);
  EXPECT_FALSE(event.distance_thresh_from_ref1.has_value());
  EXPECT_FALSE(event.ref_location1.has_value());
  ASSERT_TRUE(event.meas_trigger_quant_thres_or_offset.has_value());
  ASSERT_TRUE(event.meas_trigger_quant_thres_or_offset->rsrp.has_value());
  EXPECT_EQ(event.meas_trigger_quant_thres_or_offset->rsrp.value(), 6); // 3 dB in 0.5 dB steps
  ASSERT_TRUE(event.use_allowed_cell_list.has_value());
}

/// A D2 configuration names no reference locations, so requiring them would reject a valid configuration.
TEST(cu_cp_config_translators_test, d2_conditional_trigger_needs_no_reference_locations)
{
  cu_cp_unit_report_config cfg;
  cfg.report_cfg_id                = 1;
  cfg.report_type                  = "cond_trigger";
  cfg.report_interval_ms           = 1024;
  cfg.event_triggered_report_type  = ocucp::rrc_event_id::event_id_t::d2;
  cfg.distance_thresh_from_ref1_km = 5.0;
  cfg.distance_thresh_from_ref2_km = 3.0;
  cfg.hysteresis_location_km       = 0.1;
  cfg.time_to_trigger_ms           = 100;

  const ocucp::rrc_report_cfg_nr report_cfg = translate(cfg);

  ASSERT_TRUE(std::holds_alternative<ocucp::rrc_cond_trigger_cfg>(report_cfg));
  const auto& event = std::get<ocucp::rrc_cond_trigger_cfg>(report_cfg).cond_event_id;
  EXPECT_EQ(event.id, ocucp::rrc_event_id::event_id_t::d2);
  ASSERT_TRUE(event.distance_thresh_from_ref1.has_value());
  EXPECT_EQ(event.distance_thresh_from_ref1.value(), 5000);
  EXPECT_FALSE(event.ref_location1.has_value());
  EXPECT_FALSE(event.ref_location2.has_value());
}

/// A T1 configuration names no distances, so requiring them would reject a valid configuration.
TEST(cu_cp_config_translators_test, t1_conditional_trigger_needs_no_distance_parameters)
{
  OCUDU_TEST_REQUIREMENTS("CU-NTN-MOB-3");

  cu_cp_unit_report_config cfg;
  cfg.report_cfg_id               = 1;
  cfg.report_type                 = "cond_trigger";
  cfg.report_interval_ms          = 1024;
  cfg.event_triggered_report_type = ocucp::rrc_event_id::event_id_t::t1;
  cfg.t1_thres                    = std::chrono::system_clock::from_time_t(1700000000);
  cfg.duration                    = std::chrono::duration<double>(1.5);

  const ocucp::rrc_report_cfg_nr report_cfg = translate(cfg);

  ASSERT_TRUE(std::holds_alternative<ocucp::rrc_cond_trigger_cfg>(report_cfg));
  const auto& event = std::get<ocucp::rrc_cond_trigger_cfg>(report_cfg).cond_event_id;
  EXPECT_EQ(event.id, ocucp::rrc_event_id::event_id_t::t1);
  ASSERT_TRUE(event.t1_thres.has_value());
  ASSERT_TRUE(event.duration.has_value());
  EXPECT_EQ(event.duration.value(), 1500); // 1.5 s in milliseconds
  EXPECT_FALSE(event.distance_thresh_from_ref1.has_value());
  EXPECT_FALSE(event.hysteresis_location.has_value());
}

/// A signal-level event asked for as a conditional trigger takes the other event builder.
TEST(cu_cp_config_translators_test, a3_conditional_trigger_carries_its_offset)
{
  cu_cp_unit_report_config cfg;
  cfg.report_cfg_id                   = 1;
  cfg.report_type                     = "cond_trigger";
  cfg.report_interval_ms              = 1024;
  cfg.event_triggered_report_type     = ocucp::rrc_event_id::event_id_t::a3;
  cfg.meas_trigger_quantity           = "rsrp";
  cfg.meas_trigger_quantity_offset_db = 3;
  cfg.hysteresis_db                   = 0;
  cfg.time_to_trigger_ms              = 100;

  const ocucp::rrc_report_cfg_nr report_cfg = translate(cfg);

  ASSERT_TRUE(std::holds_alternative<ocucp::rrc_cond_trigger_cfg>(report_cfg));
  const auto& event = std::get<ocucp::rrc_cond_trigger_cfg>(report_cfg).cond_event_id;
  EXPECT_EQ(event.id, ocucp::rrc_event_id::event_id_t::a3);
  ASSERT_TRUE(event.meas_trigger_quant_thres_or_offset.has_value());
  // A conditional event carries no allowed cell list, unlike the same event in a measurement report.
  EXPECT_FALSE(event.use_allowed_cell_list.has_value());
}

/// The remaining report type reaches the CU-CP as a periodical configuration.
TEST(cu_cp_config_translators_test, periodical_report_is_translated)
{
  cu_cp_unit_report_config cfg;
  cfg.report_cfg_id           = 1;
  cfg.report_type             = "periodical";
  cfg.report_interval_ms      = 1024;
  cfg.periodic_ho_rsrp_offset = -1;

  const ocucp::rrc_report_cfg_nr report_cfg = translate(cfg);

  ASSERT_TRUE(std::holds_alternative<ocucp::rrc_periodical_report_cfg>(report_cfg));
  EXPECT_EQ(std::get<ocucp::rrc_periodical_report_cfg>(report_cfg).report_interv, 1024);
}

/// A1 and A2 take a threshold and, carrying no allowed cell list, leave it unset even in a measurement report.
TEST(cu_cp_config_translators_test, a1_event_triggered_report_carries_a_threshold)
{
  const ocucp::rrc_report_cfg_nr report_cfg =
      translate(make_signal_level_report_config(ocucp::rrc_event_id::event_id_t::a1, "event_triggered"));

  ASSERT_TRUE(std::holds_alternative<ocucp::rrc_event_trigger_cfg>(report_cfg));
  const auto& event = std::get<ocucp::rrc_event_trigger_cfg>(report_cfg).event_id;
  EXPECT_EQ(event.id, ocucp::rrc_event_id::event_id_t::a1);
  ASSERT_TRUE(event.meas_trigger_quant_thres_or_offset.has_value());
  ASSERT_TRUE(event.meas_trigger_quant_thres_or_offset->rsrp.has_value());
  EXPECT_EQ(event.meas_trigger_quant_thres_or_offset->rsrp.value(), 56); // -100 dB + 156
  EXPECT_FALSE(event.meas_trigger_quant_thres_2.has_value());
  EXPECT_FALSE(event.use_allowed_cell_list.has_value());
}

/// A4 takes a threshold and does carry an allowed cell list.
TEST(cu_cp_config_translators_test, a4_event_triggered_report_carries_a_threshold_and_a_cell_list)
{
  const ocucp::rrc_report_cfg_nr report_cfg =
      translate(make_signal_level_report_config(ocucp::rrc_event_id::event_id_t::a4, "event_triggered"));

  ASSERT_TRUE(std::holds_alternative<ocucp::rrc_event_trigger_cfg>(report_cfg));
  const auto& event = std::get<ocucp::rrc_event_trigger_cfg>(report_cfg).event_id;
  EXPECT_EQ(event.id, ocucp::rrc_event_id::event_id_t::a4);
  ASSERT_TRUE(event.meas_trigger_quant_thres_or_offset->rsrp.has_value());
  EXPECT_EQ(event.meas_trigger_quant_thres_or_offset->rsrp.value(), 56);
  ASSERT_TRUE(event.use_allowed_cell_list.has_value());
  EXPECT_FALSE(event.use_allowed_cell_list.value());
}

/// A5 is the only event taking two thresholds.
TEST(cu_cp_config_translators_test, a5_event_triggered_report_carries_two_thresholds)
{
  const ocucp::rrc_report_cfg_nr report_cfg =
      translate(make_signal_level_report_config(ocucp::rrc_event_id::event_id_t::a5, "event_triggered"));

  ASSERT_TRUE(std::holds_alternative<ocucp::rrc_event_trigger_cfg>(report_cfg));
  const auto& event = std::get<ocucp::rrc_event_trigger_cfg>(report_cfg).event_id;
  EXPECT_EQ(event.id, ocucp::rrc_event_id::event_id_t::a5);
  ASSERT_TRUE(event.meas_trigger_quant_thres_or_offset->rsrp.has_value());
  EXPECT_EQ(event.meas_trigger_quant_thres_or_offset->rsrp.value(), 56); // -100 dB + 156
  ASSERT_TRUE(event.meas_trigger_quant_thres_2.has_value());
  ASSERT_TRUE(event.meas_trigger_quant_thres_2->rsrp.has_value());
  EXPECT_EQ(event.meas_trigger_quant_thres_2->rsrp.value(), 66); // -90 dB + 156
  ASSERT_TRUE(event.use_allowed_cell_list.has_value());
}

/// TS 38.331 sec. 6.3.2 gives CondTriggerConfig condEventA4, which takes a threshold rather than an offset.
TEST(cu_cp_config_translators_test, a4_conditional_trigger_carries_a_threshold)
{
  const ocucp::rrc_report_cfg_nr report_cfg =
      translate(make_signal_level_report_config(ocucp::rrc_event_id::event_id_t::a4, "cond_trigger"));

  ASSERT_TRUE(std::holds_alternative<ocucp::rrc_cond_trigger_cfg>(report_cfg));
  const auto& event = std::get<ocucp::rrc_cond_trigger_cfg>(report_cfg).cond_event_id;
  EXPECT_EQ(event.id, ocucp::rrc_event_id::event_id_t::a4);
  ASSERT_TRUE(event.meas_trigger_quant_thres_or_offset->rsrp.has_value());
  EXPECT_EQ(event.meas_trigger_quant_thres_or_offset->rsrp.value(), 56); // -100 dB + 156
  EXPECT_FALSE(event.use_allowed_cell_list.has_value());
}

/// condEventA5 is the only conditional event taking two thresholds.
TEST(cu_cp_config_translators_test, a5_conditional_trigger_carries_two_thresholds)
{
  const ocucp::rrc_report_cfg_nr report_cfg =
      translate(make_signal_level_report_config(ocucp::rrc_event_id::event_id_t::a5, "cond_trigger"));

  ASSERT_TRUE(std::holds_alternative<ocucp::rrc_cond_trigger_cfg>(report_cfg));
  const auto& event = std::get<ocucp::rrc_cond_trigger_cfg>(report_cfg).cond_event_id;
  EXPECT_EQ(event.id, ocucp::rrc_event_id::event_id_t::a5);
  ASSERT_TRUE(event.meas_trigger_quant_thres_or_offset->rsrp.has_value());
  EXPECT_EQ(event.meas_trigger_quant_thres_or_offset->rsrp.value(), 56);
  ASSERT_TRUE(event.meas_trigger_quant_thres_2.has_value());
  EXPECT_EQ(event.meas_trigger_quant_thres_2->rsrp.value(), 66); // -90 dB + 156
  EXPECT_FALSE(event.use_allowed_cell_list.has_value());
}

/// A regression test to verify that the CU-CP application config correctly parses rrc_reject_wait_time_s.
TEST(cu_cp_config_translators_test, rrc_reject_wait_time_s_is_propagated_to_cu_cp_configuration)
{
  cu_cp_unit_config cfg;
  cfg.rrc_config.rrc_reject_wait_time_s = 16;

  const ocucp::cu_cp_configuration out_cfg = generate_cu_cp_config(cfg);

  ASSERT_TRUE(out_cfg.rrc.rrc_reject_wait_time.has_value())
      << "rrc_reject_wait_time_s from the application config was not propagated to the CU-CP configuration";
  EXPECT_EQ(out_cfg.rrc.rrc_reject_wait_time.value(), std::chrono::seconds{16});
}

TEST(cu_cp_config_translators_test, unset_rrc_reject_wait_time_s_leaves_no_wait_time)
{
  cu_cp_unit_config cfg; // rrc_config.rrc_reject_wait_time_s is std::nullopt by default.

  const ocucp::cu_cp_configuration out_cfg = generate_cu_cp_config(cfg);

  EXPECT_FALSE(out_cfg.rrc.rrc_reject_wait_time.has_value());
}

TEST(cu_cp_config_translators_test, satellite_rat_type_of_a_tracking_area_is_propagated_to_the_ngap_configuration)
{
  OCUDU_TEST_REQUIREMENTS("CU-NTN-LOC-5");

  cu_cp_unit_config cfg;
  cfg.amf_config.amf.supported_tas.front().satellite_rat = "nr_leo";
  cfg.amf_config.amf.supported_tas.push_back({8, cfg.amf_config.amf.supported_tas.front().plmn_list, std::nullopt});

  const ocucp::cu_cp_configuration out_cfg = generate_cu_cp_config(cfg);

  ASSERT_EQ(out_cfg.ngap.ngaps.size(), 1);
  const auto& supported_tas = out_cfg.ngap.ngaps.front().supported_tas;
  ASSERT_EQ(supported_tas.size(), 2);
  ASSERT_EQ(supported_tas[0].satellite_rat, ocucp::satellite_rat_type::nr_leo);
  ASSERT_FALSE(supported_tas[1].satellite_rat.has_value()) << "A terrestrial tracking area must carry no satellite RAT";
}

TEST(cu_cp_config_translators_test, tracking_areas_carry_no_satellite_rat_type_when_it_is_not_configured)
{
  OCUDU_TEST_REQUIREMENTS("CU-NTN-LOC-5");

  cu_cp_unit_config cfg;
  cfg.amf_config.amf.supported_tas.push_back({8, cfg.amf_config.amf.supported_tas.front().plmn_list, std::nullopt});

  const ocucp::cu_cp_configuration out_cfg = generate_cu_cp_config(cfg);

  ASSERT_EQ(out_cfg.ngap.ngaps.size(), 1);
  const auto& supported_tas = out_cfg.ngap.ngaps.front().supported_tas;
  ASSERT_EQ(supported_tas.size(), 2);
  for (const auto& ta : supported_tas) {
    ASSERT_FALSE(ta.satellite_rat.has_value()) << "TAC " << ta.tac << " must carry no satellite RAT";
  }
}
