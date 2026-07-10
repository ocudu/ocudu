// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/ntn/ntn_configuration_manager.h"
#include "ocudu/ntn/ntn_configuration_manager_config.h"
#include "ocudu/ntn/ntn_configuration_manager_dependencies.h"
#include "ocudu/ntn/ntn_configuration_manager_factory.h"
#include "ocudu/ntn/ntn_meas_info_update_handler.h"
#include "ocudu/ntn/ntn_time_provider.h"
#include "ocudu/support/executors/manual_task_worker.h"
#include "ocudu/support/timers.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocudu_ntn;

namespace {

/// Time provider returning a configurable time-slot mapping.
class fake_ntn_time_provider : public ntn_time_provider
{
public:
  std::optional<ntn_time_slot_mapping> mapping;

  std::optional<ntn_time_slot_mapping> get_last_mapping(const nr_cell_global_id_t& nr_cgi,
                                                        subcarrier_spacing         scs) override
  {
    return mapping;
  }
};

/// Meas info update handler capturing all received requests.
class meas_info_capture : public ntn_meas_info_update_handler
{
public:
  std::vector<ntn_meas_info_update_request> reqs;

  void handle_ntn_meas_info_update(const ntn_meas_info_update_request& req) override { reqs.push_back(req); }
};

} // namespace

class ntn_configuration_manager_test : public ::testing::Test
{
protected:
  using time_point = std::chrono::system_clock::time_point;

  static constexpr unsigned update_period_ms = 32;

  /// Fixed reference time (2025-06-24T09:00:00 UTC) used as satellite epoch and current time.
  const time_point t0{std::chrono::seconds{1750755600}};

  void create_manager(const ntn_configuration_manager_config& cfg)
  {
    auto tp       = std::make_unique<fake_ntn_time_provider>();
    time_provider = tp.get();
    auto mh       = std::make_unique<meas_info_capture>();
    meas_handler  = mh.get();

    ntn_configuration_manager_dependencies deps{/*sib19_msg_update_handler=*/nullptr,
                                                std::move(tp),
                                                /*doppler_handler=*/nullptr,
                                                std::move(mh),
                                                timers,
                                                worker};
    manager = create_ntn_configuration_manager(cfg, std::move(deps));
  }

  /// Advances time by the given number of milliseconds, running expired timers.
  void tick(unsigned nof_ms)
  {
    for (unsigned i = 0; i != nof_ms; ++i) {
      timers.tick();
      worker.run_pending_tasks();
    }
  }

  /// Builds a manager config with one measurement-driven cell (update_period) and one NTN neighbour cell.
  ntn_configuration_manager_config make_meas_cell_config()
  {
    ntn_configuration_manager_config cfg;

    ntn_satellite_config& sat = cfg.satellites.emplace_back();
    sat.satellite_index       = 0;
    sat.epoch_timestamp       = t0;
    ecef_coordinates_t ecef;
    ecef.position_x    = -3621225.25;
    ecef.position_y    = -5839350.24;
    ecef.position_z    = 101120.52;
    ecef.velocity_vx   = 3498.87;
    ecef.velocity_vy   = -2055.89;
    ecef.velocity_vz   = 6104.62;
    sat.ephemeris_info = ecef;

    ntn_cell_config& cell = cfg.cells.emplace_back();
    cell.nr_cgi.plmn_id   = plmn_identity::test_value();
    cell.nr_cgi.nci       = nr_cell_identity::create(0x19b0).value();
    cell.update_period    = std::chrono::milliseconds(update_period_ms);

    ntn_neighbor_cell_config& ncell = cell.ncells.emplace_back();
    ncell.satellite_index           = 0;
    ncell.nci                       = nr_cell_identity::create(0x19b1).value();
    ncell.use_state_vector          = true;
    geodetic_coordinates_t ref_loc;
    ref_loc.latitude         = 12.0;
    ref_loc.longitude        = 45.0;
    ref_loc.altitude         = 0.0;
    ncell.reference_location = ref_loc;

    return cfg;
  }

  ntn_time_slot_mapping make_mapping(unsigned sfn = 100) const
  {
    return ntn_time_slot_mapping{slot_point{subcarrier_spacing::kHz15, sfn, 0}, t0};
  }

  manual_task_worker                         worker{64};
  timer_manager                              timers;
  fake_ntn_time_provider*                    time_provider = nullptr;
  meas_info_capture*                         meas_handler  = nullptr;
  std::unique_ptr<ntn_configuration_manager> manager;
};

TEST_F(ntn_configuration_manager_test, meas_cell_publishes_ntn_neighbour_info_with_epoch_in_serving_sfn)
{
  ntn_configuration_manager_config cfg = make_meas_cell_config();
  create_manager(cfg);
  time_provider->mapping = make_mapping(100);

  tick(update_period_ms);

  ASSERT_EQ(meas_handler->reqs.size(), 1);
  const ntn_meas_info_update_request& req = meas_handler->reqs.front();
  EXPECT_EQ(req.serving_cgi.nci, cfg.cells.front().nr_cgi.nci);
  ASSERT_EQ(req.ncells.size(), 1);

  const ntn_neighbour_meas_info& info = req.ncells.front();
  EXPECT_EQ(info.nci, *cfg.cells.front().ncells.front().nci);
  // Without SIB19 the epoch is anchored to the current slot (sfn=100, slot=0), not an update period ahead.
  EXPECT_EQ(info.epoch_time.sfn, 100U);
  EXPECT_EQ(info.epoch_time.subframe_number, 0U);
  // The neighbour requested the state vector ephemeris format.
  EXPECT_TRUE(std::holds_alternative<ecef_coordinates_t>(info.ephemeris));
  ASSERT_TRUE(info.ref_location.has_value());
  EXPECT_DOUBLE_EQ(info.ref_location->latitude, 12.0);
  EXPECT_DOUBLE_EQ(info.ref_location->longitude, 45.0);
}

TEST_F(ntn_configuration_manager_test, no_publication_until_time_mapping_is_available)
{
  create_manager(make_meas_cell_config());

  // No time-slot mapping available yet (no reference time report received).
  tick(update_period_ms);
  EXPECT_TRUE(meas_handler->reqs.empty());

  // Once the mapping becomes available, the next period publishes.
  time_provider->mapping = make_mapping();
  tick(update_period_ms);
  EXPECT_EQ(meas_handler->reqs.size(), 1);
}

TEST_F(ntn_configuration_manager_test, ncell_without_nci_is_not_published)
{
  ntn_configuration_manager_config cfg = make_meas_cell_config();
  // Add a second neighbour without NCI (SIB19-only neighbour).
  ntn_neighbor_cell_config& ncell = cfg.cells.front().ncells.emplace_back();
  ncell.satellite_index           = 0;
  create_manager(cfg);
  time_provider->mapping = make_mapping();

  tick(update_period_ms);

  ASSERT_EQ(meas_handler->reqs.size(), 1);
  ASSERT_EQ(meas_handler->reqs.front().ncells.size(), 1);
  EXPECT_EQ(meas_handler->reqs.front().ncells.front().nci, *cfg.cells.front().ncells.front().nci);
}
