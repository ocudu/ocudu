// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "apps/units/flexible_o_du/split_helpers/commands/ntn_config_update_remote_command.h"
#include "nlohmann/json.hpp"
#include "ocudu/ntn/ntn_configuration_manager.h"
#include "ocudu/ocudulog/logger.h"
#include "fmt/format.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocudu_ntn;

class dummy_ntn_configuration_manager : public ntn_configuration_manager
{
public:
  ntn_config_update_result handle_ntn_config_update(const ntn_config_update_info& req) override
  {
    last_request = req;
    return result_to_return;
  }

  // The remote command does not drive the manager lifecycle, so these are no-ops.
  void start() override {}

  void stop() override {}

  ntn_config_update_info   last_request;
  ntn_config_update_result result_to_return;
};

/// Test fixture for NTN config update remote command tests.
class ntn_config_update_remote_command_test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    logger.set_level(ocudulog::basic_levels::debug);
    ocudulog::init();
  }

  void TearDown() override
  {
    // flush logger after each test
    ocudulog::flush();
  }

  dummy_ntn_configuration_manager ntn_cfg_manager;
  ocudulog::basic_logger&         logger = ocudulog::fetch_basic_logger("TEST");
};

TEST_F(ntn_config_update_remote_command_test, single_cell_update_succeeds)
{
  // Create remote command.
  ntn_config_update_remote_command cmd(ntn_cfg_manager);

  // Build request with one valid cell.
  nlohmann::json req;
  req["cells"] = nlohmann::json::array();

  auto future_time_ms =
      std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
          .count() +
      10000ULL;

  nlohmann::json cell;
  cell["plmn"]                                  = "00101";
  cell["nci"]                                   = static_cast<uint64_t>(1);
  cell["epoch_timestamp"]                       = future_time_ms;
  cell["ntn_ul_sync_validity_duration"]         = 5;
  cell["ephemeris_info"]["ecef"]["position_x"]  = 3578630.0;
  cell["ephemeris_info"]["ecef"]["position_y"]  = 1234567.0;
  cell["ephemeris_info"]["ecef"]["position_z"]  = 5432109.0;
  cell["ephemeris_info"]["ecef"]["velocity_vx"] = 1500.0;
  cell["ephemeris_info"]["ecef"]["velocity_vy"] = 3073.0;
  cell["ephemeris_info"]["ecef"]["velocity_vz"] = -2500.0;

  // Optional fields.
  cell["ta_info"]["ta_common"]               = 7874.95;
  cell["ta_info"]["ta_common_drift"]         = 0.5;
  cell["ta_info"]["ta_common_drift_variant"] = 0.1;

  cell["feeder_link_info"]["enable_doppler_compensation"] = true;
  cell["feeder_link_info"]["dl_freq"]                     = 2185000000.0;
  cell["feeder_link_info"]["ul_freq"]                     = 1995000000.0;

  cell["ntn_gateway_location"]["latitude"]  = 45.0;  // degrees
  cell["ntn_gateway_location"]["longitude"] = 15.0;  // degrees
  cell["ntn_gateway_location"]["altitude"]  = 100.0; // meters

  req["cells"].push_back(cell);

  // Setup expected result
  nr_cell_global_id_t cgi;
  cgi.plmn_id = plmn_identity::parse("00101").value();
  cgi.nci     = nr_cell_identity::create(1).value();
  ntn_cfg_manager.result_to_return.succeeded.push_back(cgi);

  // Execute command
  expected<nlohmann::json, std::string> result = cmd.execute(req);

  // Verify command succeeded
  ASSERT_TRUE(result.has_value()) << "Command failed: " << result.error();
  ASSERT_EQ(ntn_cfg_manager.last_request.cells.size(), 1);
  EXPECT_EQ(ntn_cfg_manager.last_request.cells[0].nr_cgi.nci.value(), 1);

  // Verify epoch timestamp.
  auto epoch_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                      ntn_cfg_manager.last_request.cells[0].epoch_time.time_since_epoch())
                      .count();
  EXPECT_EQ(epoch_ms, future_time_ms);

  // Verify required fields.
  EXPECT_EQ(ntn_cfg_manager.last_request.cells[0].ntn_ul_sync_validity_duration, 5);
  EXPECT_TRUE(std::holds_alternative<ecef_coordinates_t>(ntn_cfg_manager.last_request.cells[0].ephemeris_info));

  // Verify ECEF coordinates.
  auto& ecef = std::get<ecef_coordinates_t>(ntn_cfg_manager.last_request.cells[0].ephemeris_info);
  EXPECT_DOUBLE_EQ(ecef.position_x, 3578630.0);
  EXPECT_DOUBLE_EQ(ecef.position_y, 1234567.0);
  EXPECT_DOUBLE_EQ(ecef.position_z, 5432109.0);
  EXPECT_DOUBLE_EQ(ecef.velocity_vx, 1500.0);
  EXPECT_DOUBLE_EQ(ecef.velocity_vy, 3073.0);
  EXPECT_DOUBLE_EQ(ecef.velocity_vz, -2500.0);

  // Verify optional fields were parsed.
  ASSERT_TRUE(ntn_cfg_manager.last_request.cells[0].ta_info.has_value());
  EXPECT_DOUBLE_EQ(ntn_cfg_manager.last_request.cells[0].ta_info->ta_common, 7874.95);
  EXPECT_DOUBLE_EQ(ntn_cfg_manager.last_request.cells[0].ta_info->ta_common_drift, 0.5);
  EXPECT_DOUBLE_EQ(ntn_cfg_manager.last_request.cells[0].ta_info->ta_common_drift_variant, 0.1);

  ASSERT_TRUE(ntn_cfg_manager.last_request.cells[0].feeder_link_info.has_value());
  EXPECT_TRUE(ntn_cfg_manager.last_request.cells[0].feeder_link_info->enable_doppler_compensation);
  EXPECT_DOUBLE_EQ(ntn_cfg_manager.last_request.cells[0].feeder_link_info->dl_freq, 2185000000.0);
  EXPECT_DOUBLE_EQ(ntn_cfg_manager.last_request.cells[0].feeder_link_info->ul_freq, 1995000000.0);

  ASSERT_TRUE(ntn_cfg_manager.last_request.cells[0].ntn_gateway_location.has_value());
  EXPECT_DOUBLE_EQ(ntn_cfg_manager.last_request.cells[0].ntn_gateway_location->latitude, 45.0);
  EXPECT_DOUBLE_EQ(ntn_cfg_manager.last_request.cells[0].ntn_gateway_location->longitude, 15.0);
  EXPECT_DOUBLE_EQ(ntn_cfg_manager.last_request.cells[0].ntn_gateway_location->altitude, 100.0);
}

TEST_F(ntn_config_update_remote_command_test, multi_cell_with_complete_config)
{
  // Create remote command.
  ntn_config_update_remote_command cmd(ntn_cfg_manager);

  // Use future timestamp (current time + 10 seconds).
  auto future_time_ms =
      std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
          .count() +
      10000ULL;

  // Build JSON request: Multi-cell with complete config per cell.
  nlohmann::json req;
  req["cells"] = nlohmann::json::array();

  nlohmann::json cell1;
  cell1["plmn"]                                  = "00101";
  cell1["nci"]                                   = static_cast<uint64_t>(1);
  cell1["epoch_timestamp"]                       = future_time_ms;
  cell1["ntn_ul_sync_validity_duration"]         = 5;
  cell1["ephemeris_info"]["ecef"]["position_x"]  = 3578630.0;
  cell1["ephemeris_info"]["ecef"]["position_y"]  = 1234567.0;
  cell1["ephemeris_info"]["ecef"]["position_z"]  = 5432109.0;
  cell1["ephemeris_info"]["ecef"]["velocity_vx"] = 1500.0;
  cell1["ephemeris_info"]["ecef"]["velocity_vy"] = 3073.0;
  cell1["ephemeris_info"]["ecef"]["velocity_vz"] = -2500.0;

  nlohmann::json cell2;
  cell2["plmn"]                                         = "00101";
  cell2["nci"]                                          = static_cast<uint64_t>(2);
  cell2["epoch_timestamp"]                              = future_time_ms + 10000ULL;
  cell2["ntn_ul_sync_validity_duration"]                = 5;
  cell2["ephemeris_info"]["orbital"]["semi_major_axis"] = 6917000.0;
  cell2["ephemeris_info"]["orbital"]["eccentricity"]    = 0.001;
  cell2["ephemeris_info"]["orbital"]["periapsis"]       = 0.5236;
  cell2["ephemeris_info"]["orbital"]["longitude"]       = 1.0472;
  cell2["ephemeris_info"]["orbital"]["inclination"]     = 1.5708;
  cell2["ephemeris_info"]["orbital"]["mean_anomaly"]    = 0.7854;

  req["cells"].push_back(cell1);
  req["cells"].push_back(cell2);

  // Setup expected result.
  nr_cell_global_id_t cgi1;
  cgi1.plmn_id = plmn_identity::parse("00101").value();
  cgi1.nci     = nr_cell_identity::create(1).value();

  nr_cell_global_id_t cgi2;
  cgi2.plmn_id = plmn_identity::parse("00101").value();
  cgi2.nci     = nr_cell_identity::create(2).value();

  ntn_cfg_manager.result_to_return.succeeded.push_back(cgi1);
  ntn_cfg_manager.result_to_return.succeeded.push_back(cgi2);

  // Execute command.
  expected<nlohmann::json, std::string> result = cmd.execute(req);

  // Verify command succeeded.
  ASSERT_TRUE(result.has_value()) << "Command failed: " << result.error();

  // Verify correct number of cells were sent to manager.
  ASSERT_EQ(ntn_cfg_manager.last_request.cells.size(), 2);

  // Verify first cell config.
  EXPECT_EQ(ntn_cfg_manager.last_request.cells[0].nr_cgi.nci.value(), 1);

  // Verify epoch timestamp for cell 1.
  auto epoch1_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                       ntn_cfg_manager.last_request.cells[0].epoch_time.time_since_epoch())
                       .count();
  EXPECT_EQ(epoch1_ms, future_time_ms);

  EXPECT_EQ(ntn_cfg_manager.last_request.cells[0].ntn_ul_sync_validity_duration, 5);
  EXPECT_TRUE(std::holds_alternative<ecef_coordinates_t>(ntn_cfg_manager.last_request.cells[0].ephemeris_info));

  // Verify all ECEF coordinates for cell 1.
  auto& ecef1 = std::get<ecef_coordinates_t>(ntn_cfg_manager.last_request.cells[0].ephemeris_info);
  EXPECT_DOUBLE_EQ(ecef1.position_x, 3578630.0);
  EXPECT_DOUBLE_EQ(ecef1.position_y, 1234567.0);
  EXPECT_DOUBLE_EQ(ecef1.position_z, 5432109.0);
  EXPECT_DOUBLE_EQ(ecef1.velocity_vx, 1500.0);
  EXPECT_DOUBLE_EQ(ecef1.velocity_vy, 3073.0);
  EXPECT_DOUBLE_EQ(ecef1.velocity_vz, -2500.0);

  // Verify second cell config (uses orbital parameters).
  EXPECT_EQ(ntn_cfg_manager.last_request.cells[1].nr_cgi.nci.value(), 2);

  // Verify epoch timestamp for cell 2.
  auto epoch2_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                       ntn_cfg_manager.last_request.cells[1].epoch_time.time_since_epoch())
                       .count();
  EXPECT_EQ(epoch2_ms, future_time_ms + 10000ULL);

  EXPECT_EQ(ntn_cfg_manager.last_request.cells[1].ntn_ul_sync_validity_duration, 5);
  EXPECT_TRUE(std::holds_alternative<orbital_coordinates_t>(ntn_cfg_manager.last_request.cells[1].ephemeris_info));

  // Verify all orbital parameters for cell 2.
  auto& orbital2 = std::get<orbital_coordinates_t>(ntn_cfg_manager.last_request.cells[1].ephemeris_info);
  EXPECT_DOUBLE_EQ(orbital2.semi_major_axis, 6917000.0);
  EXPECT_DOUBLE_EQ(orbital2.eccentricity, 0.001);
  EXPECT_DOUBLE_EQ(orbital2.periapsis, 0.5236);
  EXPECT_DOUBLE_EQ(orbital2.longitude, 1.0472);
  EXPECT_DOUBLE_EQ(orbital2.inclination, 1.5708);
  EXPECT_DOUBLE_EQ(orbital2.mean_anomaly, 0.7854);
}

TEST_F(ntn_config_update_remote_command_test, common_config_with_overrides)
{
  // Create remote command.
  ntn_config_update_remote_command cmd(ntn_cfg_manager);

  // Build JSON request: Common config + per-cell overrides.
  nlohmann::json req;

  // Use future timestamp.
  auto future_time_ms =
      std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
          .count() +
      10000ULL;

  // Common NTN config
  req["common_ntn_config"]["epoch_timestamp"]                       = future_time_ms;
  req["common_ntn_config"]["ntn_ul_sync_validity_duration"]         = 5;
  req["common_ntn_config"]["ephemeris_info"]["ecef"]["position_x"]  = 3578630.0;
  req["common_ntn_config"]["ephemeris_info"]["ecef"]["position_y"]  = 1234567.0;
  req["common_ntn_config"]["ephemeris_info"]["ecef"]["position_z"]  = 5432109.0;
  req["common_ntn_config"]["ephemeris_info"]["ecef"]["velocity_vx"] = 1500.0;
  req["common_ntn_config"]["ephemeris_info"]["ecef"]["velocity_vy"] = 3073.0;
  req["common_ntn_config"]["ephemeris_info"]["ecef"]["velocity_vz"] = -2500.0;

  // Cells array.
  req["cells"] = nlohmann::json::array();

  // Cell 1: Uses common config (no cell-specific fields).
  nlohmann::json cell1;
  cell1["plmn"] = "00101";
  cell1["nci"]  = static_cast<uint64_t>(1);

  // Cell 2: Override epoch_timestamp (directly in cell, no "overrides" wrapper).
  nlohmann::json cell2;
  cell2["plmn"]            = "00101";
  cell2["nci"]             = static_cast<uint64_t>(2);
  cell2["epoch_timestamp"] = future_time_ms + 10000ULL;

  // Cell 3: Override epoch and ephemeris with orbital parameters (directly in cell).
  nlohmann::json cell3;
  cell3["plmn"]                                         = "00101";
  cell3["nci"]                                          = static_cast<uint64_t>(3);
  cell3["epoch_timestamp"]                              = future_time_ms + 20000ULL;
  cell3["ephemeris_info"]["orbital"]["semi_major_axis"] = 6917000.0;
  cell3["ephemeris_info"]["orbital"]["eccentricity"]    = 0.001;
  cell3["ephemeris_info"]["orbital"]["periapsis"]       = 0.5236;
  cell3["ephemeris_info"]["orbital"]["longitude"]       = 1.0472;
  cell3["ephemeris_info"]["orbital"]["inclination"]     = 1.5708;
  cell3["ephemeris_info"]["orbital"]["mean_anomaly"]    = 0.7854;

  req["cells"].push_back(cell1);
  req["cells"].push_back(cell2);
  req["cells"].push_back(cell3);

  // Setup expected result.
  nr_cell_global_id_t cgi1, cgi2, cgi3;
  cgi1.plmn_id = plmn_identity::parse("00101").value();
  cgi1.nci     = nr_cell_identity::create(1).value();
  cgi2.plmn_id = plmn_identity::parse("00101").value();
  cgi2.nci     = nr_cell_identity::create(2).value();
  cgi3.plmn_id = plmn_identity::parse("00101").value();
  cgi3.nci     = nr_cell_identity::create(3).value();

  ntn_cfg_manager.result_to_return.succeeded.push_back(cgi1);
  ntn_cfg_manager.result_to_return.succeeded.push_back(cgi2);
  ntn_cfg_manager.result_to_return.succeeded.push_back(cgi3);

  // Execute command.
  expected<nlohmann::json, std::string> result = cmd.execute(req);

  // Verify command succeeded.
  ASSERT_TRUE(result.has_value()) << "Command failed: " << result.error();

  // Verify correct number of cells.
  ASSERT_EQ(ntn_cfg_manager.last_request.cells.size(), 3);

  // Verify Cell 1: Uses common config.
  EXPECT_EQ(ntn_cfg_manager.last_request.cells[0].nr_cgi.nci.value(), 1);
  auto epoch1_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                       ntn_cfg_manager.last_request.cells[0].epoch_time.time_since_epoch())
                       .count();
  EXPECT_EQ(epoch1_ms, future_time_ms);
  EXPECT_EQ(ntn_cfg_manager.last_request.cells[0].ntn_ul_sync_validity_duration, 5);

  // Verify all ECEF coordinates from common config.
  auto& ecef1 = std::get<ecef_coordinates_t>(ntn_cfg_manager.last_request.cells[0].ephemeris_info);
  EXPECT_DOUBLE_EQ(ecef1.position_x, 3578630.0);
  EXPECT_DOUBLE_EQ(ecef1.position_y, 1234567.0);
  EXPECT_DOUBLE_EQ(ecef1.position_z, 5432109.0);
  EXPECT_DOUBLE_EQ(ecef1.velocity_vx, 1500.0);
  EXPECT_DOUBLE_EQ(ecef1.velocity_vy, 3073.0);
  EXPECT_DOUBLE_EQ(ecef1.velocity_vz, -2500.0);

  // Verify Cell 2: Overridden epoch_timestamp, other parameters from common config.
  EXPECT_EQ(ntn_cfg_manager.last_request.cells[1].nr_cgi.nci.value(), 2);

  // Verify epoch timestamp (cell-specific override).
  auto epoch2_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                       ntn_cfg_manager.last_request.cells[1].epoch_time.time_since_epoch())
                       .count();
  EXPECT_EQ(epoch2_ms, future_time_ms + 10000ULL);

  // Verify ntn_ul_sync_validity_duration (from common config).
  EXPECT_EQ(ntn_cfg_manager.last_request.cells[1].ntn_ul_sync_validity_duration, 5);

  // Verify all ECEF coordinates (from common config).
  EXPECT_TRUE(std::holds_alternative<ecef_coordinates_t>(ntn_cfg_manager.last_request.cells[1].ephemeris_info));
  auto& ecef2 = std::get<ecef_coordinates_t>(ntn_cfg_manager.last_request.cells[1].ephemeris_info);
  EXPECT_DOUBLE_EQ(ecef2.position_x, 3578630.0);
  EXPECT_DOUBLE_EQ(ecef2.position_y, 1234567.0);
  EXPECT_DOUBLE_EQ(ecef2.position_z, 5432109.0);
  EXPECT_DOUBLE_EQ(ecef2.velocity_vx, 1500.0);
  EXPECT_DOUBLE_EQ(ecef2.velocity_vy, 3073.0);
  EXPECT_DOUBLE_EQ(ecef2.velocity_vz, -2500.0);

  // Verify Cell 3: Overridden epoch_timestamp and ephemeris_info, ntn_ul_sync_validity_duration from common config.
  EXPECT_EQ(ntn_cfg_manager.last_request.cells[2].nr_cgi.nci.value(), 3);

  // Verify epoch timestamp (cell-specific override).
  auto epoch3_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                       ntn_cfg_manager.last_request.cells[2].epoch_time.time_since_epoch())
                       .count();
  EXPECT_EQ(epoch3_ms, future_time_ms + 20000ULL);

  // Verify ntn_ul_sync_validity_duration (from common config).
  EXPECT_EQ(ntn_cfg_manager.last_request.cells[2].ntn_ul_sync_validity_duration, 5);

  // Verify all orbital parameters (cell-specific override - uses orbital instead of ECEF).
  EXPECT_TRUE(std::holds_alternative<orbital_coordinates_t>(ntn_cfg_manager.last_request.cells[2].ephemeris_info));
  auto& orbital3 = std::get<orbital_coordinates_t>(ntn_cfg_manager.last_request.cells[2].ephemeris_info);
  EXPECT_DOUBLE_EQ(orbital3.semi_major_axis, 6917000.0);
  EXPECT_DOUBLE_EQ(orbital3.eccentricity, 0.001);
  EXPECT_DOUBLE_EQ(orbital3.periapsis, 0.5236);
  EXPECT_DOUBLE_EQ(orbital3.longitude, 1.0472);
  EXPECT_DOUBLE_EQ(orbital3.inclination, 1.5708);
  EXPECT_DOUBLE_EQ(orbital3.mean_anomaly, 0.7854);
}

TEST_F(ntn_config_update_remote_command_test, common_config_without_cells_fails)
{
  // Create remote command.
  ntn_config_update_remote_command cmd(ntn_cfg_manager);

  // Build request with common_ntn_config but without cells array.
  nlohmann::json req;

  // Use future timestamp.
  auto future_time_ms =
      std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
          .count() +
      10000ULL;

  // Common NTN config - but no cells defined.
  req["common_ntn_config"]["epoch_timestamp"]                       = future_time_ms;
  req["common_ntn_config"]["ntn_ul_sync_validity_duration"]         = 5;
  req["common_ntn_config"]["ephemeris_info"]["ecef"]["position_x"]  = 3578630.0;
  req["common_ntn_config"]["ephemeris_info"]["ecef"]["position_y"]  = 1234567.0;
  req["common_ntn_config"]["ephemeris_info"]["ecef"]["position_z"]  = 5432109.0;
  req["common_ntn_config"]["ephemeris_info"]["ecef"]["velocity_vx"] = 1500.0;
  req["common_ntn_config"]["ephemeris_info"]["ecef"]["velocity_vy"] = 3073.0;
  req["common_ntn_config"]["ephemeris_info"]["ecef"]["velocity_vz"] = -2500.0;

  // No cells array provided - cell identities (NCIs) are unknown.

  // Execute command.
  expected<nlohmann::json, std::string> result = cmd.execute(req);

  // Verify command failed - cannot apply config without knowing which cells to update.
  ASSERT_FALSE(result.has_value());
  logger.info("Received error: {}", result.error());
  EXPECT_TRUE(result.error().find("cells") != std::string::npos ||
              result.error().find("must have") != std::string::npos)
      << "Expected error about missing cells, got: " << result.error();
}

TEST_F(ntn_config_update_remote_command_test, when_cells_array_is_empty_then_error_is_returned)
{
  // Create remote command.
  ntn_config_update_remote_command cmd(ntn_cfg_manager);

  // Build request with empty cells array.
  nlohmann::json req;
  req["cells"] = nlohmann::json::array();

  // Execute command.
  expected<nlohmann::json, std::string> result = cmd.execute(req);

  // Verify command failed - empty cells array.
  ASSERT_FALSE(result.has_value());
  logger.info("Received error: {}", result.error());
  EXPECT_TRUE(result.error().find("empty") != std::string::npos)
      << "Expected error about empty cells array, got: " << result.error();
}

TEST_F(ntn_config_update_remote_command_test, when_some_cells_fail_then_error_is_returned)
{
  // Create remote command.
  ntn_config_update_remote_command cmd(ntn_cfg_manager);

  // Build simple request.
  nlohmann::json req;
  req["cells"] = nlohmann::json::array();

  // Use future timestamp.
  auto future_time_ms =
      std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
          .count() +
      10000ULL;

  nlohmann::json cell1;
  cell1["plmn"]                                  = "00101";
  cell1["nci"]                                   = static_cast<uint64_t>(1);
  cell1["epoch_timestamp"]                       = future_time_ms;
  cell1["ntn_ul_sync_validity_duration"]         = 5;
  cell1["ephemeris_info"]["ecef"]["position_x"]  = 3578630.0;
  cell1["ephemeris_info"]["ecef"]["position_y"]  = 1234567.0;
  cell1["ephemeris_info"]["ecef"]["position_z"]  = 5432109.0;
  cell1["ephemeris_info"]["ecef"]["velocity_vx"] = 1500.0;
  cell1["ephemeris_info"]["ecef"]["velocity_vy"] = 3073.0;
  cell1["ephemeris_info"]["ecef"]["velocity_vz"] = -2500.0;

  req["cells"].push_back(cell1);

  // Setup result with failed cell.
  nr_cell_global_id_t cgi1;
  cgi1.plmn_id = plmn_identity::parse("00101").value();
  cgi1.nci     = nr_cell_identity::create(1).value();

  ntn_cfg_manager.result_to_return.failed.push_back(cgi1);

  // Execute command.
  expected<nlohmann::json, std::string> result = cmd.execute(req);

  // Verify command failed.
  ASSERT_FALSE(result.has_value());
  logger.info("Received error: {}", result.error());
  // Check for the enhanced error format with cell details
  EXPECT_TRUE(result.error().find("failed") != std::string::npos || result.error().find("Failed") != std::string::npos)
      << "Expected error about failed cells, got: " << result.error();
}

TEST_F(ntn_config_update_remote_command_test, when_invalid_format_then_error_is_returned)
{
  // Create remote command.
  ntn_config_update_remote_command cmd(ntn_cfg_manager);

  // Build invalid request (missing cells array).
  nlohmann::json req;
  req["epoch_timestamp"] = 1706000000000ULL;

  // Execute command.
  expected<nlohmann::json, std::string> result = cmd.execute(req);

  // Verify command failed.
  ASSERT_FALSE(result.has_value());
  logger.info("Received error: {}", result.error());
  EXPECT_TRUE(result.error().find("'cells' field is required") != std::string::npos)
      << "Expected error about missing cells array, got: " << result.error();
}

TEST_F(ntn_config_update_remote_command_test, when_request_has_invalid_field_then_error_is_returned)
{
  ntn_config_update_remote_command cmd(ntn_cfg_manager);

  auto future_time_ms =
      std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
          .count() +
      10000ULL;

  nlohmann::json req;
  req["cells"] = nlohmann::json::array();

  nlohmann::json cell;
  cell["plmn"]                                  = "00101";
  cell["nci"]                                   = static_cast<uint64_t>(1);
  cell["epoch_timestamp"]                       = future_time_ms;
  cell["ntn_ul_sync_validity_duration"]         = 5;
  cell["ephemeris_info"]["ecef"]["position_x"]  = 3578630.0;
  cell["ephemeris_info"]["ecef"]["position_y"]  = 1234567.0;
  cell["ephemeris_info"]["ecef"]["position_z"]  = 5432109.0;
  cell["ephemeris_info"]["ecef"]["velocity_vx"] = 1500.0;
  cell["ephemeris_info"]["ecef"]["velocity_vy"] = 3073.0;
  cell["ephemeris_info"]["ecef"]["velocity_vz"] = -2500.0;
  cell["polarization"]["dl"]                    = "circular"; // Not one of rhcp/lhcp/linear.
  req["cells"].push_back(cell);

  expected<nlohmann::json, std::string> result = cmd.execute(req);

  ASSERT_FALSE(result.has_value());
  logger.info("Received error: {}", result.error());
  EXPECT_TRUE(result.error().find("polarization") != std::string::npos ||
              result.error().find("Invalid") != std::string::npos)
      << "Expected error about invalid polarization type, got: " << result.error();
}

TEST_F(ntn_config_update_remote_command_test, when_one_cell_has_invalid_config_then_entire_request_is_rejected)
{
  // Create remote command.
  ntn_config_update_remote_command cmd(ntn_cfg_manager);

  // Use future timestamp.
  auto future_time_ms =
      std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
          .count() +
      10000ULL;

  // Build JSON request with multiple cells where one has invalid config.
  nlohmann::json req;
  req["cells"] = nlohmann::json::array();

  // Cell 1: Valid config.
  nlohmann::json cell1;
  cell1["plmn"]                                  = "00101";
  cell1["nci"]                                   = static_cast<uint64_t>(1);
  cell1["epoch_timestamp"]                       = future_time_ms;
  cell1["ntn_ul_sync_validity_duration"]         = 5;
  cell1["ephemeris_info"]["ecef"]["position_x"]  = 3578630.0;
  cell1["ephemeris_info"]["ecef"]["position_y"]  = 1234567.0;
  cell1["ephemeris_info"]["ecef"]["position_z"]  = 5432109.0;
  cell1["ephemeris_info"]["ecef"]["velocity_vx"] = 1500.0;
  cell1["ephemeris_info"]["ecef"]["velocity_vy"] = 3073.0;
  cell1["ephemeris_info"]["ecef"]["velocity_vz"] = -2500.0;

  // Cell 2: Invalid config (missing required field: epoch_timestamp).
  nlohmann::json cell2;
  cell2["plmn"] = "00101";
  cell2["nci"]  = static_cast<uint64_t>(2);
  // Missing epoch_timestamp - should cause parsing to fail
  cell2["ntn_ul_sync_validity_duration"]         = 5;
  cell2["ephemeris_info"]["ecef"]["position_x"]  = 4000000.0;
  cell2["ephemeris_info"]["ecef"]["position_y"]  = 2000000.0;
  cell2["ephemeris_info"]["ecef"]["position_z"]  = 3500000.0;
  cell2["ephemeris_info"]["ecef"]["velocity_vx"] = 2000.0;
  cell2["ephemeris_info"]["ecef"]["velocity_vy"] = 2500.0;
  cell2["ephemeris_info"]["ecef"]["velocity_vz"] = -1800.0;

  req["cells"].push_back(cell1);
  req["cells"].push_back(cell2);

  // Execute command.
  expected<nlohmann::json, std::string> result = cmd.execute(req);

  // Verify command failed - entire request should be rejected when one cell fails parsing.
  ASSERT_FALSE(result.has_value());
  logger.info("Received error: {}", result.error());
  EXPECT_TRUE(result.error().find("epoch_timestamp") != std::string::npos ||
              result.error().find("Failed to parse") != std::string::npos)
      << "Expected error about missing required field, got: " << result.error();
}

TEST_F(ntn_config_update_remote_command_test,
       when_common_config_with_one_invalid_cell_override_then_entire_request_is_rejected)
{
  // Create remote command.
  ntn_config_update_remote_command cmd(ntn_cfg_manager);

  // Use future timestamp.
  auto future_time_ms =
      std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
          .count() +
      10000ULL;

  // Build JSON request with common config and multiple cells, where one cell has invalid override.
  nlohmann::json req;

  // Common NTN config (valid).
  req["common_ntn_config"]["epoch_timestamp"]                       = future_time_ms;
  req["common_ntn_config"]["ntn_ul_sync_validity_duration"]         = 5;
  req["common_ntn_config"]["ephemeris_info"]["ecef"]["position_x"]  = 3578630.0;
  req["common_ntn_config"]["ephemeris_info"]["ecef"]["position_y"]  = 1234567.0;
  req["common_ntn_config"]["ephemeris_info"]["ecef"]["position_z"]  = 5432109.0;
  req["common_ntn_config"]["ephemeris_info"]["ecef"]["velocity_vx"] = 1500.0;
  req["common_ntn_config"]["ephemeris_info"]["ecef"]["velocity_vy"] = 3073.0;
  req["common_ntn_config"]["ephemeris_info"]["ecef"]["velocity_vz"] = -2500.0;

  // Cells array.
  req["cells"] = nlohmann::json::array();

  // Cell 1: Valid (uses common config).
  nlohmann::json cell1;
  cell1["plmn"] = "00101";
  cell1["nci"]  = static_cast<uint64_t>(1);

  // Cell 2: Invalid override (invalid PLMN format).
  nlohmann::json cell2;
  cell2["plmn"] = "invalid_plmn"; // Invalid PLMN should cause parsing to fail
  cell2["nci"]  = static_cast<uint64_t>(2);

  req["cells"].push_back(cell1);
  req["cells"].push_back(cell2);

  // Execute command.
  expected<nlohmann::json, std::string> result = cmd.execute(req);

  // Verify command failed - entire request should be rejected when one cell fails parsing.
  ASSERT_FALSE(result.has_value());
  logger.info("Received error: {}", result.error());
  EXPECT_TRUE(result.error().find("Failed to parse") != std::string::npos)
      << "Expected error about failed parsing, got: " << result.error();
}

TEST_F(ntn_config_update_remote_command_test, single_cell_update_from_json_string)
{
  // Create remote command.
  ntn_config_update_remote_command cmd(ntn_cfg_manager);

  // Calculate future timestamp.
  auto future_time_ms =
      std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
          .count() +
      10000ULL;

  auto t_service_ms = future_time_ms + 2000ULL;
  auto sat_start_ms = future_time_ms + 5000ULL;
  auto sat_epoch_ms = future_time_ms + 3000ULL;

  // Define NTN config update request as JSON string with all possible fields supported by WS update.
  std::string json_str = fmt::format(R"json(
{{
  "cells": [
    {{
      "plmn": "00101",
      "nci": 1,
      "epoch_timestamp": {},
      "ntn_ul_sync_validity_duration": 5,
      "ephemeris_info": {{
        "ecef": {{
          "position_x": 3578630.0,
          "position_y": 1234567.0,
          "position_z": 5432109.0,
          "velocity_vx": 1500.0,
          "velocity_vy": 3073.0,
          "velocity_vz": -2500.0
        }}
      }},
      "ta_info": {{
        "ta_common": 7874.95,
        "ta_common_drift": 0.5,
        "ta_common_drift_variant": 0.1
      }},
      "feeder_link_info": {{
        "enable_doppler_compensation": true,
        "dl_freq": 2185000000.0,
        "ul_freq": 1995000000.0
      }},
      "ntn_gateway_location": {{
        "latitude": 45.0,
        "longitude": 15.0,
        "altitude": 100.0
      }},
      "reference_location": {{
        "latitude": 44.5,
        "longitude": 14.5
      }},
      "distance_threshold": 10000,
      "t_service": {},
      "polarization": {{
        "dl": "lhcp",
        "ul": "rhcp"
      }},
      "ta_report": true,
      "moving_ref_location": {{
        "latitude": 44.1,
        "longitude": 14.1
      }},
      "ncells": [
        {{
          "pci": 2,
          "carrier_freq": 650000
        }},
        {{
          "pci": 3,
          "carrier_freq": 650001
        }}
      ],
      "sat_switch_with_resync": {{
        "epoch_timestamp": {},
        "ntn_gateway_location": {{
          "latitude": 46.1,
          "longitude": 16.2,
          "altitude": 120.0
        }},
        "t_service_start": {},
        "ssb_time_offset_sf": 60,
        "ntn_cfg": {{
          "cell_specific_koffset": 16,
          "k_mac": 32,
          "ntn_ul_sync_validity_dur": 10,
          "ta_report": true,
          "ta_info": {{
            "ta_common": 100.0,
            "ta_common_drift": 0.01,
            "ta_common_drift_variant": 0.001
          }},
          "polarization": {{
            "dl": "rhcp",
            "ul": "lhcp"
          }},
          "ephemeris_info": {{
            "orbital": {{
              "semi_major_axis": 7000000.0,
              "eccentricity": 0.002,
              "periapsis": 0.3,
              "longitude": 1.3,
              "inclination": 1.5,
              "mean_anomaly": 0.7
            }}
          }}
        }}
      }}
    }}
  ]
}}
)json",
                                     future_time_ms,
                                     t_service_ms,
                                     sat_epoch_ms,
                                     sat_start_ms);

  // Parse JSON string.
  nlohmann::json req = nlohmann::json::parse(json_str);
  logger.info("Parsed JSON request:\n{}", req.dump(2));

  // Setup expected result.
  nr_cell_global_id_t cgi;
  cgi.plmn_id = plmn_identity::parse("00101").value();
  cgi.nci     = nr_cell_identity::create(1).value();
  ntn_cfg_manager.result_to_return.succeeded.push_back(cgi);

  // Execute command.
  expected<nlohmann::json, std::string> result = cmd.execute(req);

  // Verify command succeeded.
  ASSERT_TRUE(result.has_value()) << "Command failed: " << result.error();
  ASSERT_EQ(ntn_cfg_manager.last_request.cells.size(), 1);
  EXPECT_EQ(ntn_cfg_manager.last_request.cells[0].nr_cgi.nci.value(), 1);

  // Verify epoch timestamp.
  auto epoch_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                      ntn_cfg_manager.last_request.cells[0].epoch_time.time_since_epoch())
                      .count();
  EXPECT_EQ(epoch_ms, future_time_ms);

  EXPECT_EQ(ntn_cfg_manager.last_request.cells[0].ntn_ul_sync_validity_duration, 5);

  // Verify all ECEF coordinates.
  EXPECT_TRUE(std::holds_alternative<ecef_coordinates_t>(ntn_cfg_manager.last_request.cells[0].ephemeris_info));
  auto& ecef = std::get<ecef_coordinates_t>(ntn_cfg_manager.last_request.cells[0].ephemeris_info);
  EXPECT_DOUBLE_EQ(ecef.position_x, 3578630.0);
  EXPECT_DOUBLE_EQ(ecef.position_y, 1234567.0);
  EXPECT_DOUBLE_EQ(ecef.position_z, 5432109.0);
  EXPECT_DOUBLE_EQ(ecef.velocity_vx, 1500.0);
  EXPECT_DOUBLE_EQ(ecef.velocity_vy, 3073.0);
  EXPECT_DOUBLE_EQ(ecef.velocity_vz, -2500.0);

  // Verify optional fields were parsed.
  ASSERT_TRUE(ntn_cfg_manager.last_request.cells[0].ta_info.has_value());
  EXPECT_DOUBLE_EQ(ntn_cfg_manager.last_request.cells[0].ta_info->ta_common, 7874.95);
  EXPECT_DOUBLE_EQ(ntn_cfg_manager.last_request.cells[0].ta_info->ta_common_drift, 0.5);
  EXPECT_DOUBLE_EQ(ntn_cfg_manager.last_request.cells[0].ta_info->ta_common_drift_variant, 0.1);

  ASSERT_TRUE(ntn_cfg_manager.last_request.cells[0].feeder_link_info.has_value());
  EXPECT_TRUE(ntn_cfg_manager.last_request.cells[0].feeder_link_info->enable_doppler_compensation);
  EXPECT_DOUBLE_EQ(ntn_cfg_manager.last_request.cells[0].feeder_link_info->dl_freq, 2185000000.0);
  EXPECT_DOUBLE_EQ(ntn_cfg_manager.last_request.cells[0].feeder_link_info->ul_freq, 1995000000.0);

  ASSERT_TRUE(ntn_cfg_manager.last_request.cells[0].ntn_gateway_location.has_value());
  EXPECT_DOUBLE_EQ(ntn_cfg_manager.last_request.cells[0].ntn_gateway_location->latitude, 45.0);
  EXPECT_DOUBLE_EQ(ntn_cfg_manager.last_request.cells[0].ntn_gateway_location->longitude, 15.0);
  EXPECT_DOUBLE_EQ(ntn_cfg_manager.last_request.cells[0].ntn_gateway_location->altitude, 100.0);

  ASSERT_TRUE(ntn_cfg_manager.last_request.cells[0].reference_location.has_value());
  EXPECT_DOUBLE_EQ(ntn_cfg_manager.last_request.cells[0].reference_location->latitude, 44.5);
  EXPECT_DOUBLE_EQ(ntn_cfg_manager.last_request.cells[0].reference_location->longitude, 14.5);

  ASSERT_TRUE(ntn_cfg_manager.last_request.cells[0].distance_threshold.has_value());
  EXPECT_EQ(ntn_cfg_manager.last_request.cells[0].distance_threshold.value(), 10000U);

  ASSERT_TRUE(ntn_cfg_manager.last_request.cells[0].t_service.has_value());
  auto t_service_parsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                                 ntn_cfg_manager.last_request.cells[0].t_service->time_since_epoch())
                                 .count();
  EXPECT_EQ(t_service_parsed_ms, t_service_ms);

  ASSERT_TRUE(ntn_cfg_manager.last_request.cells[0].polarization.has_value());
  ASSERT_TRUE(ntn_cfg_manager.last_request.cells[0].polarization->dl.has_value());
  ASSERT_TRUE(ntn_cfg_manager.last_request.cells[0].polarization->ul.has_value());
  EXPECT_EQ(ntn_cfg_manager.last_request.cells[0].polarization->dl.value(),
            ntn_polarization_t::polarization_type::lhcp);
  EXPECT_EQ(ntn_cfg_manager.last_request.cells[0].polarization->ul.value(),
            ntn_polarization_t::polarization_type::rhcp);

  ASSERT_TRUE(ntn_cfg_manager.last_request.cells[0].ta_report.has_value());
  EXPECT_TRUE(ntn_cfg_manager.last_request.cells[0].ta_report.value());

  ASSERT_TRUE(ntn_cfg_manager.last_request.cells[0].moving_ref_location.has_value());
  EXPECT_DOUBLE_EQ(ntn_cfg_manager.last_request.cells[0].moving_ref_location->latitude, 44.1);
  EXPECT_DOUBLE_EQ(ntn_cfg_manager.last_request.cells[0].moving_ref_location->longitude, 14.1);

  ASSERT_TRUE(ntn_cfg_manager.last_request.cells[0].ncells.has_value());
  ASSERT_EQ(ntn_cfg_manager.last_request.cells[0].ncells->size(), 2);
  ASSERT_TRUE((*ntn_cfg_manager.last_request.cells[0].ncells)[0].phys_cell_id.has_value());
  ASSERT_TRUE((*ntn_cfg_manager.last_request.cells[0].ncells)[0].carrier_freq.has_value());
  EXPECT_EQ((*ntn_cfg_manager.last_request.cells[0].ncells)[0].phys_cell_id.value(), 2);
  EXPECT_EQ((*ntn_cfg_manager.last_request.cells[0].ncells)[0].carrier_freq->value(), 650000);
  ASSERT_TRUE((*ntn_cfg_manager.last_request.cells[0].ncells)[1].phys_cell_id.has_value());
  ASSERT_TRUE((*ntn_cfg_manager.last_request.cells[0].ncells)[1].carrier_freq.has_value());
  EXPECT_EQ((*ntn_cfg_manager.last_request.cells[0].ncells)[1].phys_cell_id.value(), 3);
  EXPECT_EQ((*ntn_cfg_manager.last_request.cells[0].ncells)[1].carrier_freq->value(), 650001);

  ASSERT_TRUE(ntn_cfg_manager.last_request.cells[0].sat_switch_with_resync.has_value());
  const auto& sat_sw = *ntn_cfg_manager.last_request.cells[0].sat_switch_with_resync;
  ASSERT_TRUE(sat_sw.t_service_start.has_value());
  auto sat_start_parsed_ms =
      std::chrono::duration_cast<std::chrono::milliseconds>(sat_sw.t_service_start->time_since_epoch()).count();
  EXPECT_EQ(sat_start_parsed_ms, sat_start_ms);
  ASSERT_TRUE(sat_sw.epoch_timestamp.has_value());
  auto sat_epoch_parsed_ms =
      std::chrono::duration_cast<std::chrono::milliseconds>(sat_sw.epoch_timestamp->time_since_epoch()).count();
  EXPECT_EQ(sat_epoch_parsed_ms, sat_epoch_ms);
  ASSERT_TRUE(sat_sw.ntn_gateway_location.has_value());
  EXPECT_DOUBLE_EQ(sat_sw.ntn_gateway_location->latitude, 46.1);
  EXPECT_DOUBLE_EQ(sat_sw.ntn_gateway_location->longitude, 16.2);
  EXPECT_DOUBLE_EQ(sat_sw.ntn_gateway_location->altitude, 120.0);
  ASSERT_TRUE(sat_sw.ssb_time_offset_sf.has_value());
  EXPECT_EQ(sat_sw.ssb_time_offset_sf->value(), 60);

  ASSERT_TRUE(sat_sw.ntn_cfg.cell_specific_koffset.has_value());
  EXPECT_EQ(sat_sw.ntn_cfg.cell_specific_koffset->count(), 16);
  ASSERT_TRUE(sat_sw.ntn_cfg.k_mac.has_value());
  EXPECT_EQ(sat_sw.ntn_cfg.k_mac->count(), 32);
  ASSERT_TRUE(sat_sw.ntn_cfg.ntn_ul_sync_validity_dur.has_value());
  EXPECT_EQ(sat_sw.ntn_cfg.ntn_ul_sync_validity_dur.value(), 10U);
  ASSERT_TRUE(sat_sw.ntn_cfg.ta_report.has_value());
  EXPECT_TRUE(sat_sw.ntn_cfg.ta_report.value());
  ASSERT_TRUE(sat_sw.ntn_cfg.ta_info.has_value());
  EXPECT_DOUBLE_EQ(sat_sw.ntn_cfg.ta_info->ta_common, 100.0);
  EXPECT_DOUBLE_EQ(sat_sw.ntn_cfg.ta_info->ta_common_drift, 0.01);
  EXPECT_DOUBLE_EQ(sat_sw.ntn_cfg.ta_info->ta_common_drift_variant, 0.001);
  ASSERT_TRUE(sat_sw.ntn_cfg.polarization.has_value());
  ASSERT_TRUE(sat_sw.ntn_cfg.polarization->dl.has_value());
  ASSERT_TRUE(sat_sw.ntn_cfg.polarization->ul.has_value());
  EXPECT_EQ(sat_sw.ntn_cfg.polarization->dl.value(), ntn_polarization_t::polarization_type::rhcp);
  EXPECT_EQ(sat_sw.ntn_cfg.polarization->ul.value(), ntn_polarization_t::polarization_type::lhcp);
  ASSERT_TRUE(sat_sw.ntn_cfg.ephemeris_info.has_value());
  EXPECT_TRUE(std::holds_alternative<orbital_coordinates_t>(sat_sw.ntn_cfg.ephemeris_info.value()));
  const auto& sat_orb = std::get<orbital_coordinates_t>(sat_sw.ntn_cfg.ephemeris_info.value());
  EXPECT_DOUBLE_EQ(sat_orb.semi_major_axis, 7000000.0);
  EXPECT_DOUBLE_EQ(sat_orb.eccentricity, 0.002);
  EXPECT_DOUBLE_EQ(sat_orb.periapsis, 0.3);
  EXPECT_DOUBLE_EQ(sat_orb.longitude, 1.3);
  EXPECT_DOUBLE_EQ(sat_orb.inclination, 1.5);
  EXPECT_DOUBLE_EQ(sat_orb.mean_anomaly, 0.7);
}

TEST_F(ntn_config_update_remote_command_test, single_cell_update_with_iso_timestamp_string)
{
  // Create remote command.
  ntn_config_update_remote_command cmd(ntn_cfg_manager);

  // Generate future ISO timestamp string (current time + 10 seconds, without milliseconds).
  auto        future_time       = std::chrono::system_clock::now() + std::chrono::seconds(10);
  std::time_t time_t_future     = std::chrono::system_clock::to_time_t(future_time);
  int64_t     expected_epoch_ms = static_cast<int64_t>(time_t_future) * 1000; // No milliseconds
  std::tm     tm_future         = *std::gmtime(&time_t_future);
  char        iso_buffer[32];
  std::strftime(iso_buffer, sizeof(iso_buffer), "%Y-%m-%dT%H:%M:%S", &tm_future);
  std::string iso_timestamp = std::string(iso_buffer);

  // Build request with ISO timestamp string.
  nlohmann::json req;
  req["cells"] = nlohmann::json::array();

  nlohmann::json cell;
  cell["plmn"]                                  = "00101";
  cell["nci"]                                   = static_cast<uint64_t>(1);
  cell["epoch_timestamp"]                       = iso_timestamp; // ISO format without milliseconds
  cell["ntn_ul_sync_validity_duration"]         = 5;
  cell["ephemeris_info"]["ecef"]["position_x"]  = 3578630.0;
  cell["ephemeris_info"]["ecef"]["position_y"]  = 1234567.0;
  cell["ephemeris_info"]["ecef"]["position_z"]  = 5432109.0;
  cell["ephemeris_info"]["ecef"]["velocity_vx"] = 1500.0;
  cell["ephemeris_info"]["ecef"]["velocity_vy"] = 3073.0;
  cell["ephemeris_info"]["ecef"]["velocity_vz"] = -2500.0;

  req["cells"].push_back(cell);

  // Setup expected result.
  nr_cell_global_id_t cgi;
  cgi.plmn_id = plmn_identity::parse("00101").value();
  cgi.nci     = nr_cell_identity::create(1).value();
  ntn_cfg_manager.result_to_return.succeeded.push_back(cgi);

  // Execute command.
  expected<nlohmann::json, std::string> result = cmd.execute(req);

  // Verify command succeeded.
  ASSERT_TRUE(result.has_value()) << "Command failed: " << result.error();
  ASSERT_EQ(ntn_cfg_manager.last_request.cells.size(), 1);
  EXPECT_EQ(ntn_cfg_manager.last_request.cells[0].nr_cgi.nci.value(), 1);

  // Verify timestamp parsed correctly from ISO format (without milliseconds).
  auto epoch_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                      ntn_cfg_manager.last_request.cells[0].epoch_time.time_since_epoch())
                      .count();
  EXPECT_EQ(epoch_ms, expected_epoch_ms);

  // Verify ntn_ul_sync_validity_duration.
  EXPECT_EQ(ntn_cfg_manager.last_request.cells[0].ntn_ul_sync_validity_duration, 5);

  // Verify all ECEF coordinates.
  EXPECT_TRUE(std::holds_alternative<ecef_coordinates_t>(ntn_cfg_manager.last_request.cells[0].ephemeris_info));
  auto& ecef = std::get<ecef_coordinates_t>(ntn_cfg_manager.last_request.cells[0].ephemeris_info);
  EXPECT_DOUBLE_EQ(ecef.position_x, 3578630.0);
  EXPECT_DOUBLE_EQ(ecef.position_y, 1234567.0);
  EXPECT_DOUBLE_EQ(ecef.position_z, 5432109.0);
  EXPECT_DOUBLE_EQ(ecef.velocity_vx, 1500.0);
  EXPECT_DOUBLE_EQ(ecef.velocity_vy, 3073.0);
  EXPECT_DOUBLE_EQ(ecef.velocity_vz, -2500.0);
}

TEST_F(ntn_config_update_remote_command_test, multi_cell_update_from_json_string)
{
  // Create remote command.
  ntn_config_update_remote_command cmd(ntn_cfg_manager);

  // Calculate future timestamp in milliseconds for cell 1.
  auto future_time_ms =
      std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
          .count() +
      10000ULL;

  // Generate future ISO timestamp string for cell 2 (current time + 20 seconds).
  auto        future_time        = std::chrono::system_clock::now() + std::chrono::seconds(20);
  std::time_t time_t_future      = std::chrono::system_clock::to_time_t(future_time);
  int64_t     expected_epoch2_ms = static_cast<int64_t>(time_t_future) * 1000; // No milliseconds
  std::tm     tm_future          = *std::gmtime(&time_t_future);
  char        iso_buffer[32];
  std::strftime(iso_buffer, sizeof(iso_buffer), "%Y-%m-%dT%H:%M:%S", &tm_future);
  std::string iso_timestamp = std::string(iso_buffer);

  // Define multi-cell NTN config update request as JSON string.
  // Cell 1 uses milliseconds (number), Cell 2 uses ISO timestamp (string).
  std::string json_str = fmt::format(R"json(
{{
  "cells": [
    {{
      "plmn": "00101",
      "nci": 1,
      "epoch_timestamp": {},
      "ntn_ul_sync_validity_duration": 5,
      "ephemeris_info": {{
        "ecef": {{
          "position_x": 3578630.0,
          "position_y": 1234567.0,
          "position_z": 5432109.0,
          "velocity_vx": 1500.0,
          "velocity_vy": 3073.0,
          "velocity_vz": -2500.0
        }}
      }}
    }},
    {{
      "plmn": "00101",
      "nci": 2,
      "epoch_timestamp": "{}",
      "ntn_ul_sync_validity_duration": 5,
      "ephemeris_info": {{
        "orbital": {{
          "semi_major_axis": 6917000.0,
          "eccentricity": 0.001,
          "periapsis": 0.5236,
          "longitude": 1.0472,
          "inclination": 1.5708,
          "mean_anomaly": 0.7854
        }}
      }}
    }}
  ]
}}
)json",
                                     future_time_ms,
                                     iso_timestamp);

  // Parse JSON string.
  nlohmann::json req = nlohmann::json::parse(json_str);
  logger.info("Parsed multi-cell JSON request:\n{}", req.dump(2));

  // Setup expected result.
  nr_cell_global_id_t cgi1, cgi2;
  cgi1.plmn_id = plmn_identity::parse("00101").value();
  cgi1.nci     = nr_cell_identity::create(1).value();
  cgi2.plmn_id = plmn_identity::parse("00101").value();
  cgi2.nci     = nr_cell_identity::create(2).value();

  ntn_cfg_manager.result_to_return.succeeded.push_back(cgi1);
  ntn_cfg_manager.result_to_return.succeeded.push_back(cgi2);

  // Execute command.
  expected<nlohmann::json, std::string> result = cmd.execute(req);

  // Verify command succeeded.
  ASSERT_TRUE(result.has_value()) << "Command failed: " << result.error();
  ASSERT_EQ(ntn_cfg_manager.last_request.cells.size(), 2);

  // Verify first cell (uses milliseconds timestamp).
  EXPECT_EQ(ntn_cfg_manager.last_request.cells[0].nr_cgi.nci.value(), 1);

  // Verify epoch timestamp for cell 1.
  auto epoch1_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                       ntn_cfg_manager.last_request.cells[0].epoch_time.time_since_epoch())
                       .count();
  EXPECT_EQ(epoch1_ms, future_time_ms);

  EXPECT_EQ(ntn_cfg_manager.last_request.cells[0].ntn_ul_sync_validity_duration, 5);

  // Verify all ECEF coordinates for cell 1.
  auto& ecef1 = std::get<ecef_coordinates_t>(ntn_cfg_manager.last_request.cells[0].ephemeris_info);
  EXPECT_DOUBLE_EQ(ecef1.position_x, 3578630.0);
  EXPECT_DOUBLE_EQ(ecef1.position_y, 1234567.0);
  EXPECT_DOUBLE_EQ(ecef1.position_z, 5432109.0);
  EXPECT_DOUBLE_EQ(ecef1.velocity_vx, 1500.0);
  EXPECT_DOUBLE_EQ(ecef1.velocity_vy, 3073.0);
  EXPECT_DOUBLE_EQ(ecef1.velocity_vz, -2500.0);

  // Verify second cell (uses orbital parameters and ISO timestamp string).
  EXPECT_EQ(ntn_cfg_manager.last_request.cells[1].nr_cgi.nci.value(), 2);

  // Verify epoch timestamp for cell 2 (parsed from ISO string).
  auto epoch2_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                       ntn_cfg_manager.last_request.cells[1].epoch_time.time_since_epoch())
                       .count();
  EXPECT_EQ(epoch2_ms, expected_epoch2_ms);

  EXPECT_EQ(ntn_cfg_manager.last_request.cells[1].ntn_ul_sync_validity_duration, 5);
  EXPECT_TRUE(std::holds_alternative<orbital_coordinates_t>(ntn_cfg_manager.last_request.cells[1].ephemeris_info));

  // Verify all orbital parameters for cell 2.
  auto& orbital2 = std::get<orbital_coordinates_t>(ntn_cfg_manager.last_request.cells[1].ephemeris_info);
  EXPECT_DOUBLE_EQ(orbital2.semi_major_axis, 6917000.0);
  EXPECT_DOUBLE_EQ(orbital2.eccentricity, 0.001);
  EXPECT_DOUBLE_EQ(orbital2.periapsis, 0.5236);
  EXPECT_DOUBLE_EQ(orbital2.longitude, 1.0472);
  EXPECT_DOUBLE_EQ(orbital2.inclination, 1.5708);
  EXPECT_DOUBLE_EQ(orbital2.mean_anomaly, 0.7854);
}

TEST_F(ntn_config_update_remote_command_test, common_config_with_overrides_from_json_string)
{
  // Create remote command.
  ntn_config_update_remote_command cmd(ntn_cfg_manager);

  // Calculate future timestamp in milliseconds.
  auto future_time_ms =
      std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
          .count() +
      10000ULL;

  // Generate future ISO timestamp string with milliseconds for cell 3 (current time + 20 seconds).
  auto        future_time    = std::chrono::system_clock::now() + std::chrono::seconds(20);
  std::time_t time_t_future  = std::chrono::system_clock::to_time_t(future_time);
  auto        ms_since_epoch = std::chrono::duration_cast<std::chrono::milliseconds>(future_time.time_since_epoch());
  int64_t     expected_epoch3_ms = ms_since_epoch.count(); // Save expected value for verification
  int         milliseconds_part  = expected_epoch3_ms % 1000;
  std::tm     tm_future          = *std::gmtime(&time_t_future);
  char        iso_buffer[32];
  std::strftime(iso_buffer, sizeof(iso_buffer), "%Y-%m-%dT%H:%M:%S", &tm_future);
  std::string iso_timestamp = fmt::format("{}.{:03d}", iso_buffer, milliseconds_part);

  // Define common config with per-cell overrides as JSON string.
  // Cell 2 uses milliseconds override, Cell 3 uses ISO timestamp override.
  std::string json_str = fmt::format(R"json(
{{
  "common_ntn_config": {{
    "epoch_timestamp": {},
    "ntn_ul_sync_validity_duration": 5,
    "ephemeris_info": {{
      "ecef": {{
        "position_x": 3578630.0,
        "position_y": 1234567.0,
        "position_z": 5432109.0,
        "velocity_vx": 1500.0,
        "velocity_vy": 3073.0,
        "velocity_vz": -2500.0
      }}
    }}
  }},
  "cells": [
    {{
      "plmn": "00101",
      "nci": 1
    }},
    {{
      "plmn": "00101",
      "nci": 2,
      "epoch_timestamp": {}
    }},
    {{
      "plmn": "00101",
      "nci": 3,
      "epoch_timestamp": "{}",
      "ephemeris_info": {{
        "orbital": {{
          "semi_major_axis": 6917000.0,
          "eccentricity": 0.001,
          "periapsis": 0.5236,
          "longitude": 1.0472,
          "inclination": 1.5708,
          "mean_anomaly": 0.7854
        }}
      }}
    }}
  ]
}}
)json",
                                     future_time_ms,
                                     future_time_ms + 10000ULL,
                                     iso_timestamp);

  // Parse JSON string.
  nlohmann::json req = nlohmann::json::parse(json_str);
  logger.info("Parsed common config with overrides JSON request:\n{}", req.dump(2));

  // Setup expected result.
  nr_cell_global_id_t cgi1, cgi2, cgi3;
  cgi1.plmn_id = plmn_identity::parse("00101").value();
  cgi1.nci     = nr_cell_identity::create(1).value();
  cgi2.plmn_id = plmn_identity::parse("00101").value();
  cgi2.nci     = nr_cell_identity::create(2).value();
  cgi3.plmn_id = plmn_identity::parse("00101").value();
  cgi3.nci     = nr_cell_identity::create(3).value();

  ntn_cfg_manager.result_to_return.succeeded.push_back(cgi1);
  ntn_cfg_manager.result_to_return.succeeded.push_back(cgi2);
  ntn_cfg_manager.result_to_return.succeeded.push_back(cgi3);

  // Execute command.
  expected<nlohmann::json, std::string> result = cmd.execute(req);

  // Verify command succeeded.
  ASSERT_TRUE(result.has_value()) << "Command failed: " << result.error();
  ASSERT_EQ(ntn_cfg_manager.last_request.cells.size(), 3);

  // Verify Cell 1: Uses common config.
  EXPECT_EQ(ntn_cfg_manager.last_request.cells[0].nr_cgi.nci.value(), 1);
  auto epoch1_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                       ntn_cfg_manager.last_request.cells[0].epoch_time.time_since_epoch())
                       .count();
  EXPECT_EQ(epoch1_ms, future_time_ms);
  EXPECT_EQ(ntn_cfg_manager.last_request.cells[0].ntn_ul_sync_validity_duration, 5);
  EXPECT_TRUE(std::holds_alternative<ecef_coordinates_t>(ntn_cfg_manager.last_request.cells[0].ephemeris_info));
  auto& ecef1 = std::get<ecef_coordinates_t>(ntn_cfg_manager.last_request.cells[0].ephemeris_info);
  EXPECT_DOUBLE_EQ(ecef1.position_x, 3578630.0);
  EXPECT_DOUBLE_EQ(ecef1.position_y, 1234567.0);
  EXPECT_DOUBLE_EQ(ecef1.position_z, 5432109.0);
  EXPECT_DOUBLE_EQ(ecef1.velocity_vx, 1500.0);
  EXPECT_DOUBLE_EQ(ecef1.velocity_vy, 3073.0);
  EXPECT_DOUBLE_EQ(ecef1.velocity_vz, -2500.0);

  // Verify Cell 2: Overridden epoch_timestamp, other parameters from common config.
  EXPECT_EQ(ntn_cfg_manager.last_request.cells[1].nr_cgi.nci.value(), 2);

  // Verify epoch timestamp (cell-specific override).
  auto epoch2_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                       ntn_cfg_manager.last_request.cells[1].epoch_time.time_since_epoch())
                       .count();
  EXPECT_EQ(epoch2_ms, future_time_ms + 10000ULL);

  // Verify ntn_ul_sync_validity_duration (from common config).
  EXPECT_EQ(ntn_cfg_manager.last_request.cells[1].ntn_ul_sync_validity_duration, 5);

  // Verify all ECEF coordinates (from common config).
  EXPECT_TRUE(std::holds_alternative<ecef_coordinates_t>(ntn_cfg_manager.last_request.cells[1].ephemeris_info));
  auto& ecef2 = std::get<ecef_coordinates_t>(ntn_cfg_manager.last_request.cells[1].ephemeris_info);
  EXPECT_DOUBLE_EQ(ecef2.position_x, 3578630.0);
  EXPECT_DOUBLE_EQ(ecef2.position_y, 1234567.0);
  EXPECT_DOUBLE_EQ(ecef2.position_z, 5432109.0);
  EXPECT_DOUBLE_EQ(ecef2.velocity_vx, 1500.0);
  EXPECT_DOUBLE_EQ(ecef2.velocity_vy, 3073.0);
  EXPECT_DOUBLE_EQ(ecef2.velocity_vz, -2500.0);

  // Verify Cell 3: Overridden epoch_timestamp (ISO format with milliseconds) and ephemeris_info,
  // ntn_ul_sync_validity_duration from common config.
  EXPECT_EQ(ntn_cfg_manager.last_request.cells[2].nr_cgi.nci.value(), 3);

  // Verify epoch timestamp (cell-specific override).
  auto epoch3_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                       ntn_cfg_manager.last_request.cells[2].epoch_time.time_since_epoch())
                       .count();
  EXPECT_EQ(epoch3_ms, expected_epoch3_ms);

  // Verify ntn_ul_sync_validity_duration (from common config).
  EXPECT_EQ(ntn_cfg_manager.last_request.cells[2].ntn_ul_sync_validity_duration, 5);

  // Verify all orbital parameters (cell-specific override).
  EXPECT_TRUE(std::holds_alternative<orbital_coordinates_t>(ntn_cfg_manager.last_request.cells[2].ephemeris_info));
  auto& orbital3 = std::get<orbital_coordinates_t>(ntn_cfg_manager.last_request.cells[2].ephemeris_info);
  EXPECT_DOUBLE_EQ(orbital3.semi_major_axis, 6917000.0);
  EXPECT_DOUBLE_EQ(orbital3.eccentricity, 0.001);
  EXPECT_DOUBLE_EQ(orbital3.periapsis, 0.5236);
  EXPECT_DOUBLE_EQ(orbital3.longitude, 1.0472);
  EXPECT_DOUBLE_EQ(orbital3.inclination, 1.5708);
  EXPECT_DOUBLE_EQ(orbital3.mean_anomaly, 0.7854);
}

TEST_F(ntn_config_update_remote_command_test, single_cell_update_with_ncells_pci_and_carrier_freq)
{
  ntn_config_update_remote_command cmd(ntn_cfg_manager);

  auto future_time_ms =
      std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
          .count() +
      10000ULL;

  nlohmann::json req;
  req["cells"] = nlohmann::json::array();

  nlohmann::json cell;
  cell["plmn"]                                  = "00101";
  cell["nci"]                                   = static_cast<uint64_t>(1);
  cell["epoch_timestamp"]                       = future_time_ms;
  cell["ntn_ul_sync_validity_duration"]         = 5;
  cell["ephemeris_info"]["ecef"]["position_x"]  = 3578630.0;
  cell["ephemeris_info"]["ecef"]["position_y"]  = 1234567.0;
  cell["ephemeris_info"]["ecef"]["position_z"]  = 5432109.0;
  cell["ephemeris_info"]["ecef"]["velocity_vx"] = 1500.0;
  cell["ephemeris_info"]["ecef"]["velocity_vy"] = 3073.0;
  cell["ephemeris_info"]["ecef"]["velocity_vz"] = -2500.0;
  cell["ncells"] = nlohmann::json::array({{{"pci", static_cast<uint64_t>(2)}, {"carrier_freq", 650000U}},
                                          {{"pci", static_cast<uint64_t>(3)}, {"carrier_freq", 650001U}}});
  req["cells"].push_back(cell);

  nr_cell_global_id_t cgi;
  cgi.plmn_id = plmn_identity::parse("00101").value();
  cgi.nci     = nr_cell_identity::create(1).value();
  ntn_cfg_manager.result_to_return.succeeded.push_back(cgi);

  expected<nlohmann::json, std::string> result = cmd.execute(req);

  ASSERT_TRUE(result.has_value()) << "Command failed: " << result.error();
  ASSERT_EQ(ntn_cfg_manager.last_request.cells.size(), 1);
  ASSERT_TRUE(ntn_cfg_manager.last_request.cells[0].ncells.has_value());
  ASSERT_EQ(ntn_cfg_manager.last_request.cells[0].ncells->size(), 2);

  ASSERT_TRUE((*ntn_cfg_manager.last_request.cells[0].ncells)[0].phys_cell_id.has_value());
  EXPECT_EQ((*ntn_cfg_manager.last_request.cells[0].ncells)[0].phys_cell_id.value(), 2);
  ASSERT_TRUE((*ntn_cfg_manager.last_request.cells[0].ncells)[0].carrier_freq.has_value());
  EXPECT_EQ((*ntn_cfg_manager.last_request.cells[0].ncells)[0].carrier_freq->value(), 650000);

  ASSERT_TRUE((*ntn_cfg_manager.last_request.cells[0].ncells)[1].phys_cell_id.has_value());
  EXPECT_EQ((*ntn_cfg_manager.last_request.cells[0].ncells)[1].phys_cell_id.value(), 3);
  ASSERT_TRUE((*ntn_cfg_manager.last_request.cells[0].ncells)[1].carrier_freq.has_value());
  EXPECT_EQ((*ntn_cfg_manager.last_request.cells[0].ncells)[1].carrier_freq->value(), 650001);
}

TEST_F(ntn_config_update_remote_command_test, single_cell_update_with_ncells_partial_entries)
{
  ntn_config_update_remote_command cmd(ntn_cfg_manager);

  auto future_time_ms =
      std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
          .count() +
      10000ULL;

  nlohmann::json req;
  req["cells"] = nlohmann::json::array();

  nlohmann::json cell;
  cell["plmn"]                                  = "00101";
  cell["nci"]                                   = static_cast<uint64_t>(1);
  cell["epoch_timestamp"]                       = future_time_ms;
  cell["ntn_ul_sync_validity_duration"]         = 5;
  cell["ephemeris_info"]["ecef"]["position_x"]  = 3578630.0;
  cell["ephemeris_info"]["ecef"]["position_y"]  = 1234567.0;
  cell["ephemeris_info"]["ecef"]["position_z"]  = 5432109.0;
  cell["ephemeris_info"]["ecef"]["velocity_vx"] = 1500.0;
  cell["ephemeris_info"]["ecef"]["velocity_vy"] = 3073.0;
  cell["ephemeris_info"]["ecef"]["velocity_vz"] = -2500.0;
  cell["ncells"] = nlohmann::json::array({{{"pci", static_cast<uint64_t>(7)}}, {{"carrier_freq", 700123U}}});
  req["cells"].push_back(cell);

  nr_cell_global_id_t cgi;
  cgi.plmn_id = plmn_identity::parse("00101").value();
  cgi.nci     = nr_cell_identity::create(1).value();
  ntn_cfg_manager.result_to_return.succeeded.push_back(cgi);

  expected<nlohmann::json, std::string> result = cmd.execute(req);

  ASSERT_TRUE(result.has_value()) << "Command failed: " << result.error();
  ASSERT_EQ(ntn_cfg_manager.last_request.cells.size(), 1);
  ASSERT_TRUE(ntn_cfg_manager.last_request.cells[0].ncells.has_value());
  ASSERT_EQ(ntn_cfg_manager.last_request.cells[0].ncells->size(), 2);

  ASSERT_TRUE((*ntn_cfg_manager.last_request.cells[0].ncells)[0].phys_cell_id.has_value());
  EXPECT_EQ((*ntn_cfg_manager.last_request.cells[0].ncells)[0].phys_cell_id.value(), 7);
  EXPECT_FALSE((*ntn_cfg_manager.last_request.cells[0].ncells)[0].carrier_freq.has_value());

  EXPECT_FALSE((*ntn_cfg_manager.last_request.cells[0].ncells)[1].phys_cell_id.has_value());
  ASSERT_TRUE((*ntn_cfg_manager.last_request.cells[0].ncells)[1].carrier_freq.has_value());
  EXPECT_EQ((*ntn_cfg_manager.last_request.cells[0].ncells)[1].carrier_freq->value(), 700123);
}

TEST_F(ntn_config_update_remote_command_test, single_cell_update_with_sat_switch_ntn_cfg_fields)
{
  ntn_config_update_remote_command cmd(ntn_cfg_manager);

  auto future_time_ms =
      std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
          .count() +
      10000ULL;
  auto sat_start_ms = future_time_ms + 5000ULL;
  auto sat_epoch_ms = future_time_ms + 3000ULL;

  nlohmann::json req;
  req["cells"] = nlohmann::json::array();

  nlohmann::json cell;
  cell["plmn"]                                                          = "00101";
  cell["nci"]                                                           = static_cast<uint64_t>(1);
  cell["epoch_timestamp"]                                               = future_time_ms;
  cell["ntn_ul_sync_validity_duration"]                                 = 5;
  cell["ephemeris_info"]["ecef"]["position_x"]                          = 3578630.0;
  cell["ephemeris_info"]["ecef"]["position_y"]                          = 1234567.0;
  cell["ephemeris_info"]["ecef"]["position_z"]                          = 5432109.0;
  cell["ephemeris_info"]["ecef"]["velocity_vx"]                         = 1500.0;
  cell["ephemeris_info"]["ecef"]["velocity_vy"]                         = 3073.0;
  cell["ephemeris_info"]["ecef"]["velocity_vz"]                         = -2500.0;
  cell["sat_switch_with_resync"]["t_service_start"]                     = sat_start_ms;
  cell["sat_switch_with_resync"]["epoch_timestamp"]                     = sat_epoch_ms;
  cell["sat_switch_with_resync"]["ntn_gateway_location"]["latitude"]    = 46.1;
  cell["sat_switch_with_resync"]["ntn_gateway_location"]["longitude"]   = 16.2;
  cell["sat_switch_with_resync"]["ntn_gateway_location"]["altitude"]    = 120.0;
  cell["sat_switch_with_resync"]["ssb_time_offset_sf"]                  = 60U;
  cell["sat_switch_with_resync"]["ntn_cfg"]["cell_specific_koffset"]    = 16U;
  cell["sat_switch_with_resync"]["ntn_cfg"]["k_mac"]                    = 32U;
  cell["sat_switch_with_resync"]["ntn_cfg"]["ntn_ul_sync_validity_dur"] = 10U;
  cell["sat_switch_with_resync"]["ntn_cfg"]["ta_report"]                = true;
  req["cells"].push_back(cell);

  nr_cell_global_id_t cgi;
  cgi.plmn_id = plmn_identity::parse("00101").value();
  cgi.nci     = nr_cell_identity::create(1).value();
  ntn_cfg_manager.result_to_return.succeeded.push_back(cgi);

  expected<nlohmann::json, std::string> result = cmd.execute(req);

  ASSERT_TRUE(result.has_value()) << "Command failed: " << result.error();
  ASSERT_EQ(ntn_cfg_manager.last_request.cells.size(), 1);
  ASSERT_TRUE(ntn_cfg_manager.last_request.cells[0].sat_switch_with_resync.has_value());

  const auto& sat_sw = *ntn_cfg_manager.last_request.cells[0].sat_switch_with_resync;
  ASSERT_TRUE(sat_sw.t_service_start.has_value());
  auto parsed_start_ms =
      std::chrono::duration_cast<std::chrono::milliseconds>(sat_sw.t_service_start->time_since_epoch()).count();
  EXPECT_EQ(parsed_start_ms, sat_start_ms);
  ASSERT_TRUE(sat_sw.epoch_timestamp.has_value());
  auto parsed_epoch_ms =
      std::chrono::duration_cast<std::chrono::milliseconds>(sat_sw.epoch_timestamp->time_since_epoch()).count();
  EXPECT_EQ(parsed_epoch_ms, sat_epoch_ms);
  ASSERT_TRUE(sat_sw.ntn_gateway_location.has_value());
  EXPECT_DOUBLE_EQ(sat_sw.ntn_gateway_location->latitude, 46.1);
  EXPECT_DOUBLE_EQ(sat_sw.ntn_gateway_location->longitude, 16.2);
  EXPECT_DOUBLE_EQ(sat_sw.ntn_gateway_location->altitude, 120.0);
  ASSERT_TRUE(sat_sw.ssb_time_offset_sf.has_value());
  EXPECT_EQ(sat_sw.ssb_time_offset_sf->value(), 60);

  ASSERT_TRUE(sat_sw.ntn_cfg.cell_specific_koffset.has_value());
  EXPECT_EQ(sat_sw.ntn_cfg.cell_specific_koffset->count(), 16);
  ASSERT_TRUE(sat_sw.ntn_cfg.k_mac.has_value());
  EXPECT_EQ(sat_sw.ntn_cfg.k_mac->count(), 32);
  ASSERT_TRUE(sat_sw.ntn_cfg.ntn_ul_sync_validity_dur.has_value());
  EXPECT_EQ(sat_sw.ntn_cfg.ntn_ul_sync_validity_dur.value(), 10);
  ASSERT_TRUE(sat_sw.ntn_cfg.ta_report.has_value());
  EXPECT_TRUE(sat_sw.ntn_cfg.ta_report.value());
}

TEST_F(ntn_config_update_remote_command_test, when_epoch_timestamp_is_in_past_then_error_is_returned)
{
  ntn_config_update_remote_command cmd(ntn_cfg_manager);

  auto past_time_ms =
      std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
          .count() -
      10000ULL;

  nlohmann::json req;
  req["cells"] = nlohmann::json::array();

  nlohmann::json cell;
  cell["plmn"]                                  = "00101";
  cell["nci"]                                   = static_cast<uint64_t>(1);
  cell["epoch_timestamp"]                       = past_time_ms;
  cell["ntn_ul_sync_validity_duration"]         = 5;
  cell["ephemeris_info"]["ecef"]["position_x"]  = 3578630.0;
  cell["ephemeris_info"]["ecef"]["position_y"]  = 1234567.0;
  cell["ephemeris_info"]["ecef"]["position_z"]  = 5432109.0;
  cell["ephemeris_info"]["ecef"]["velocity_vx"] = 1500.0;
  cell["ephemeris_info"]["ecef"]["velocity_vy"] = 3073.0;
  cell["ephemeris_info"]["ecef"]["velocity_vz"] = -2500.0;
  req["cells"].push_back(cell);

  expected<nlohmann::json, std::string> result = cmd.execute(req);

  ASSERT_FALSE(result.has_value());
  logger.info("Received error: {}", result.error());
  EXPECT_TRUE(result.error().find("past") != std::string::npos)
      << "Expected error about past timestamp, got: " << result.error();
}

TEST_F(ntn_config_update_remote_command_test, when_ephemeris_info_has_no_known_subkey_then_error_is_returned)
{
  ntn_config_update_remote_command cmd(ntn_cfg_manager);

  auto future_time_ms =
      std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
          .count() +
      10000ULL;

  nlohmann::json req;
  req["cells"] = nlohmann::json::array();

  nlohmann::json cell;
  cell["plmn"]                                = "00101";
  cell["nci"]                                 = static_cast<uint64_t>(1);
  cell["epoch_timestamp"]                     = future_time_ms;
  cell["ntn_ul_sync_validity_duration"]       = 5;
  cell["ephemeris_info"]["eci"]["position_x"] = 3578630.0; // "eci" is not supported.
  req["cells"].push_back(cell);

  expected<nlohmann::json, std::string> result = cmd.execute(req);

  ASSERT_FALSE(result.has_value());
  logger.info("Received error: {}", result.error());
  EXPECT_TRUE(result.error().find("ephemeris_info") != std::string::npos ||
              result.error().find("Invalid") != std::string::npos)
      << "Expected error about invalid ephemeris_info, got: " << result.error();
}

TEST_F(ntn_config_update_remote_command_test, when_sat_switch_with_resync_has_no_ntn_cfg_then_error_is_returned)
{
  ntn_config_update_remote_command cmd(ntn_cfg_manager);

  auto future_time_ms =
      std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
          .count() +
      10000ULL;

  nlohmann::json req;
  req["cells"] = nlohmann::json::array();

  nlohmann::json cell;
  cell["plmn"]                                         = "00101";
  cell["nci"]                                          = static_cast<uint64_t>(1);
  cell["epoch_timestamp"]                              = future_time_ms;
  cell["ntn_ul_sync_validity_duration"]                = 5;
  cell["ephemeris_info"]["ecef"]["position_x"]         = 3578630.0;
  cell["ephemeris_info"]["ecef"]["position_y"]         = 1234567.0;
  cell["ephemeris_info"]["ecef"]["position_z"]         = 5432109.0;
  cell["ephemeris_info"]["ecef"]["velocity_vx"]        = 1500.0;
  cell["ephemeris_info"]["ecef"]["velocity_vy"]        = 3073.0;
  cell["ephemeris_info"]["ecef"]["velocity_vz"]        = -2500.0;
  cell["sat_switch_with_resync"]["ssb_time_offset_sf"] = 60U; // ntn_cfg is absent.
  req["cells"].push_back(cell);

  expected<nlohmann::json, std::string> result = cmd.execute(req);

  ASSERT_FALSE(result.has_value());
  logger.info("Received error: {}", result.error());
  EXPECT_TRUE(result.error().find("ntn_cfg") != std::string::npos)
      << "Expected error about missing ntn_cfg, got: " << result.error();
}

TEST_F(ntn_config_update_remote_command_test, when_ncells_entry_has_unsupported_field_then_error_is_returned)
{
  ntn_config_update_remote_command cmd(ntn_cfg_manager);

  auto future_time_ms =
      std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
          .count() +
      10000ULL;

  nlohmann::json req;
  req["cells"] = nlohmann::json::array();

  nlohmann::json cell;
  cell["plmn"]                                  = "00101";
  cell["nci"]                                   = static_cast<uint64_t>(1);
  cell["epoch_timestamp"]                       = future_time_ms;
  cell["ntn_ul_sync_validity_duration"]         = 5;
  cell["ephemeris_info"]["ecef"]["position_x"]  = 3578630.0;
  cell["ephemeris_info"]["ecef"]["position_y"]  = 1234567.0;
  cell["ephemeris_info"]["ecef"]["position_z"]  = 5432109.0;
  cell["ephemeris_info"]["ecef"]["velocity_vx"] = 1500.0;
  cell["ephemeris_info"]["ecef"]["velocity_vy"] = 3073.0;
  cell["ephemeris_info"]["ecef"]["velocity_vz"] = -2500.0;
  // "arfcn" is not a supported field in ncells entries.
  cell["ncells"] = nlohmann::json::array({{{"pci", 2U}, {"carrier_freq", 650000U}, {"arfcn", 700000U}}});
  req["cells"].push_back(cell);

  expected<nlohmann::json, std::string> result = cmd.execute(req);

  ASSERT_FALSE(result.has_value());
  logger.info("Received error: {}", result.error());
  EXPECT_TRUE(result.error().find("arfcn") != std::string::npos ||
              result.error().find("not supported") != std::string::npos)
      << "Expected error about unsupported ncells field, got: " << result.error();
}

TEST_F(ntn_config_update_remote_command_test, when_ssb_time_offset_sf_is_out_of_range_then_error_is_returned)
{
  ntn_config_update_remote_command cmd(ntn_cfg_manager);

  auto future_time_ms =
      std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
          .count() +
      10000ULL;

  nlohmann::json req;
  req["cells"] = nlohmann::json::array();

  nlohmann::json cell;
  cell["plmn"]                                                       = "00101";
  cell["nci"]                                                        = static_cast<uint64_t>(1);
  cell["epoch_timestamp"]                                            = future_time_ms;
  cell["ntn_ul_sync_validity_duration"]                              = 5;
  cell["ephemeris_info"]["ecef"]["position_x"]                       = 3578630.0;
  cell["ephemeris_info"]["ecef"]["position_y"]                       = 1234567.0;
  cell["ephemeris_info"]["ecef"]["position_z"]                       = 5432109.0;
  cell["ephemeris_info"]["ecef"]["velocity_vx"]                      = 1500.0;
  cell["ephemeris_info"]["ecef"]["velocity_vy"]                      = 3073.0;
  cell["ephemeris_info"]["ecef"]["velocity_vz"]                      = -2500.0;
  cell["sat_switch_with_resync"]["ssb_time_offset_sf"]               = 160U; // Max is 159.
  cell["sat_switch_with_resync"]["ntn_cfg"]["cell_specific_koffset"] = 16U;
  req["cells"].push_back(cell);

  expected<nlohmann::json, std::string> result = cmd.execute(req);

  ASSERT_FALSE(result.has_value());
  logger.info("Received error: {}", result.error());
  EXPECT_TRUE(result.error().find("ssb_time_offset_sf") != std::string::npos)
      << "Expected error about ssb_time_offset_sf range, got: " << result.error();
}

TEST_F(ntn_config_update_remote_command_test, when_ncells_pci_is_out_of_range_then_error_is_returned)
{
  ntn_config_update_remote_command cmd(ntn_cfg_manager);

  auto future_time_ms =
      std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
          .count() +
      10000ULL;

  nlohmann::json req;
  req["cells"] = nlohmann::json::array();

  nlohmann::json cell;
  cell["plmn"]                                  = "00101";
  cell["nci"]                                   = static_cast<uint64_t>(1);
  cell["epoch_timestamp"]                       = future_time_ms;
  cell["ntn_ul_sync_validity_duration"]         = 5;
  cell["ephemeris_info"]["ecef"]["position_x"]  = 3578630.0;
  cell["ephemeris_info"]["ecef"]["position_y"]  = 1234567.0;
  cell["ephemeris_info"]["ecef"]["position_z"]  = 5432109.0;
  cell["ephemeris_info"]["ecef"]["velocity_vx"] = 1500.0;
  cell["ephemeris_info"]["ecef"]["velocity_vy"] = 3073.0;
  cell["ephemeris_info"]["ecef"]["velocity_vz"] = -2500.0;
  cell["ncells"]                                = nlohmann::json::array({{{"pci", 1008U}}}); // MAX_PCI is 1007.
  req["cells"].push_back(cell);

  expected<nlohmann::json, std::string> result = cmd.execute(req);

  ASSERT_FALSE(result.has_value());
  logger.info("Received error: {}", result.error());
  EXPECT_TRUE(result.error().find("pci") != std::string::npos)
      << "Expected error about PCI out of range, got: " << result.error();
}

TEST_F(ntn_config_update_remote_command_test, when_ncells_carrier_freq_is_out_of_range_then_error_is_returned)
{
  ntn_config_update_remote_command cmd(ntn_cfg_manager);

  auto future_time_ms =
      std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
          .count() +
      10000ULL;

  nlohmann::json req;
  req["cells"] = nlohmann::json::array();

  nlohmann::json cell;
  cell["plmn"]                                  = "00101";
  cell["nci"]                                   = static_cast<uint64_t>(1);
  cell["epoch_timestamp"]                       = future_time_ms;
  cell["ntn_ul_sync_validity_duration"]         = 5;
  cell["ephemeris_info"]["ecef"]["position_x"]  = 3578630.0;
  cell["ephemeris_info"]["ecef"]["position_y"]  = 1234567.0;
  cell["ephemeris_info"]["ecef"]["position_z"]  = 5432109.0;
  cell["ephemeris_info"]["ecef"]["velocity_vx"] = 1500.0;
  cell["ephemeris_info"]["ecef"]["velocity_vy"] = 3073.0;
  cell["ephemeris_info"]["ecef"]["velocity_vz"] = -2500.0;
  cell["ncells"] = nlohmann::json::array({{{"carrier_freq", 3279166U}}}); // Max is 3279165.
  req["cells"].push_back(cell);

  expected<nlohmann::json, std::string> result = cmd.execute(req);

  ASSERT_FALSE(result.has_value());
  logger.info("Received error: {}", result.error());
  EXPECT_TRUE(result.error().find("carrier_freq") != std::string::npos)
      << "Expected error about carrier_freq out of range, got: " << result.error();
}

TEST_F(ntn_config_update_remote_command_test, when_ncells_exceeds_max_count_then_error_is_returned)
{
  ntn_config_update_remote_command cmd(ntn_cfg_manager);

  auto future_time_ms =
      std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
          .count() +
      10000ULL;

  nlohmann::json req;
  req["cells"] = nlohmann::json::array();

  nlohmann::json cell;
  cell["plmn"]                                  = "00101";
  cell["nci"]                                   = static_cast<uint64_t>(1);
  cell["epoch_timestamp"]                       = future_time_ms;
  cell["ntn_ul_sync_validity_duration"]         = 5;
  cell["ephemeris_info"]["ecef"]["position_x"]  = 3578630.0;
  cell["ephemeris_info"]["ecef"]["position_y"]  = 1234567.0;
  cell["ephemeris_info"]["ecef"]["position_z"]  = 5432109.0;
  cell["ephemeris_info"]["ecef"]["velocity_vx"] = 1500.0;
  cell["ephemeris_info"]["ecef"]["velocity_vy"] = 3073.0;
  cell["ephemeris_info"]["ecef"]["velocity_vz"] = -2500.0;

  // Add 9 ncells entries, exceeding MAX_NOF_NTN_NEIGHBORS (8).
  cell["ncells"] = nlohmann::json::array();
  for (unsigned i = 0; i < 9; ++i) {
    cell["ncells"].push_back({{"pci", i}});
  }
  req["cells"].push_back(cell);

  expected<nlohmann::json, std::string> result = cmd.execute(req);

  ASSERT_FALSE(result.has_value());
  logger.info("Received error: {}", result.error());
  EXPECT_TRUE(result.error().find("ncells") != std::string::npos)
      << "Expected error about ncells count limit, got: " << result.error();
}

TEST_F(ntn_config_update_remote_command_test, single_cell_update_with_linear_polarization)
{
  ntn_config_update_remote_command cmd(ntn_cfg_manager);

  auto future_time_ms =
      std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
          .count() +
      10000ULL;

  nlohmann::json req;
  req["cells"] = nlohmann::json::array();

  nlohmann::json cell;
  cell["plmn"]                                  = "00101";
  cell["nci"]                                   = static_cast<uint64_t>(1);
  cell["epoch_timestamp"]                       = future_time_ms;
  cell["ntn_ul_sync_validity_duration"]         = 5;
  cell["ephemeris_info"]["ecef"]["position_x"]  = 3578630.0;
  cell["ephemeris_info"]["ecef"]["position_y"]  = 1234567.0;
  cell["ephemeris_info"]["ecef"]["position_z"]  = 5432109.0;
  cell["ephemeris_info"]["ecef"]["velocity_vx"] = 1500.0;
  cell["ephemeris_info"]["ecef"]["velocity_vy"] = 3073.0;
  cell["ephemeris_info"]["ecef"]["velocity_vz"] = -2500.0;
  cell["polarization"]["dl"]                    = "linear";
  cell["polarization"]["ul"]                    = "linear";
  req["cells"].push_back(cell);

  nr_cell_global_id_t cgi;
  cgi.plmn_id = plmn_identity::parse("00101").value();
  cgi.nci     = nr_cell_identity::create(1).value();
  ntn_cfg_manager.result_to_return.succeeded.push_back(cgi);

  expected<nlohmann::json, std::string> result = cmd.execute(req);

  ASSERT_TRUE(result.has_value()) << "Command failed: " << result.error();
  ASSERT_EQ(ntn_cfg_manager.last_request.cells.size(), 1);
  ASSERT_TRUE(ntn_cfg_manager.last_request.cells[0].polarization.has_value());
  ASSERT_TRUE(ntn_cfg_manager.last_request.cells[0].polarization->dl.has_value());
  ASSERT_TRUE(ntn_cfg_manager.last_request.cells[0].polarization->ul.has_value());
  EXPECT_EQ(ntn_cfg_manager.last_request.cells[0].polarization->dl.value(),
            ntn_polarization_t::polarization_type::linear);
  EXPECT_EQ(ntn_cfg_manager.last_request.cells[0].polarization->ul.value(),
            ntn_polarization_t::polarization_type::linear);
}

TEST_F(ntn_config_update_remote_command_test, single_cell_update_with_ta_info_only_ta_common)
{
  ntn_config_update_remote_command cmd(ntn_cfg_manager);

  auto future_time_ms =
      std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
          .count() +
      10000ULL;

  nlohmann::json req;
  req["cells"] = nlohmann::json::array();

  nlohmann::json cell;
  cell["plmn"]                                  = "00101";
  cell["nci"]                                   = static_cast<uint64_t>(1);
  cell["epoch_timestamp"]                       = future_time_ms;
  cell["ntn_ul_sync_validity_duration"]         = 5;
  cell["ephemeris_info"]["ecef"]["position_x"]  = 3578630.0;
  cell["ephemeris_info"]["ecef"]["position_y"]  = 1234567.0;
  cell["ephemeris_info"]["ecef"]["position_z"]  = 5432109.0;
  cell["ephemeris_info"]["ecef"]["velocity_vx"] = 1500.0;
  cell["ephemeris_info"]["ecef"]["velocity_vy"] = 3073.0;
  cell["ephemeris_info"]["ecef"]["velocity_vz"] = -2500.0;
  cell["ta_info"]["ta_common"]                  = 5000.0; // drift fields omitted.
  req["cells"].push_back(cell);

  nr_cell_global_id_t cgi;
  cgi.plmn_id = plmn_identity::parse("00101").value();
  cgi.nci     = nr_cell_identity::create(1).value();
  ntn_cfg_manager.result_to_return.succeeded.push_back(cgi);

  expected<nlohmann::json, std::string> result = cmd.execute(req);

  ASSERT_TRUE(result.has_value()) << "Command failed: " << result.error();
  ASSERT_EQ(ntn_cfg_manager.last_request.cells.size(), 1);
  ASSERT_TRUE(ntn_cfg_manager.last_request.cells[0].ta_info.has_value());
  EXPECT_DOUBLE_EQ(ntn_cfg_manager.last_request.cells[0].ta_info->ta_common, 5000.0);
}
