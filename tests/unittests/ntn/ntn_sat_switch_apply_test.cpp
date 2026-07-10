// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/ntn/ntn_configuration_manager_impl.h"
#include "lib/ntn/ntn_sat_switch_helpers.h"
#include "ocudu/ntn/ntn_sib19_update_handler.h"
#include "ocudu/ntn/ntn_time_provider.h"
#include "ocudu/support/executors/manual_task_worker.h"
#include "ocudu/support/timers.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocudu_ntn;

namespace {

ntn_cell_config make_base_config()
{
  ntn_cell_config cfg{};
  cfg.si_sched = ntn_si_scheduling_info{/*si_msg_idx=*/0,
                                        /*si_period_rf=*/1,
                                        /*si_window_len_slots=*/1,
                                        /*si_window_position=*/1};

  ntn_serving_cell_config serving{};
  serving.satellite_index          = 1;
  serving.cell_specific_koffset    = std::chrono::milliseconds{10};
  serving.ntn_ul_sync_validity_dur = 30U;
  serving.k_mac                    = 2U;
  serving.ta_report                = true;
  serving.use_state_vector         = true;
  serving.reference_location       = geodetic_coordinates_t{1.0, 2.0, 3.0};
  serving.t_service                = std::chrono::system_clock::time_point{std::chrono::seconds{2000}};
  cfg.ntn_cfg                      = serving;

  ntn_neighbor_cell_config ncell{};
  ncell.satellite_index = 5;
  cfg.ncells.push_back(ncell);

  return cfg;
}

ntn_sat_switch_config make_sat_switch(unsigned satellite_index)
{
  ntn_sat_switch_config sw{};
  sw.satellite_index    = satellite_index;
  sw.t_service_start    = std::chrono::system_clock::time_point{std::chrono::seconds{1000}};
  sw.promote_to_serving = true;
  return sw;
}

class fake_sib19_update_handler : public ntn_sib19_update_handler
{
public:
  std::vector<ntn_sib19_update_request> requests;

  void handle_sib19_msg_update(const ntn_sib19_update_request& req) override { requests.push_back(req); }
};

/// Returns an advancing (slot, time) pair on each call: each call moves 10ms/10 slots forward from start_time,
/// mirroring the 10 ticks that elapse between successive periodic-task timer firings (si_period_rf == 1 -> 10ms
/// period). The time/slot are advanced before being returned so that the Nth call reflects N*10ms of elapsed real
/// timer_manager ticks, matching when the per-cell timer actually fires.
class fake_ntn_time_provider : public ntn_time_provider
{
public:
  explicit fake_ntn_time_provider(std::chrono::system_clock::time_point start_time) : cur_time(start_time) {}

  std::optional<ntn_time_slot_mapping> get_last_mapping(const nr_cell_global_id_t& nr_cgi,
                                                        subcarrier_spacing         scs) override
  {
    slot_count += 10;
    cur_time += std::chrono::milliseconds(10);
    ntn_time_slot_mapping mapping;
    mapping.slot_tx    = slot_point(0, slot_count); // numerology 0 == 15kHz SCS, 1 slot == 1ms
    mapping.time_point = cur_time;
    return mapping;
  }

private:
  unsigned                              slot_count = 0;
  std::chrono::system_clock::time_point cur_time;
};

} // namespace

TEST(derive_post_switch_config_test, returns_nullopt_when_no_sat_switch)
{
  ntn_cell_config cfg = make_base_config();
  EXPECT_FALSE(derive_post_switch_config(cfg).has_value());
}

TEST(derive_post_switch_config_test, returns_nullopt_when_no_serving_ntn_cfg)
{
  ntn_cell_config cfg = make_base_config();
  cfg.ntn_cfg.reset();
  cfg.sat_switch = make_sat_switch(9);
  EXPECT_FALSE(derive_post_switch_config(cfg).has_value());
}

TEST(derive_post_switch_config_test, returns_nullopt_when_serving_has_no_t_service)
{
  ntn_cell_config cfg = make_base_config();
  cfg.sat_switch      = make_sat_switch(9);
  cfg.ntn_cfg->t_service.reset();
  EXPECT_FALSE(derive_post_switch_config(cfg).has_value())
      << "per TS 38.331 clause 5.7.19 the switch only executes when t-Service is broadcast";
}

TEST(derive_post_switch_config_test, switches_satellite_index_and_clears_sat_switch)
{
  ntn_cell_config cfg = make_base_config();
  cfg.sat_switch      = make_sat_switch(9);

  auto derived = derive_post_switch_config(cfg);
  ASSERT_TRUE(derived.has_value());
  ASSERT_TRUE(derived->ntn_cfg.has_value());
  EXPECT_EQ(derived->ntn_cfg->satellite_index, 9U);
  EXPECT_FALSE(derived->sat_switch.has_value());
  EXPECT_FALSE(derived->ntn_cfg->t_service.has_value())
      << "t_service described the source satellite's service stop and must not survive the switch";
}

TEST(derive_post_switch_config_test, applies_sat_switch_overrides_when_set)
{
  ntn_cell_config cfg                      = make_base_config();
  cfg.sat_switch                           = make_sat_switch(9);
  cfg.sat_switch->ntn_ul_sync_validity_dur = 60U;
  cfg.sat_switch->cell_specific_koffset    = std::chrono::milliseconds{20};
  cfg.sat_switch->k_mac                    = 4U;
  cfg.sat_switch->ta_report                = false;
  cfg.sat_switch->use_state_vector         = false;
  cfg.sat_switch->polarization = ntn_polarization_t{ntn_polarization_t::polarization_type::lhcp, std::nullopt};

  auto derived = derive_post_switch_config(cfg);
  ASSERT_TRUE(derived.has_value());
  EXPECT_EQ(derived->ntn_cfg->ntn_ul_sync_validity_dur, 60U);
  EXPECT_EQ(derived->ntn_cfg->cell_specific_koffset, std::chrono::milliseconds{20});
  EXPECT_EQ(*derived->ntn_cfg->k_mac, 4U);
  EXPECT_EQ(*derived->ntn_cfg->ta_report, false);
  EXPECT_EQ(*derived->ntn_cfg->use_state_vector, false);
  ASSERT_TRUE(derived->ntn_cfg->polarization.has_value());
  EXPECT_EQ(*derived->ntn_cfg->polarization->dl, ntn_polarization_t::polarization_type::lhcp);
}

TEST(derive_post_switch_config_test, falls_back_to_current_value_when_sat_switch_leaves_field_unset)
{
  ntn_cell_config cfg = make_base_config();
  cfg.sat_switch      = make_sat_switch(9); // leaves ntn_ul_sync_validity_dur, k_mac, etc. unset

  auto derived = derive_post_switch_config(cfg);
  ASSERT_TRUE(derived.has_value());
  EXPECT_EQ(derived->ntn_cfg->ntn_ul_sync_validity_dur, 30U); // unchanged from make_base_config()
  EXPECT_EQ(derived->ntn_cfg->cell_specific_koffset, std::chrono::milliseconds{10});
  EXPECT_EQ(*derived->ntn_cfg->k_mac, 2U);
  EXPECT_EQ(*derived->ntn_cfg->ta_report, true);
  EXPECT_EQ(*derived->ntn_cfg->use_state_vector, true);
}

TEST(derive_post_switch_config_test, preserves_ncells_when_promote_neighbors_enabled)
{
  ntn_cell_config cfg               = make_base_config();
  cfg.sat_switch                    = make_sat_switch(9);
  cfg.sat_switch->promote_neighbors = true;

  auto derived = derive_post_switch_config(cfg);
  ASSERT_TRUE(derived.has_value());
  ASSERT_EQ(derived->ncells.size(), 1U);
  EXPECT_EQ(derived->ncells[0].satellite_index, 5U);
  ASSERT_TRUE(derived->ntn_cfg->reference_location.has_value());
  EXPECT_DOUBLE_EQ(derived->ntn_cfg->reference_location->latitude, 1.0);
}

TEST(derive_post_switch_config_test, returns_nullopt_when_promote_to_serving_is_false)
{
  ntn_cell_config cfg                = make_base_config();
  cfg.sat_switch                     = make_sat_switch(9);
  cfg.sat_switch->promote_to_serving = false;
  EXPECT_FALSE(derive_post_switch_config(cfg).has_value());
}

TEST(derive_post_switch_config_test, clears_ncells_by_default)
{
  ntn_cell_config cfg = make_base_config();
  cfg.sat_switch      = make_sat_switch(9); // promote_neighbors left at its default (false)

  auto derived = derive_post_switch_config(cfg);
  ASSERT_TRUE(derived.has_value());
  EXPECT_TRUE(derived->ncells.empty());
}

TEST(sat_switch_apply_integration_test, promotes_switch_target_at_t_service_not_before)
{
  const auto t0 = std::chrono::system_clock::time_point{std::chrono::seconds{1000}};

  ntn_configuration_manager_config cfg{};

  ntn_satellite_config sat0{};
  sat0.satellite_index = 1;
  sat0.epoch_timestamp = t0;
  sat0.ephemeris_info  = ecef_coordinates_t{7000000, 0, 0, 0, 7500, 0};
  cfg.satellites.push_back(sat0);

  ntn_satellite_config sat1 = sat0;
  sat1.satellite_index      = 9;
  cfg.satellites.push_back(sat1);

  ntn_cell_config cell{};
  // 10ms period -> one timer firing per 10 ticks (si_period_rf == 1).
  cell.si_sched = ntn_si_scheduling_info{/*si_msg_idx=*/0,
                                         /*si_period_rf=*/1,
                                         /*si_window_len_slots=*/1,
                                         /*si_window_position=*/1};

  ntn_serving_cell_config serving{};
  serving.satellite_index          = 1;
  serving.cell_specific_koffset    = std::chrono::milliseconds{10};
  serving.ntn_ul_sync_validity_dur = 30U;
  serving.t_service                = t0 + std::chrono::milliseconds(15); // between the 1st and 2nd firing
  cell.ntn_cfg                     = serving;

  ntn_sat_switch_config sat_switch{};
  sat_switch.satellite_index    = 9;
  sat_switch.t_service_start    = t0 + std::chrono::milliseconds(5); // overlap window opens before the 1st firing
  sat_switch.promote_to_serving = true;
  cell.sat_switch               = sat_switch;

  cfg.cells.push_back(cell);

  timer_manager      timers;
  manual_task_worker executor{16};

  auto  sib19_handler = std::make_unique<fake_sib19_update_handler>();
  auto* sib19_ptr     = sib19_handler.get();

  ntn_configuration_manager_dependencies deps{
      std::move(sib19_handler), std::make_unique<fake_ntn_time_provider>(t0), nullptr, timers, executor};

  ntn_configuration_manager_impl manager(cfg, std::move(deps));

  // First firing (10 ticks == 10ms): inside the overlap window (past t_service_start at 5ms, before t_service at
  // 15ms) -- per TS 38.331 clause 5.7.19 the switch executes at t-Service, so the source satellite must still be
  // serving and the switch must still be advertised.
  // The timer's expiry callback is dispatched via task_executor::defer(), which manual_task_worker always enqueues
  // rather than running inline, so it must be pumped explicitly.
  for (int i = 0; i != 10; ++i) {
    timers.tick();
  }
  executor.run_pending_tasks();
  ASSERT_GE(sib19_ptr->requests.size(), 1U);
  EXPECT_TRUE(sib19_ptr->requests.back().sib19.sat_switch_with_resync.has_value())
      << "switch must stay advertised through the overlap window, until t_service";

  // Second firing (10 more ticks, 20ms total): past t_service (15ms), switch applied.
  for (int i = 0; i != 10; ++i) {
    timers.tick();
  }
  executor.run_pending_tasks();
  ASSERT_GE(sib19_ptr->requests.size(), 2U);
  EXPECT_FALSE(sib19_ptr->requests.back().sib19.sat_switch_with_resync.has_value());
  EXPECT_FALSE(sib19_ptr->requests.back().sib19.t_service.has_value())
      << "the source satellite's t_service must not be broadcast after the switch";
}

TEST(sat_switch_apply_integration_test, does_not_promote_when_promote_to_serving_is_false)
{
  const auto t0 = std::chrono::system_clock::time_point{std::chrono::seconds{1000}};

  ntn_configuration_manager_config cfg{};

  ntn_satellite_config sat0{};
  sat0.satellite_index = 1;
  sat0.epoch_timestamp = t0;
  sat0.ephemeris_info  = ecef_coordinates_t{7000000, 0, 0, 0, 7500, 0};
  cfg.satellites.push_back(sat0);

  ntn_satellite_config sat1 = sat0;
  sat1.satellite_index      = 9;
  cfg.satellites.push_back(sat1);

  ntn_cell_config cell{};
  cell.si_sched = ntn_si_scheduling_info{/*si_msg_idx=*/0,
                                         /*si_period_rf=*/1,
                                         /*si_window_len_slots=*/1,
                                         /*si_window_position=*/1};

  ntn_serving_cell_config serving{};
  serving.satellite_index          = 1;
  serving.cell_specific_koffset    = std::chrono::milliseconds{10};
  serving.ntn_ul_sync_validity_dur = 30U;
  serving.t_service                = t0 + std::chrono::milliseconds(15);
  cell.ntn_cfg                     = serving;

  ntn_sat_switch_config sat_switch{};
  sat_switch.satellite_index    = 9;
  sat_switch.t_service_start    = t0 + std::chrono::milliseconds(5);
  sat_switch.promote_to_serving = false; // explicitly disabled
  cell.sat_switch               = sat_switch;

  cfg.cells.push_back(cell);

  timer_manager      timers;
  manual_task_worker executor{16};

  auto  sib19_handler = std::make_unique<fake_sib19_update_handler>();
  auto* sib19_ptr     = sib19_handler.get();

  ntn_configuration_manager_dependencies deps{
      std::move(sib19_handler), std::make_unique<fake_ntn_time_provider>(t0), nullptr, timers, executor};

  ntn_configuration_manager_impl manager(cfg, std::move(deps));

  for (int i = 0; i != 10; ++i) {
    timers.tick();
  }
  executor.run_pending_tasks();
  ASSERT_GE(sib19_ptr->requests.size(), 1U);
  EXPECT_TRUE(sib19_ptr->requests.back().sib19.sat_switch_with_resync.has_value());

  // Past t_service: with promote_to_serving disabled, the switch stays advertised and the serving
  // satellite never changes -- there's no automatic transition into a "post-switch" state.
  for (int i = 0; i != 10; ++i) {
    timers.tick();
  }
  executor.run_pending_tasks();
  ASSERT_GE(sib19_ptr->requests.size(), 2U);
  EXPECT_TRUE(sib19_ptr->requests.back().sib19.sat_switch_with_resync.has_value());
}
