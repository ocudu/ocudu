// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/ntn/converters/reference_frame_converter.h"
#include "lib/ntn/coordinates_types.h"
#include "lib/ntn/ntn_assistance_info_generator.h"
#include "ocudu/ran/ntn.h"
#include "ocudu/ran/slot_point.h"
#include "ocudu/support/test_utils.h"
#include "fmt/printf.h"
#include <gtest/gtest.h>
#include <iomanip>

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

TEST(test_ntn_assistance_info_cfg_generator, access_link_only_scenario)
{
  // Tolerances.
  double pos_tolerance_m   = 3;   // 3m -> delay error of 10ns
  double vel_tolerance_m_s = 0.1; // 0.1m/s -> doppler error at 2GHz of 0.67Hz

  std::vector<std::tuple<double, state_vector>> test_cases = {
      {0, {{-12451277.795, 40348000.091, -1726486.314}, {35.631, -6.276, -385.708}}},
      {0.5, {{-12451259.968, 40347996.956, -1726679.165}, {35.630, -6.278, -385.704}}},
      {1, {{-12451242.141, 40347993.822, -1726872.016}, {35.629, -6.279, -385.699}}},
      {2, {{-12451206.489, 40347987.550, -1727257.710}, {35.628, -6.283, -385.690}}},
      {3, {{-12451170.838, 40347981.274, -1727643.394}, {35.627, -6.287, -385.681}}},
      {4, {{-12451135.188, 40347974.994, -1728029.069}, {35.625, -6.291, -385.672}}},
      {5, {{-12451099.540, 40347968.710, -1728414.735}, {35.624, -6.294, -385.663}}},
      {6, {{-320105.690, 6864002.842, 70594.908}, {-4049.063, -258.559, 6105.147}}}, // switch satellite
      {7, {{-324154.570, 6863740.371, 76700.001}, {-4048.706, -266.375, 6105.056}}},
      {8, {{-328203.092, 6863470.085, 82804.999}, {-4048.346, -274.191, 6104.958}}},
      {9, {{-332251.252, 6863191.982, 88909.896}, {-4047.982, -282.006, 6104.852}}},
      {10, {{-336299.046, 6862906.065, 95014.683}, {-4047.615, -289.822, 6104.738}}},
  };

  // Start with one satellite.
  std::string        epoch_time_utc_t0 = "2025-06-24T09:00:00"; // need to set some value
  auto               epoch_time_t0     = string_to_timepoint(epoch_time_utc_t0);
  ecef_coordinates_t ecef_rv_t0        = {-12451277.795, 40348000.091, -1726486.314, 35.631, -6.276, -385.708};

  // Switch to a different satellite.
  std::string        epoch_time_utc_t6 = "2025-06-24T09:00:06"; // need to set some value
  auto               epoch_time_t6     = string_to_timepoint(epoch_time_utc_t6);
  ecef_coordinates_t ecef_rv_t6        = {-320105.690, 6864002.842, 70594.908, -4049.063, -258.559, 6105.147};

  // Create SIB19 NTN-Config generator.
  ntn_assistance_info_generator sib19_ntn_cfg_gen;

  // Feed LEO ephemeris info into SIB19 NTN-Config generator.
  ephemeris_info_update ephemeris_info_t0;
  ephemeris_info_t0.epoch_time     = epoch_time_t0;
  ephemeris_info_t0.ephemeris_info = ecef_rv_t0;
  sib19_ntn_cfg_gen.enqueue_ephemeris_info(ephemeris_info_t0);

  ephemeris_info_update ephemeris_info_t6;
  ephemeris_info_t6.epoch_time     = epoch_time_t6;
  ephemeris_info_t6.ephemeris_info = ecef_rv_t6;
  sib19_ntn_cfg_gen.enqueue_ephemeris_info(ephemeris_info_t6);

  for (const auto& [propagation_time, expected_ecef_rv] : test_cases) {
    auto duration = std::chrono::duration_cast<std::chrono::system_clock::duration>(
        std::chrono::duration<double>(propagation_time));

    // Request generation of SIB19 at specific Tx time.
    sib19_ntn_configs_request request;
    request.use_state_vector         = true;
    request.feeder_link_present      = false;
    request.epoch_time               = epoch_time_t0 + duration;
    request.epoch_slot               = slot_point(0, propagation_time);
    request.ntn_ul_sync_validity_dur = 5;

    sib19_ntn_configs_reply reply = sib19_ntn_cfg_gen.generate_ntn_config(request);
    ASSERT_TRUE(reply.success);
    ASSERT_EQ(reply.epoch_time, reply.epoch_time);
    ASSERT_EQ(reply.epoch_slot, reply.epoch_slot);
    ASSERT_EQ(reply.ntn_ul_sync_validity_dur, 5);
    if (request.use_state_vector) {
      ASSERT_TRUE(std::holds_alternative<ecef_coordinates_t>(reply.ephemeris_info));
      auto ecef = std::get<ecef_coordinates_t>(reply.ephemeris_info);
      ASSERT_NEAR(ecef.position_x, expected_ecef_rv.position.x, pos_tolerance_m);
      ASSERT_NEAR(ecef.position_y, expected_ecef_rv.position.y, pos_tolerance_m);
      ASSERT_NEAR(ecef.position_z, expected_ecef_rv.position.z, pos_tolerance_m);
      ASSERT_NEAR(ecef.velocity_vx, expected_ecef_rv.velocity.x, vel_tolerance_m_s);
      ASSERT_NEAR(ecef.velocity_vy, expected_ecef_rv.velocity.y, vel_tolerance_m_s);
      ASSERT_NEAR(ecef.velocity_vz, expected_ecef_rv.velocity.z, vel_tolerance_m_s);
    } else {
      ASSERT_TRUE(std::holds_alternative<orbital_coordinates_t>(reply.ephemeris_info));
    }
    ASSERT_FALSE(reply.ta_info.has_value());
  }
}

TEST(test_ntn_assistance_info_cfg_generator, feeder_link_scenario)
{
  // Tolerances.
  double pos_tolerance_m                 = 3;     // 3m -> delay error of 10ns
  double vel_tolerance_m_s               = 0.1;   // 0.1m/s -> doppler error at 2GHz of 0.67Hz
  double fl_delay_tolerance_us           = 0.001; // 1ns
  double fl_delay_drift_tolerance_us     = 0.001; // 1ns/s
  double fl_delay_drift_var_tolerance_us = 0.001; // 1ns/s^2

  ntn_assistance_info_generator sib19_ntn_cfg_gen;

  std::string epoch_time_utc = "2025-06-24T09:00:00"; // need to set some value
  auto        epoch_time     = string_to_timepoint(epoch_time_utc);

  std::vector<std::tuple<double, ta_info_t, state_vector>> test_cases = {
      {0, {8848.607, -43.5372, 0.016784}, {{2130981.570, -6532680.896, 70598.229}, {3835.103, 1324.292, 6105.147}}},
      {1, {8805.087, -43.5047, 0.017062}, {{2134815.458, -6531352.882, 76703.323}, {3832.684, 1331.733, 6105.056}}},
      {2, {8761.600, -43.4717, 0.017345}, {{2138646.926, -6530017.427, 82808.321}, {3830.262, 1339.172, 6104.958}}},
      {3, {8718.146, -43.4381, 0.017634}, {{2142475.970, -6528674.533, 88913.217}, {3827.836, 1346.611, 6104.852}}},
      {4, {8674.726, -43.4039, 0.017928}, {{2146302.586, -6527324.201, 95018.004}, {3825.407, 1354.048, 6104.738}}},
      {5, {8631.340, -43.3692, 0.018228}, {{2150126.771, -6525966.433, 101122.673}, {3822.974, 1361.484, 6104.617}}},
      {6, {8587.990, -43.3339, 0.018534}, {{2153948.521, -6524601.230, 107227.218}, {3820.537, 1368.918, 6104.489}}},
      {7, {8544.675, -43.2980, 0.018845}, {{2157767.833, -6523228.592, 113331.631}, {3818.097, 1376.352, 6104.353}}},
      {8, {8501.396, -43.2615, 0.019162}, {{2161584.702, -6521848.522, 119435.903}, {3815.653, 1383.784, 6104.209}}},
      {9, {8458.155, -43.2244, 0.019486}, {{2165399.126, -6520461.021, 125540.029}, {3813.205, 1391.214, 6104.058}}},
      {10, {8414.950, -43.1867, 0.01981}, {{2169211.101, -6519066.089, 131643.999}, {3810.754, 1398.644, 6103.899}}},
  };

  // LEO ephemeris info.
  ecef_coordinates_t    ecef_rv = {2130981.570, -6532680.896, 70598.229, 3835.102, 1324.291, 6105.146};
  ephemeris_info_update ephemeris_info;
  ephemeris_info.epoch_time     = epoch_time;
  ephemeris_info.ephemeris_info = ecef_rv;

  ntn_gateway_location_info ntn_gw_loc;
  ntn_gw_loc.service_start_time             = epoch_time;
  ntn_gw_loc.ntn_gateway_location.latitude  = 9.362550871290306;
  ntn_gw_loc.ntn_gateway_location.longitude = -65.747661460614;
  ntn_gw_loc.ntn_gateway_location.altitude  = 1.0;

  sib19_ntn_cfg_gen.enqueue_ephemeris_info(ephemeris_info);
  sib19_ntn_cfg_gen.enqueue_ntn_gw_location(ntn_gw_loc);

  for (const auto& [propagation_time, expected_ta_info, expected_ecef_rv] : test_cases) {
    auto duration = std::chrono::duration_cast<std::chrono::system_clock::duration>(
        std::chrono::duration<double>(propagation_time));

    // Request generation of SIB19 at specific Tx time.
    sib19_ntn_configs_request request;
    request.use_state_vector         = true;
    request.feeder_link_present      = true;
    request.epoch_time               = epoch_time + duration;
    request.epoch_slot               = slot_point(0, propagation_time);
    request.ntn_ul_sync_validity_dur = 5;

    sib19_ntn_configs_reply reply = sib19_ntn_cfg_gen.generate_ntn_config(request);
    ASSERT_TRUE(reply.success);
    ASSERT_EQ(reply.epoch_time, reply.epoch_time);
    ASSERT_EQ(reply.epoch_slot, reply.epoch_slot);
    ASSERT_EQ(reply.ntn_ul_sync_validity_dur, 5);
    if (request.use_state_vector) {
      ASSERT_TRUE(std::holds_alternative<ecef_coordinates_t>(reply.ephemeris_info));
      auto ecef = std::get<ecef_coordinates_t>(reply.ephemeris_info);
      ASSERT_NEAR(ecef.position_x, expected_ecef_rv.position.x, pos_tolerance_m);
      ASSERT_NEAR(ecef.position_y, expected_ecef_rv.position.y, pos_tolerance_m);
      ASSERT_NEAR(ecef.position_z, expected_ecef_rv.position.z, pos_tolerance_m);
      ASSERT_NEAR(ecef.velocity_vx, expected_ecef_rv.velocity.x, vel_tolerance_m_s);
      ASSERT_NEAR(ecef.velocity_vy, expected_ecef_rv.velocity.y, vel_tolerance_m_s);
      ASSERT_NEAR(ecef.velocity_vz, expected_ecef_rv.velocity.z, vel_tolerance_m_s);
    } else {
      ASSERT_TRUE(std::holds_alternative<orbital_coordinates_t>(reply.ephemeris_info));
    }
    ASSERT_TRUE(reply.ta_info.has_value());
    ta_info_t& ta_info = *reply.ta_info;
    ASSERT_NEAR(ta_info.ta_common, expected_ta_info.ta_common, fl_delay_tolerance_us);
    ASSERT_NEAR(ta_info.ta_common_drift, expected_ta_info.ta_common_drift, fl_delay_drift_tolerance_us);
    ASSERT_NEAR(
        ta_info.ta_common_drift_variant, expected_ta_info.ta_common_drift_variant, fl_delay_drift_var_tolerance_us);
  }
}
