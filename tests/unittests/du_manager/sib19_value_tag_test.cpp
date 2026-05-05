// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/du/du_high/du_high_ntn_sib19_update_handler_impl.h"
#include "ocudu/du/du_high/du_manager/du_configurator.h"
#include "ocudu/ntn/ntn_sib19_update_handler.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/ran/nr_cgi.h"
#include "ocudu/ran/ntn.h"
#include "ocudu/ran/sib/system_info_config.h"
#include "ocudu/support/async/async_no_op_task.h"
#include <gtest/gtest.h>
#include <optional>

using namespace ocudu;
using namespace ocudu_ntn;
using namespace odu;

// ---------------------------------------------------------------------------
// Mock du_configurator that captures the last NTN update request.
// ---------------------------------------------------------------------------

class mock_du_configurator : public du_configurator
{
public:
  std::optional<du_ntn_param_update_request> last_ntn_req;

  async_task<du_mac_sched_control_config_response>
  configure_ue_mac_scheduler(const du_mac_sched_control_config&) override
  {
    return launch_no_op_task(du_mac_sched_control_config_response{});
  }

  du_param_config_response handle_sync_operator_config(const du_param_config_request&) override { return {true}; }

  async_task<du_param_config_response> handle_operator_config(const du_param_config_request&, task_executor&) override
  {
    return launch_no_op_task(du_param_config_response{true});
  }

  void handle_ntn_param_update(const du_ntn_param_update_request& req) override { last_ntn_req = req; }
};

// ---------------------------------------------------------------------------
// Helper: build a minimal valid ntn_sib19_update_request with ntn_cfg.
// The handler dereferences ntn_cfg->ephemeris_info.value(), so both must be set.
// ---------------------------------------------------------------------------

static ntn_sib19_update_request make_base_request(bool si_valuetag_change)
{
  ntn_sib19_update_request req;
  req.nr_cgi             = nr_cell_global_id_t{plmn_identity::test_value(), nr_cell_identity::create(1).value()};
  req.si_msg_idx         = 0;
  req.sib_idx            = 19;
  req.slot               = slot_point{subcarrier_spacing::kHz15, 0, 0};
  req.si_slot_period     = 320;
  req.si_valuetag_change = si_valuetag_change;

  sib19_info& sib19 = req.sib19;
  sib19.ntn_cfg.emplace();
  sib19.ntn_cfg->cell_specific_koffset.emplace(std::chrono::milliseconds(260));
  sib19.ntn_cfg->k_mac     = 128u;
  sib19.ntn_cfg->ta_report = true;
  sib19.ntn_cfg->epoch_time.emplace();
  sib19.ntn_cfg->epoch_time->sfn             = 0;
  sib19.ntn_cfg->epoch_time->subframe_number = 0;

  ecef_coordinates_t ecef{};
  ecef.position_x  = 1300.0;
  ecef.position_y  = 2600.0;
  ecef.position_z  = 3900.0;
  ecef.velocity_vx = 0.26;
  ecef.velocity_vy = 0.52;
  ecef.velocity_vz = 0.78;
  sib19.ntn_cfg->ephemeris_info.emplace(ecef);

  return req;
}

// ---------------------------------------------------------------------------
// Test fixture
// ---------------------------------------------------------------------------

class sib19_value_tag_handler_test : public ::testing::Test
{
protected:
  mock_du_configurator                  mock_cfgr;
  du_high_ntn_sib19_update_handler_impl handler{mock_cfgr};

  /// Returns the sib19 field of the first cell in the last captured NTN request, if set.
  std::optional<sib19_info> captured_sib19() const
  {
    if (!mock_cfgr.last_ntn_req.has_value() || mock_cfgr.last_ntn_req->cells.empty()) {
      return std::nullopt;
    }
    return mock_cfgr.last_ntn_req->cells[0].sib19;
  }
};

// ---------------------------------------------------------------------------
// Tests: si_valuetag_change propagation
// ---------------------------------------------------------------------------

/// When si_valuetag_change is true (first update or a tracked field changed),
/// the handler must set cell_req.sib19 to trigger systemInfoValueTag increment.
TEST_F(sib19_value_tag_handler_test, first_update_triggers_value_tag)
{
  auto req = make_base_request(/*si_valuetag_change=*/true);
  handler.handle_sib19_msg_update(req);

  ASSERT_TRUE(captured_sib19().has_value()) << "sib19 must be set when si_valuetag_change=true";
  EXPECT_EQ(captured_sib19()->ntn_cfg->cell_specific_koffset, req.sib19.ntn_cfg->cell_specific_koffset);
}

/// When si_valuetag_change is false (tracked fields unchanged), the handler
/// must NOT set cell_req.sib19 — no unnecessary systemInfoValueTag bump.
TEST_F(sib19_value_tag_handler_test, repeat_same_tracked_fields_no_trigger)
{
  auto req = make_base_request(/*si_valuetag_change=*/false);
  handler.handle_sib19_msg_update(req);

  EXPECT_FALSE(captured_sib19().has_value()) << "sib19 must not be set when si_valuetag_change=false";
}

/// Exempt-only changes (e.g. epoch_time, ephemeris) produce si_valuetag_change=false.
/// The handler must not set cell_req.sib19.
TEST_F(sib19_value_tag_handler_test, exempt_field_change_only_no_trigger)
{
  auto req = make_base_request(/*si_valuetag_change=*/false);
  // Modify only exempt fields to simulate a purely exempt-field update.
  req.sib19.ntn_cfg->epoch_time->sfn             = 99;
  req.sib19.ntn_cfg->epoch_time->subframe_number = 5;
  ecef_coordinates_t new_ecef{};
  new_ecef.position_x = 9999.0;
  req.sib19.ntn_cfg->ephemeris_info.emplace(new_ecef);

  handler.handle_sib19_msg_update(req);

  EXPECT_FALSE(captured_sib19().has_value());
}

/// A tracked field change (e.g. koffset) produces si_valuetag_change=true.
/// The handler must set cell_req.sib19.
TEST_F(sib19_value_tag_handler_test, tracked_koffset_change_triggers_value_tag)
{
  auto req                                 = make_base_request(/*si_valuetag_change=*/true);
  req.sib19.ntn_cfg->cell_specific_koffset = std::chrono::milliseconds(300);

  handler.handle_sib19_msg_update(req);

  ASSERT_TRUE(captured_sib19().has_value());
  EXPECT_EQ(captured_sib19()->ntn_cfg->cell_specific_koffset,
            std::optional<std::chrono::milliseconds>(std::chrono::milliseconds(300)));
}

TEST_F(sib19_value_tag_handler_test, tracked_kmac_change_triggers_value_tag)
{
  auto req                 = make_base_request(/*si_valuetag_change=*/true);
  req.sib19.ntn_cfg->k_mac = 256u;

  handler.handle_sib19_msg_update(req);

  ASSERT_TRUE(captured_sib19().has_value());
}

TEST_F(sib19_value_tag_handler_test, tracked_polarization_change_triggers_value_tag)
{
  auto req = make_base_request(/*si_valuetag_change=*/true);
  req.sib19.ntn_cfg->polarization.emplace();
  req.sib19.ntn_cfg->polarization->dl = ntn_polarization_t::polarization_type::rhcp;
  req.sib19.ntn_cfg->polarization->ul = ntn_polarization_t::polarization_type::lhcp;

  handler.handle_sib19_msg_update(req);

  ASSERT_TRUE(captured_sib19().has_value());
}

TEST_F(sib19_value_tag_handler_test, tracked_ta_report_change_triggers_value_tag)
{
  auto req                     = make_base_request(/*si_valuetag_change=*/true);
  req.sib19.ntn_cfg->ta_report = false;

  handler.handle_sib19_msg_update(req);

  ASSERT_TRUE(captured_sib19().has_value());
}

TEST_F(sib19_value_tag_handler_test, tracked_t_service_change_triggers_value_tag)
{
  auto req            = make_base_request(/*si_valuetag_change=*/true);
  req.sib19.t_service = std::chrono::system_clock::time_point(std::chrono::milliseconds(1000000));

  handler.handle_sib19_msg_update(req);

  ASSERT_TRUE(captured_sib19().has_value());
}

TEST_F(sib19_value_tag_handler_test, tracked_ref_location_change_triggers_value_tag)
{
  auto req = make_base_request(/*si_valuetag_change=*/true);
  req.sib19.ref_location.emplace();
  req.sib19.ref_location->latitude  = 48.1;
  req.sib19.ref_location->longitude = 11.6;
  req.sib19.ref_location->altitude  = 550.0;

  handler.handle_sib19_msg_update(req);

  ASSERT_TRUE(captured_sib19().has_value());
}

TEST_F(sib19_value_tag_handler_test, tracked_distance_thres_change_triggers_value_tag)
{
  auto req                 = make_base_request(/*si_valuetag_change=*/true);
  req.sib19.distance_thres = 10000u;

  handler.handle_sib19_msg_update(req);

  ASSERT_TRUE(captured_sib19().has_value());
}

TEST_F(sib19_value_tag_handler_test, tracked_ncells_change_triggers_value_tag)
{
  auto              req = make_base_request(/*si_valuetag_change=*/true);
  neighbor_ntn_cell ncell;
  ncell.phys_cell_id.emplace(100);
  ncell.carrier_freq.emplace(arfcn_t{650000});
  req.sib19.ncells.push_back(ncell);

  handler.handle_sib19_msg_update(req);

  ASSERT_TRUE(captured_sib19().has_value());
}

TEST_F(sib19_value_tag_handler_test, tracked_coverage_enhancements_change_triggers_value_tag)
{
  auto req = make_base_request(/*si_valuetag_change=*/true);
  req.sib19.coverage_enhancements.emplace();
  req.sib19.coverage_enhancements->nof_msg4_harq_ack_rep    = 4u;
  req.sib19.coverage_enhancements->rsrp_thres_msg4_harq_ack = 60u;

  handler.handle_sib19_msg_update(req);

  ASSERT_TRUE(captured_sib19().has_value());
}

TEST_F(sib19_value_tag_handler_test, tracked_sat_switch_change_triggers_value_tag)
{
  auto req = make_base_request(/*si_valuetag_change=*/true);
  req.sib19.sat_switch_with_resync.emplace();
  req.sib19.sat_switch_with_resync->ntn_cfg.cell_specific_koffset.emplace(std::chrono::milliseconds(100));
  req.sib19.sat_switch_with_resync->t_service_start =
      std::chrono::system_clock::time_point(std::chrono::milliseconds(123456789));
  req.sib19.sat_switch_with_resync->ssb_time_offset_sf.emplace(50);

  handler.handle_sib19_msg_update(req);

  ASSERT_TRUE(captured_sib19().has_value());
}

// ---------------------------------------------------------------------------
// Two cells tracked independently: a second cell's request is independent.
// ---------------------------------------------------------------------------

TEST_F(sib19_value_tag_handler_test, two_cells_tracked_independently)
{
  // Cell 1: si_valuetag_change=true.
  auto req1   = make_base_request(/*si_valuetag_change=*/true);
  req1.nr_cgi = nr_cell_global_id_t{plmn_identity::test_value(), nr_cell_identity::create(1).value()};

  handler.handle_sib19_msg_update(req1);
  ASSERT_TRUE(captured_sib19().has_value()) << "Cell 1 with si_valuetag_change=true must set sib19";

  // Cell 2: si_valuetag_change=false.
  auto req2   = make_base_request(/*si_valuetag_change=*/false);
  req2.nr_cgi = nr_cell_global_id_t{plmn_identity::test_value(), nr_cell_identity::create(2).value()};

  handler.handle_sib19_msg_update(req2);
  EXPECT_FALSE(captured_sib19().has_value()) << "Cell 2 with si_valuetag_change=false must not set sib19";
}

// ---------------------------------------------------------------------------

int main(int argc, char** argv)
{
  ocudulog::init();
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
