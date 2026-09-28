// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "du_manager_procedure_test_helpers.h"
#include "lib/du/du_high/du_manager/procedures/du_ue_e_cid_measurement_procedure.h"
#include "ocudu/du/du_cell_config_helpers.h"
#include "ocudu/support/async/async_test_utils.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace odu;

namespace {

/// Angle of arrival the MAC reports, in degrees.
constexpr float test_azimuth_deg = 123.4F;
constexpr float test_zenith_deg  = 56.7F;

/// The reported values of the angles above, as per TS 38.133, Table 13.4.1-1 and Table 13.4.1-2.
constexpr uint16_t test_azimuth_aoa = 3034;
constexpr uint16_t test_zenith_aoa  = 567;

/// Latitude the test cell reports, in the TS 38.455 Section 9.1.19.7 encoding.
constexpr uint32_t test_latitude = 12345;

std::vector<du_cell_config> make_test_cells()
{
  std::vector<du_cell_config> cells = {config_helpers::make_default_du_cell_config()};

  ng_ran_access_point_position_t position{};
  position.latitude            = test_latitude;
  cells[0].trp_geo_coordinates = position;

  return cells;
}

class du_ue_e_cid_measurement_procedure_test : public du_manager_proc_tester, public ::testing::Test
{
protected:
  du_ue_e_cid_measurement_procedure_test() : du_manager_proc_tester(make_test_cells())
  {
    ocudulog::fetch_basic_logger("DU-MNG").set_level(ocudulog::basic_levels::debug);
    ocudulog::init();
  }

  ~du_ue_e_cid_measurement_procedure_test() override { ocudulog::flush(); }

  /// Programs the MAC to report an angle of arrival on the next measurement request.
  void mac_reports_aoa(std::optional<float> azimuth_deg, std::optional<float> zenith_deg)
  {
    auto& mac_resp = mac.next_positioning_meas_response;
    mac_resp.cell_results.resize(1);
    mac_resp.cell_results[0].ul_srs_pos_meass.resize(1);
    mac_resp.cell_results[0].ul_srs_pos_meass[0].azimuth_aoa_deg = azimuth_deg;
    mac_resp.cell_results[0].ul_srs_pos_meass[0].zenith_aoa_deg  = zenith_deg;
  }

  du_e_cid_meas_response run_procedure(const du_e_cid_meas_request& req)
  {
    async_task<du_e_cid_meas_response> t = launch_async<du_ue_e_cid_measurement_procedure>(
        req, cell_mng, static_cast<du_ue_manager_repository&>(ue_mng), params);
    lazy_task_launcher<du_e_cid_meas_response> launcher{t};
    ocudu_assert(launcher.ready(), "The E-CID measurement procedure should have completed by now");
    return t.get();
  }

  du_e_cid_meas_request make_request(du_ue_index_t ue_index, std::vector<e_cid_meas_quantity> quantities)
  {
    return du_e_cid_meas_request{ue_index, std::move(quantities)};
  }

  du_ue_index_t test_ue_index = to_du_ue_index(0);
};

} // namespace

TEST_F(du_ue_e_cid_measurement_procedure_test, when_ue_does_not_exist_then_measurement_fails)
{
  du_e_cid_meas_response resp = run_procedure(make_request(test_ue_index, {e_cid_meas_quantity::nr_angle_of_arrival}));

  ASSERT_FALSE(resp.success);
  ASSERT_FALSE(mac.last_positioning_meas_request.has_value());
}

TEST_F(du_ue_e_cid_measurement_procedure_test, when_ue_has_no_srs_config_then_measurement_fails)
{
  // Remove the SRS configuration from the UE resources before the UE is created.
  cell_res_alloc.next_context_update_result.cell_group.cells[SERVING_PCELL_IDX]
      .serv_cell_cfg.ul_config->init_ul_bwp.srs_cfg.reset();
  create_ue(test_ue_index);

  du_e_cid_meas_response resp = run_procedure(make_request(test_ue_index, {e_cid_meas_quantity::nr_angle_of_arrival}));

  ASSERT_FALSE(resp.success);
  ASSERT_FALSE(mac.last_positioning_meas_request.has_value());
}

TEST_F(du_ue_e_cid_measurement_procedure_test, when_only_the_default_quantity_is_requested_then_no_mac_request_is_sent)
{
  create_ue(test_ue_index);

  du_e_cid_meas_response resp = run_procedure(make_request(test_ue_index, {e_cid_meas_quantity::default_quantity}));

  ASSERT_TRUE(resp.success);
  ASSERT_TRUE(resp.ul_aoa_results.empty());
  ASSERT_FALSE(mac.last_positioning_meas_request.has_value());
}

TEST_F(du_ue_e_cid_measurement_procedure_test,
       when_aoa_is_requested_then_mac_request_carries_the_ue_rnti_and_srs_config)
{
  du_ue& ue = create_ue(test_ue_index);
  mac_reports_aoa(test_azimuth_deg, test_zenith_deg);

  du_e_cid_meas_request  req  = make_request(test_ue_index, {e_cid_meas_quantity::nr_angle_of_arrival});
  du_e_cid_meas_response resp = run_procedure(req);

  ASSERT_TRUE(resp.success);
  ASSERT_TRUE(mac.last_positioning_meas_request.has_value());
  const auto& mac_req = mac.last_positioning_meas_request.value();
  ASSERT_EQ(mac_req.cells.size(), 1);
  ASSERT_EQ(mac_req.cells[0].ue_index, test_ue_index);
  ASSERT_EQ(mac_req.cells[0].rnti, ue.rnti);
  ASSERT_EQ(mac_req.cells[0].srs_to_meas,
            ue.resources->cell_group.cells.at(SERVING_PCELL_IDX).serv_cell_cfg.ul_config->init_ul_bwp.srs_cfg.value());
}

TEST_F(du_ue_e_cid_measurement_procedure_test, when_srs_period_is_long_then_mac_timeout_follows_the_srs_period)
{
  // A long SRS period delays the first SRS occasion. Set the longest period that the UE can use.
  auto& srs_cfg = cell_res_alloc.next_context_update_result.cell_group.cells[SERVING_PCELL_IDX]
                      .serv_cell_cfg.ul_config->init_ul_bwp.srs_cfg.value();
  srs_cfg.srs_res_list[0].periodicity_and_offset.emplace();
  srs_cfg.srs_res_list[0].periodicity_and_offset->period = srs_periodicity::sl2560;
  create_ue(test_ue_index);
  mac_reports_aoa(test_azimuth_deg, test_zenith_deg);

  run_procedure(make_request(test_ue_index, {e_cid_meas_quantity::nr_angle_of_arrival}));

  ASSERT_TRUE(mac.last_positioning_meas_request.has_value());
  // The MAC waits for at least one SRS period, so the measurement does not fail before the UE transmits.
  const unsigned period_ms =
      2560 / get_nof_slots_per_subframe(
                 cell_mng.get_cell_cfg(to_du_cell_index(0)).ran.ul_cfg_common.init_ul_bwp.generic_params.scs);
  ASSERT_GE(mac.last_positioning_meas_request->timeout, std::chrono::milliseconds{period_ms});
}

TEST_F(du_ue_e_cid_measurement_procedure_test, when_mac_reports_aoa_then_degrees_are_encoded_as_reported_values)
{
  create_ue(test_ue_index);
  mac_reports_aoa(test_azimuth_deg, test_zenith_deg);

  du_e_cid_meas_response resp = run_procedure(make_request(test_ue_index, {e_cid_meas_quantity::nr_angle_of_arrival}));

  ASSERT_TRUE(resp.success);
  ASSERT_EQ(resp.ul_aoa_results.size(), 1);
  // The degrees are encoded as per TS 38.133, Table 13.4.1-1 and Table 13.4.1-2.
  ASSERT_EQ(resp.ul_aoa_results[0].azimuth_aoa, test_azimuth_aoa);
  ASSERT_TRUE(resp.ul_aoa_results[0].zenith_aoa.has_value());
  ASSERT_EQ(resp.ul_aoa_results[0].zenith_aoa.value(), test_zenith_aoa);
}

TEST_F(du_ue_e_cid_measurement_procedure_test, when_mac_reports_no_zenith_angle_then_only_the_azimuth_is_reported)
{
  create_ue(test_ue_index);
  mac_reports_aoa(test_azimuth_deg, std::nullopt);

  du_e_cid_meas_response resp = run_procedure(make_request(test_ue_index, {e_cid_meas_quantity::nr_angle_of_arrival}));

  ASSERT_TRUE(resp.success);
  ASSERT_EQ(resp.ul_aoa_results.size(), 1);
  ASSERT_FALSE(resp.ul_aoa_results[0].zenith_aoa.has_value());
}

TEST_F(du_ue_e_cid_measurement_procedure_test, when_mac_reports_nothing_then_measurement_fails)
{
  create_ue(test_ue_index);
  // The MAC returns an empty response, which is what it does when the measurement times out.

  du_e_cid_meas_response resp = run_procedure(make_request(test_ue_index, {e_cid_meas_quantity::nr_angle_of_arrival}));

  ASSERT_FALSE(resp.success);
  ASSERT_TRUE(resp.ul_aoa_results.empty());
}

TEST_F(du_ue_e_cid_measurement_procedure_test, when_mac_reports_no_angle_of_arrival_then_measurement_fails)
{
  create_ue(test_ue_index);
  mac_reports_aoa(std::nullopt, std::nullopt);

  du_e_cid_meas_response resp = run_procedure(make_request(test_ue_index, {e_cid_meas_quantity::nr_angle_of_arrival}));

  ASSERT_FALSE(resp.success);
  ASSERT_TRUE(resp.ul_aoa_results.empty());
}

TEST_F(du_ue_e_cid_measurement_procedure_test, when_the_cell_has_geo_coordinates_then_they_are_reported)
{
  create_ue(test_ue_index);
  mac_reports_aoa(test_azimuth_deg, test_zenith_deg);

  du_e_cid_meas_response resp = run_procedure(make_request(test_ue_index, {e_cid_meas_quantity::nr_angle_of_arrival}));

  ASSERT_TRUE(resp.success);
  ASSERT_TRUE(resp.geo_coords.has_value());
  const auto& direct = std::get<trp_position_direct_t>(resp.geo_coords->trp_position_definition_type);
  ASSERT_EQ(std::get<ng_ran_access_point_position_t>(direct.accuracy).latitude, test_latitude);
}

TEST_F(du_ue_e_cid_measurement_procedure_test,
       when_only_the_default_quantity_is_requested_then_geo_coordinates_are_reported)
{
  create_ue(test_ue_index);

  du_e_cid_meas_response resp = run_procedure(make_request(test_ue_index, {e_cid_meas_quantity::default_quantity}));

  ASSERT_TRUE(resp.success);
  ASSERT_TRUE(resp.geo_coords.has_value());
}

TEST_F(du_ue_e_cid_measurement_procedure_test, when_ue_has_no_serving_cell_then_measurement_fails)
{
  create_ue(test_ue_index);
  // Drop the serving cell configuration of the UE.
  cell_res_alloc.ue_resource_pool[test_ue_index].cell_group.cells.clear();

  du_e_cid_meas_response resp = run_procedure(make_request(test_ue_index, {e_cid_meas_quantity::nr_angle_of_arrival}));

  ASSERT_FALSE(resp.success);
  ASSERT_FALSE(mac.last_positioning_meas_request.has_value());
}
