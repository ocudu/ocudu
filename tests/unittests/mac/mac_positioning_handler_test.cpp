// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/mac/mac_sched/positioning_handler.h"
#include "tests/test_doubles/utils/test_rng.h"
#include "ocudu/adt/format.h"
#include "ocudu/mac/mac_positioning_measurement_handler.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/scheduler/scheduler_positioning_handler.h"
#include "ocudu/support/async/async_test_utils.h"
#include "ocudu/support/executors/manual_task_worker.h"
#include <gtest/gtest.h>

using namespace ocudu;

namespace {

class dummy_scheduler_positioning_handler : public scheduler_positioning_handler
{
public:
  std::optional<positioning_measurement_request>    last_req;
  std::vector<positioning_measurement_request>      received_reqs;
  std::vector<positioning_measurement_stop_request> stop_reqs;

  void handle_positioning_measurement_request(const positioning_measurement_request& req) override
  {
    last_req = req;
    received_reqs.push_back(req);
  }

  void handle_positioning_measurement_stop(const positioning_measurement_stop_request& req) override
  {
    stop_reqs.push_back(req);
  }
};

srs_config make_test_srs_config()
{
  srs_config cfg{};
  cfg.srs_res_list.resize(1);
  cfg.srs_res_list[0].id.ue_res_id   = srs_config::MIN_SRS_RES_ID;
  cfg.srs_res_list[0].id.cell_res_id = 0;
  cfg.srs_res_list[0].sequence_id    = 1;
  cfg.srs_res_set_list.resize(1);
  cfg.srs_res_set_list[0].res_type.emplace<srs_config::srs_resource_set::periodic_resource_type>();
  return cfg;
}

mac_srs_indication_message make_srs_ind(rnti_t               rnti,
                                        std::optional<float> azimuth_aoa_deg = std::nullopt,
                                        std::optional<float> zenith_aoa_deg  = std::nullopt)
{
  mac_srs_indication_message msg;
  msg.sl_rx = {0, 0};
  msg.srss.resize(1);
  msg.srss[0].rnti   = rnti;
  msg.srss[0].report = mac_srs_pdu::positioning_report{
      phy_time_unit::from_units_of_Tc(1000), std::nullopt, azimuth_aoa_deg, zenith_aoa_deg};
  return msg;
}

/// Creates an SRS indication carrying a positioning report with UL-AoA only, i.e. without UL-RTOA.
mac_srs_indication_message make_aoa_only_srs_ind(rnti_t rnti, float azimuth_aoa_deg)
{
  mac_srs_indication_message msg;
  msg.sl_rx = {0, 0};
  msg.srss.resize(1);
  msg.srss[0].rnti   = rnti;
  msg.srss[0].report = mac_srs_pdu::positioning_report{std::nullopt, std::nullopt, azimuth_aoa_deg, std::nullopt};
  return msg;
}

mac_positioning_measurement_request make_positioning_request_for_neighbor_ue()
{
  mac_positioning_measurement_request req;
  req.cells.resize(1);
  req.cells[0].cell_index  = to_du_cell_index(0);
  req.cells[0].srs_to_meas = make_test_srs_config();
  return req;
}

mac_positioning_measurement_request make_positioning_request_for_connected_ue(unsigned nof_cells = 1)
{
  mac_positioning_measurement_request req;
  req.cells.resize(nof_cells);
  for (unsigned i = 0; i != nof_cells; ++i) {
    req.cells[i].cell_index  = to_du_cell_index(i);
    req.cells[i].rnti        = to_rnti(0x4601 + i);
    req.cells[i].ue_index    = to_du_ue_index(i);
    req.cells[i].srs_to_meas = make_test_srs_config();
  }
  return req;
}

} // namespace

class positioning_handler_test
{
protected:
  positioning_handler_test() :
    logger(ocudulog::fetch_basic_logger("SCHED")),
    pos_handler(create_positioning_handler(sched, worker, timers, logger))
  {
  }

  dummy_scheduler_positioning_handler sched;
  manual_task_worker                  worker{128};
  timer_manager                       timers;
  ocudulog::basic_logger&             logger;

  std::unique_ptr<positioning_handler> pos_handler;

  std::vector<std::unique_ptr<cell_positioning_handler>> cells;
};

class single_cell_positioning_handler_test : public positioning_handler_test, public ::testing::Test
{
public:
  single_cell_positioning_handler_test() { cells.push_back(pos_handler->add_cell(to_du_cell_index(0))); }
};

TEST_F(single_cell_positioning_handler_test,
       when_connected_ue_initiates_positioning_measurement_then_scheduler_is_notified)
{
  mac_positioning_measurement_request                      req = make_positioning_request_for_connected_ue();
  auto                                                     t = pos_handler->handle_positioning_measurement_request(req);
  lazy_task_launcher<mac_positioning_measurement_response> t_launcher(t);

  // Scheduler gets notified.
  ASSERT_TRUE(sched.last_req.has_value());
  ASSERT_EQ(sched.last_req->cells.size(), 1);
  const auto& cell_req = sched.last_req->cells.front();
  ASSERT_EQ(cell_req.ue_index, req.cells[0].ue_index);
  ASSERT_EQ(cell_req.pos_rnti, req.cells[0].rnti);
  ASSERT_TRUE(is_crnti(cell_req.pos_rnti));
  ASSERT_EQ(cell_req.srs_to_measure, req.cells[0].srs_to_meas);
}

TEST_F(single_cell_positioning_handler_test,
       when_positioning_starts_for_neighbor_cell_ue_then_scheduler_is_notified_with_fake_rnti)
{
  mac_positioning_measurement_request req = make_positioning_request_for_neighbor_ue();

  auto                                                     t = pos_handler->handle_positioning_measurement_request(req);
  lazy_task_launcher<mac_positioning_measurement_response> t_launcher(t);

  // Scheduler gets notified.
  ASSERT_TRUE(sched.last_req.has_value());
  ASSERT_EQ(sched.last_req->cells.size(), 1);
  const auto& cell_req = sched.last_req->cells.front();
  ASSERT_FALSE(cell_req.ue_index.has_value());
  ASSERT_FALSE(is_crnti(cell_req.pos_rnti));
  ASSERT_EQ(cell_req.srs_to_measure, req.cells[0].srs_to_meas);
}

TEST_F(single_cell_positioning_handler_test,
       when_connected_ue_initiates_positioning_measurement_then_it_only_completes_when_measurement_completes)
{
  mac_positioning_measurement_request req  = make_positioning_request_for_connected_ue();
  rnti_t                              rnti = req.cells[0].rnti.value();

  auto                                                     t = pos_handler->handle_positioning_measurement_request(req);
  lazy_task_launcher<mac_positioning_measurement_response> t_launcher(t);

  // Procedure has not completed yet.
  worker.run_pending_tasks();
  ASSERT_FALSE(t_launcher.ready());
  ASSERT_TRUE(sched.stop_reqs.empty());

  // Forward SRS indication.
  mac_srs_indication_message srs_ind = make_srs_ind(rnti);
  cells[0]->handle_srs_indication(srs_ind);
  worker.run_pending_tasks();

  // Positioning has been completed.
  ASSERT_TRUE(t_launcher.ready());
  ASSERT_TRUE(t_launcher.result.has_value());
  ASSERT_EQ(t_launcher.result.value().sl_rx, srs_ind.sl_rx);
  ASSERT_EQ(t_launcher.result.value().cell_results.size(), 1);
  ASSERT_EQ(t_launcher.result.value().cell_results[0].ul_srs_pos_meass.size(), 1);
  ASSERT_EQ(t_launcher.result.value().cell_results[0].ul_srs_pos_meass[0].ul_rtoa,
            std::get<mac_srs_pdu::positioning_report>(srs_ind.srss[0].report).ul_rtoa.value());

  // Scheduler was notified to stop positioning measurements.
  ASSERT_FALSE(sched.stop_reqs.empty());
  ASSERT_EQ(sched.stop_reqs.back().completed_meas.size(), 1);
  ASSERT_EQ(sched.stop_reqs.back().completed_meas[0].second, rnti);
}

TEST_F(single_cell_positioning_handler_test,
       when_positioning_measurement_of_neighbor_cell_ue_is_initiated_then_it_only_completes_when_measurement_completes)
{
  mac_positioning_measurement_request req = make_positioning_request_for_neighbor_ue();

  auto                                                     t = pos_handler->handle_positioning_measurement_request(req);
  lazy_task_launcher<mac_positioning_measurement_response> t_launcher(t);
  worker.run_pending_tasks();
  ASSERT_TRUE(sched.last_req.has_value());
  ASSERT_EQ(sched.last_req->cells.size(), 1);
  const rnti_t pos_rnti = sched.last_req->cells.front().pos_rnti;

  // Procedure has not completed yet.
  ASSERT_FALSE(t_launcher.ready());
  ASSERT_TRUE(sched.stop_reqs.empty());

  mac_srs_indication_message srs_ind = make_srs_ind(pos_rnti);
  cells[0]->handle_srs_indication(srs_ind);
  worker.run_pending_tasks();

  // Positioning has been completed.
  ASSERT_TRUE(t_launcher.ready());
  ASSERT_TRUE(t_launcher.result.has_value());
  ASSERT_EQ(t_launcher.result.value().sl_rx, srs_ind.sl_rx);
  ASSERT_EQ(t_launcher.result.value().cell_results.size(), 1);
  ASSERT_EQ(t_launcher.result.value().cell_results[0].ul_srs_pos_meass.size(), 1);
  ASSERT_EQ(t_launcher.result.value().cell_results[0].ul_srs_pos_meass[0].ul_rtoa,
            std::get<mac_srs_pdu::positioning_report>(srs_ind.srss[0].report).ul_rtoa.value());

  // Scheduler was notified to stop positioning measurements.
  ASSERT_EQ(sched.stop_reqs.size(), 1);
  ASSERT_EQ(sched.stop_reqs.back().completed_meas.size(), 1);
  ASSERT_EQ(sched.stop_reqs.back().completed_meas[0].second, pos_rnti);
}

TEST_F(single_cell_positioning_handler_test,
       when_neighbor_ue_positioning_completes_then_pos_rnti_becomes_available_again)
{
  // First positioning run.
  mac_positioning_measurement_request                      req = make_positioning_request_for_neighbor_ue();
  auto                                                     t = pos_handler->handle_positioning_measurement_request(req);
  lazy_task_launcher<mac_positioning_measurement_response> t_launcher(t);
  const rnti_t                                             pos_rnti1 = sched.last_req->cells[0].pos_rnti;
  mac_srs_indication_message                               srs_ind   = make_srs_ind(pos_rnti1);
  cells[0]->handle_srs_indication(srs_ind);
  worker.run_pending_tasks();
  ASSERT_TRUE(t_launcher.ready());

  // Second positioning run.
  auto t2 = pos_handler->handle_positioning_measurement_request(req);
  lazy_task_launcher<mac_positioning_measurement_response> t_launcher2(t2);
  const rnti_t                                             pos_rnti2 = sched.last_req->cells[0].pos_rnti;
  worker.run_pending_tasks();
  ASSERT_FALSE(t_launcher2.ready()) << "Results were not correctly reset";
  srs_ind = make_srs_ind(pos_rnti2);
  cells[0]->handle_srs_indication(srs_ind);
  worker.run_pending_tasks();
  ASSERT_TRUE(t_launcher2.ready());

  ASSERT_EQ(pos_rnti1, pos_rnti2);
}

TEST_F(single_cell_positioning_handler_test, when_srs_indication_contains_ul_aoa_then_result_carries_ul_aoa)
{
  static constexpr float azimuth_deg = 123.4F;
  static constexpr float zenith_deg  = 56.7F;

  // Start a positioning measurement for a UE connected to the cell.
  mac_positioning_measurement_request                      req  = make_positioning_request_for_connected_ue();
  rnti_t                                                   rnti = req.cells[0].rnti.value();
  auto                                                     t = pos_handler->handle_positioning_measurement_request(req);
  lazy_task_launcher<mac_positioning_measurement_response> t_launcher(t);

  // Forward an SRS indication carrying both UL-RTOA and UL-AoA.
  mac_srs_indication_message srs_ind = make_srs_ind(rnti, azimuth_deg, zenith_deg);
  cells[0]->handle_srs_indication(srs_ind);
  worker.run_pending_tasks();

  // The AoA values are forwarded unchanged in the positioning measurement result.
  ASSERT_TRUE(t_launcher.ready());
  const auto& meas = t_launcher.result.value().cell_results[0].ul_srs_pos_meass[0];
  ASSERT_TRUE(meas.ul_rtoa.has_value());
  ASSERT_TRUE(meas.azimuth_aoa_deg.has_value());
  EXPECT_FLOAT_EQ(meas.azimuth_aoa_deg.value(), azimuth_deg);
  ASSERT_TRUE(meas.zenith_aoa_deg.has_value());
  EXPECT_FLOAT_EQ(meas.zenith_aoa_deg.value(), zenith_deg);
}

TEST_F(single_cell_positioning_handler_test, when_srs_indication_contains_only_ul_aoa_then_it_is_not_dropped)
{
  static constexpr float azimuth_deg = 200.1F;

  // Start a positioning measurement for a UE connected to the cell.
  mac_positioning_measurement_request                      req  = make_positioning_request_for_connected_ue();
  rnti_t                                                   rnti = req.cells[0].rnti.value();
  auto                                                     t = pos_handler->handle_positioning_measurement_request(req);
  lazy_task_launcher<mac_positioning_measurement_response> t_launcher(t);

  // Forward an SRS indication carrying UL-AoA only, i.e. without UL-RTOA.
  mac_srs_indication_message srs_ind = make_aoa_only_srs_ind(rnti, azimuth_deg);
  cells[0]->handle_srs_indication(srs_ind);
  worker.run_pending_tasks();

  // The report is not dropped despite missing UL-RTOA.
  ASSERT_TRUE(t_launcher.ready());
  const auto& meas = t_launcher.result.value().cell_results[0].ul_srs_pos_meass[0];
  ASSERT_FALSE(meas.ul_rtoa.has_value());
  ASSERT_TRUE(meas.azimuth_aoa_deg.has_value());
  EXPECT_FLOAT_EQ(meas.azimuth_aoa_deg.value(), azimuth_deg);
}

class multi_cell_positioning_handler_test : public positioning_handler_test, public ::testing::Test
{
public:
  multi_cell_positioning_handler_test()
  {
    cells.push_back(pos_handler->add_cell(to_du_cell_index(0)));
    cells.push_back(pos_handler->add_cell(to_du_cell_index(1)));
  }
};

TEST_F(multi_cell_positioning_handler_test,
       when_positioning_measurement_is_initiated_for_multiple_cells_then_it_completes_when_all_cells_report)
{
  mac_positioning_measurement_request                      req       = make_positioning_request_for_connected_ue(2);
  rnti_t                                                   pos_rnti1 = req.cells[0].rnti.value();
  rnti_t                                                   pos_rnti2 = req.cells[1].rnti.value();
  unsigned                                                 rand_slot_offset = test_rng::uniform_int(0, 100);
  auto                                                     t = pos_handler->handle_positioning_measurement_request(req);
  lazy_task_launcher<mac_positioning_measurement_response> t_launcher(t);

  // Scheduler gets notified for all cells.
  ASSERT_TRUE(sched.last_req.has_value());
  ASSERT_EQ(sched.last_req->cells.size(), req.cells.size());
  ASSERT_EQ(sched.received_reqs.size(), 1);
  for (unsigned i = 0; i != req.cells.size(); ++i) {
    const auto& expected_cell = req.cells[i];
    const auto& notif_cell    = sched.last_req->cells[i];
    EXPECT_EQ(notif_cell.cell_index, expected_cell.cell_index);
    EXPECT_EQ(notif_cell.ue_index, expected_cell.ue_index);
    EXPECT_EQ(notif_cell.pos_rnti, expected_cell.rnti);
    EXPECT_TRUE(is_crnti(notif_cell.pos_rnti));
    EXPECT_EQ(notif_cell.srs_to_measure, expected_cell.srs_to_meas);
  }

  // Procedure has not completed yet.
  worker.run_pending_tasks();
  ASSERT_FALSE(t_launcher.ready());
  ASSERT_TRUE(sched.stop_reqs.empty());

  // Cell 0 reports.
  mac_srs_indication_message srs_ind = make_srs_ind(pos_rnti1);
  srs_ind.sl_rx += rand_slot_offset;
  cells[0]->handle_srs_indication(srs_ind);
  worker.run_pending_tasks();
  ASSERT_FALSE(t_launcher.ready());

  // Cell 1 reports.
  srs_ind = make_srs_ind(pos_rnti2);
  srs_ind.sl_rx += rand_slot_offset;
  cells[1]->handle_srs_indication(srs_ind);
  worker.run_pending_tasks();

  // All cells reported.
  ASSERT_TRUE(t_launcher.ready());
  // Scheduler was notified to stop positioning measurements.
  ASSERT_EQ(sched.stop_reqs.size(), 1);
  ASSERT_EQ(sched.stop_reqs.back().completed_meas.size(), 2);
  EXPECT_EQ(sched.stop_reqs.back().completed_meas[0].second, pos_rnti1);
  EXPECT_EQ(sched.stop_reqs.back().completed_meas[1].second, pos_rnti2);
}

TEST_F(
    multi_cell_positioning_handler_test,
    when_positioning_measurement_is_initiated_for_multiple_cells_then_it_does_not_complete_until_cells_reports_match_in_slots)
{
  mac_positioning_measurement_request                      req = make_positioning_request_for_connected_ue(2);
  unsigned                                                 rand_slot_offset = test_rng::uniform_int(1, 100);
  rnti_t                                                   pos_rnti1        = req.cells[0].rnti.value();
  rnti_t                                                   pos_rnti2        = req.cells[1].rnti.value();
  auto                                                     t = pos_handler->handle_positioning_measurement_request(req);
  lazy_task_launcher<mac_positioning_measurement_response> t_launcher(t);

  // Scheduler gets notified for all cells.
  ASSERT_TRUE(sched.last_req.has_value());
  ASSERT_EQ(sched.last_req->cells.size(), req.cells.size());
  ASSERT_EQ(sched.received_reqs.size(), 1);
  for (unsigned i = 0; i != req.cells.size(); ++i) {
    const auto& expected_cell = req.cells[i];
    const auto& notif_cell    = sched.last_req->cells[i];
    EXPECT_EQ(notif_cell.cell_index, expected_cell.cell_index);
    EXPECT_EQ(notif_cell.ue_index, expected_cell.ue_index);
    EXPECT_EQ(notif_cell.pos_rnti, expected_cell.rnti);
    EXPECT_TRUE(is_crnti(notif_cell.pos_rnti));
    EXPECT_EQ(notif_cell.srs_to_measure, expected_cell.srs_to_meas);
  }

  // Procedure has not completed yet.
  worker.run_pending_tasks();
  ASSERT_FALSE(t_launcher.ready());

  // Cell 0 reports.
  mac_srs_indication_message srs_ind = make_srs_ind(pos_rnti1);
  cells[0]->handle_srs_indication(srs_ind);
  worker.run_pending_tasks();
  ASSERT_FALSE(t_launcher.ready());

  // Cell 1 reports.
  srs_ind = make_srs_ind(pos_rnti2);
  srs_ind.sl_rx += rand_slot_offset; // different slot.
  cells[1]->handle_srs_indication(srs_ind);
  worker.run_pending_tasks();

  // All cells reported, but they don't match yet in slot at which report was produced.
  ASSERT_FALSE(t_launcher.ready());
  ASSERT_TRUE(sched.stop_reqs.empty());

  // Cell 0 reports again, now matching cell 1 slot.
  srs_ind = make_srs_ind(pos_rnti1);
  srs_ind.sl_rx += rand_slot_offset;
  cells[0]->handle_srs_indication(srs_ind);
  worker.run_pending_tasks();

  // All cells reported, now matching in slot.
  ASSERT_TRUE(t_launcher.ready());
  // Scheduler was notified to stop positioning measurements.
  ASSERT_EQ(sched.stop_reqs.size(), 1);
  ASSERT_EQ(sched.stop_reqs.back().completed_meas.size(), 2);
  EXPECT_EQ(sched.stop_reqs.back().completed_meas[0].second, pos_rnti1);
  EXPECT_EQ(sched.stop_reqs.back().completed_meas[1].second, pos_rnti2);
}

TEST_F(multi_cell_positioning_handler_test, when_positioning_measurement_completes_then_resources_are_cleaned_up)
{
  // Launch positioning for 2 cells.
  mac_positioning_measurement_request                      req       = make_positioning_request_for_connected_ue(2);
  rnti_t                                                   pos_rnti1 = req.cells[0].rnti.value();
  rnti_t                                                   pos_rnti2 = req.cells[1].rnti.value();
  auto                                                     t = pos_handler->handle_positioning_measurement_request(req);
  lazy_task_launcher<mac_positioning_measurement_response> t_launcher(t);

  // Cell 0 and 1 report and positioning procedure completes.
  mac_srs_indication_message srs_ind = make_srs_ind(pos_rnti1);
  cells[0]->handle_srs_indication(srs_ind);
  srs_ind = make_srs_ind(pos_rnti2);
  cells[1]->handle_srs_indication(srs_ind);
  worker.run_pending_tasks();
  ASSERT_TRUE(t_launcher.ready());

  // A new positioning measurement can be initiated right away and so happens to match with previous indication slot.
  // This wrap-around is highly unlikely in practice, but we want to ensure that resources are correctly cleaned up.
  auto t2 = pos_handler->handle_positioning_measurement_request(req);
  lazy_task_launcher<mac_positioning_measurement_response> t_launcher2(t2);
  srs_ind = make_srs_ind(pos_rnti1);
  cells[0]->handle_srs_indication(srs_ind);
  worker.run_pending_tasks();
  ASSERT_FALSE(t_launcher2.ready());
  srs_ind = make_srs_ind(pos_rnti2);
  cells[1]->handle_srs_indication(srs_ind);
  worker.run_pending_tasks();
  ASSERT_TRUE(t_launcher2.ready());
}
