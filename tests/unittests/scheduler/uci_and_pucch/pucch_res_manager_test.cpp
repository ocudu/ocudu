// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/scheduler/cell/resource_grid.h"
#include "lib/scheduler/config/du_cell_group_config_pool.h"
#include "lib/scheduler/config/ue_configuration.h"
#include "lib/scheduler/pucch_scheduling/pucch_resource_manager.h"
#include "lib/scheduler/ue_context/ue_repository.h"
#include "tests/test_doubles/scheduler/pucch_res_test_builder_helper.h"
#include "tests/test_doubles/scheduler/scheduler_config_helper.h"
#include "tests/unittests/scheduler/test_utils/config_generators.h"
#include "ocudu/ran/du_types.h"
#include "ocudu/ran/pucch/pucch_configuration.h"
#include "ocudu/ran/pucch/pucch_constants.h"
#include "ocudu/scheduler/config/pucch_resource_builder_params.h"
#include "ocudu/scheduler/config/sched_cell_config_helpers.h"
#include "ocudu/scheduler/config/scheduler_expert_config_factory.h"
#include "ocudu/scheduler/config/serving_cell_config_builder.h"
#include "ocudu/scheduler/scheduler_configurator.h"
#include <gtest/gtest.h>

using namespace ocudu;

////////////    Test the PUCCH resource manager: all UEs sharing the same config     ////////////

class res_manager_test_bench
{
public:
  explicit res_manager_test_bench(const pucch_resource_builder_params& builder_params_) :
    builder_params(builder_params_),
    cell_cfg(*cfg_mng.add_cell([this]() {
      // Create cell and save a reference to its configuration.
      auto req                         = sched_config_helper::make_default_sched_cell_configuration_request();
      req.ran.init_bwp.pucch.resources = builder_params;
      return req;
    }())),
    cell_res_list(cell_cfg.bwp_res[to_bwp_id(0)].ul().pucch.dedicated),
    ues(config_helpers::make_default_scheduler_expert_config().ue),
    cell_ues(ues.add_cell(cell_cfg, nullptr)),
    pucch_builder(cell_cfg.expert_cfg.ue.max_pucchs_per_slot),
    res_manager(cell_cfg),
    slot_alloc(cell_cfg),
    sl_tx(slot_point(0, 0))
  {
    pucch_builder.setup(cell_cfg.params);
    res_manager.slot_indication(sl_tx);
    slot_alloc.slot_indication(sl_tx);
  }

  sched_ue_creation_request_message make_ue_creation_req(std::optional<pucch_sr_resource_id>  sr_res_id_override  = {},
                                                         std::optional<pucch_csi_resource_id> csi_res_id_override = {})
  {
    sched_ue_creation_request_message ue_req =
        sched_config_helper::create_default_sched_ue_creation_request(cell_cfg.params);

    if (not ue_ded_cfgs.empty()) {
      ue_req.ue_index =
          to_du_ue_index(static_cast<std::underlying_type_t<du_ue_index_t>>(ue_ded_cfgs.back()->ue_index) + 1);
      ue_req.crnti = to_rnti(static_cast<std::underlying_type_t<rnti_t>>(ue_ded_cfgs.back()->crnti) + 1);
    }

    const bool success = pucch_builder.add_build_new_ue_pucch_cfg(ue_req.cfg.cells->back());
    ocudu_assert(success, "UE PUCCH configuration couldn't be built");

    // Apply any resource overrides, rebuilding serv_cell_cfg to stay consistent with ue_bwp_config.
    if (sr_res_id_override.has_value() or csi_res_id_override.has_value()) {
      auto& ue_cell = ue_req.cfg.cells->back();
      if (sr_res_id_override.has_value()) {
        ue_cell.init_bwp().ul.pucch.sr_res_id = *sr_res_id_override;
      }
      if (csi_res_id_override.has_value() and ue_cell.init_bwp().ul.periodic_csi_report.has_value()) {
        ue_cell.init_bwp().ul.periodic_csi_report->pucch_res_id = *csi_res_id_override;
      }
      ue_cell.serv_cell_cfg.ul_config->init_ul_bwp.pucch_cfg =
          config_helpers::build_pucch_config(cell_cfg.params, cell_cfg.bwp_res[to_bwp_id(0)].ul(), ue_cell.init_bwp());
      ue_cell.serv_cell_cfg.csi_meas_cfg = config_helpers::build_csi_meas_config(
          cell_cfg.params, cell_cfg.bwp_res[to_bwp_id(0)].ul(), ue_cell.init_bwp());
    }

    return ue_req;
  }

  const pucch_resource_builder_params builder_params;

  // Configuration dependencies.
  const scheduler_expert_config           expert_cfg = config_helpers::make_default_scheduler_expert_config();
  test_helpers::test_sched_config_manager cfg_mng{{}, expert_cfg};
  const cell_configuration&               cell_cfg;
  const std::vector<pucch_resource>&      cell_res_list;
  ue_repository                           ues;
  ue_cell_repository&                     cell_ues;

  pucch_res_builder_test_helper pucch_builder;
  pucch_resource_manager        res_manager;
  cell_slot_resource_allocator  slot_alloc;

  std::vector<const ue_configuration*> ue_ded_cfgs;
  slot_point                           sl_tx;
};

class test_pucch_resource_manager : public ::testing::Test
{
public:
  test_pucch_resource_manager() :
    t_bench(pucch_resource_builder_params{
        .nof_cell_sr_resources = 2U,
    })
  {
    auto ue_req = t_bench.make_ue_creation_req();
    t_bench.ue_ded_cfgs.push_back(t_bench.cfg_mng.add_ue(ue_req));
    t_bench.ues.add_ue(
        *t_bench.ue_ded_cfgs.back(), ue_req.starts_in_fallback, ue_req.ul_ccch_slot_rx, ue_req.cfra_enabled);

    ue_req = t_bench.make_ue_creation_req(pucch_sr_resource_id(1));
    t_bench.ue_ded_cfgs.push_back(t_bench.cfg_mng.add_ue(ue_req));
    t_bench.ues.add_ue(
        *t_bench.ue_ded_cfgs.back(), ue_req.starts_in_fallback, ue_req.ul_ccch_slot_rx, ue_req.cfra_enabled);
  }

  const ue_cell_configuration& ue_cfg_0() const { return t_bench.ue_ded_cfgs[0]->pcell_cfg(); }
  const ue_cell_configuration& ue_cfg_1() const { return t_bench.ue_ded_cfgs[1]->pcell_cfg(); }
  const pucch_config&          pucch_cfg_0() const { return ue_cfg_0().init_bwp().ul.ded()->pucch_cfg.value(); }
  const pucch_config&          pucch_cfg_1() const { return ue_cfg_1().init_bwp().ul.ded()->pucch_cfg.value(); }

protected:
  res_manager_test_bench t_bench;

  // Allocate PUCCH for a given number of UEs, in increasing order of RNTI.
  void allocate_ues(unsigned nof_ues_to_allocate, bool format_2 = false)
  {
    for (size_t n = 0; n != nof_ues_to_allocate; ++n) {
      const rnti_t                                 rnti = to_rnti(0x4601 + n);
      pucch_resource_manager::ue_reservation_guard guard(&t_bench.res_manager, t_bench.slot_alloc, rnti, ue_cfg_0());
      if (format_2) {
        guard.reserve_harq_set_1_resource_next_available();
      } else {
        guard.reserve_harq_set_0_resource_next_available();
      }
      guard.commit();
    }
  }
};

TEST_F(test_pucch_resource_manager, common_res_available_intialization)
{
  for (unsigned r_pucch = 0; r_pucch != pucch_constants::MAX_NOF_CELL_COMMON_PUCCH_RESOURCES; ++r_pucch) {
    ASSERT_TRUE(
        t_bench.res_manager.reserve_harq_common_resource(t_bench.slot_alloc.ul_res_grid, t_bench.sl_tx, r_pucch));
  }
}

TEST_F(test_pucch_resource_manager, common_res_available_reserve_and_check)
{
  const unsigned reserved_r_pucch = 13;
  ASSERT_TRUE(t_bench.res_manager.reserve_harq_common_resource(
      t_bench.slot_alloc.ul_res_grid, t_bench.sl_tx, reserved_r_pucch));
  for (unsigned r_pucch = 0; r_pucch != pucch_constants::MAX_NOF_CELL_COMMON_PUCCH_RESOURCES; ++r_pucch) {
    if (r_pucch == reserved_r_pucch) {
      continue;
    }
    ASSERT_TRUE(
        t_bench.res_manager.reserve_harq_common_resource(t_bench.slot_alloc.ul_res_grid, t_bench.sl_tx, r_pucch));
  }

  // Increment slot point and invoke slot_indication(), which should reset the previous UE's resource allocation.
  ++t_bench.sl_tx;
  t_bench.res_manager.slot_indication(t_bench.sl_tx);
  t_bench.slot_alloc.slot_indication(t_bench.sl_tx);
  for (unsigned r_pucch_it = 0; r_pucch_it != pucch_constants::MAX_NOF_CELL_COMMON_PUCCH_RESOURCES; ++r_pucch_it) {
    ASSERT_TRUE(
        t_bench.res_manager.reserve_harq_common_resource(t_bench.slot_alloc.ul_res_grid, t_bench.sl_tx, r_pucch_it));
  }
}

TEST_F(test_pucch_resource_manager, reserve_harq_set_0_until_exhausted)
{
  const unsigned res_set_size = pucch_cfg_0().pucch_res_set[0].resources.size();
  for (unsigned i = 0; i != res_set_size; ++i) {
    pucch_resource_manager::ue_reservation_guard guard(
        &t_bench.res_manager, t_bench.slot_alloc, to_rnti(0x4601 + i), ue_cfg_0());
    const auto record = guard.reserve_harq_set_0_resource_next_available();
    ASSERT_EQ(i, record.pucch_res_indicator);
    ASSERT_EQ(&t_bench.cell_cfg.bwp_res[to_bwp_id(0)]
                   .ul()
                   .pucch.dedicated[pucch_cfg_0().pucch_res_set[0].resources[i].ded().cell_res_id],
              record.resource);
    guard.commit();
  }

  pucch_resource_manager::ue_reservation_guard guard(
      &t_bench.res_manager, t_bench.slot_alloc, to_rnti(0x4601 + res_set_size), ue_cfg_0());
  const pucch_harq_resource_alloc_record record = guard.reserve_harq_set_0_resource_next_available();
  ASSERT_EQ(nullptr, record.resource);
}

TEST_F(test_pucch_resource_manager, get_next_harq_different_slot)
{
  allocate_ues(1);
  ++t_bench.sl_tx;
  t_bench.res_manager.slot_indication(t_bench.sl_tx);
  t_bench.slot_alloc.slot_indication(t_bench.sl_tx);

  pucch_resource_manager::ue_reservation_guard guard(
      &t_bench.res_manager, t_bench.slot_alloc, to_rnti(0x4602), ue_cfg_0());
  const pucch_harq_resource_alloc_record record = guard.reserve_harq_set_0_resource_next_available();

  // Expect that pucch_res_indicator = 0 is returned, as the UE 0x4602 is allocated in a different slot to UE 0x4601.
  ASSERT_EQ(0U, record.pucch_res_indicator);
  ASSERT_EQ(&t_bench.cell_res_list[pucch_cfg_0().pucch_res_set[0].resources[0].ded().cell_res_id], record.resource);
}

TEST_F(test_pucch_resource_manager, allocate_and_release_f1)
{
  pucch_resource_manager::ue_reservation_guard guard(
      &t_bench.res_manager, t_bench.slot_alloc, to_rnti(0x4601), ue_cfg_0());
  const pucch_harq_resource_alloc_record record = guard.reserve_harq_set_0_resource_next_available();

  ASSERT_EQ(0, record.pucch_res_indicator);
  ASSERT_EQ(&t_bench.cell_res_list[pucch_cfg_0().pucch_res_set[0].resources[0].ded().cell_res_id], record.resource);

  // Release the resource and verify the UE does not hold it anymore.
  ASSERT_TRUE(guard.release_harq_set_0_resource());

  // Re-allocate the resource.
  const pucch_harq_resource_alloc_record reallocation = guard.reserve_harq_set_0_resource_next_available();
  ASSERT_EQ(0U, reallocation.pucch_res_indicator);
  ASSERT_EQ(record.resource, reallocation.resource);
}

TEST_F(test_pucch_resource_manager, allocate_and_release_multiple_ues)
{
  // Allocate 3 UEs.
  allocate_ues(3);

  pucch_resource_manager::ue_reservation_guard guard1(
      &t_bench.res_manager, t_bench.slot_alloc, to_rnti(0x4601), ue_cfg_0());
  ASSERT_TRUE(guard1.release_harq_set_0_resource());
  pucch_resource_manager::ue_reservation_guard guard3(
      &t_bench.res_manager, t_bench.slot_alloc, to_rnti(0x4603), ue_cfg_0());
  ASSERT_TRUE(guard3.release_harq_set_0_resource());

  // Re-allocate the resources to UE1 and UE3.
  const pucch_harq_resource_alloc_record realloc_ue1 = guard1.reserve_harq_set_0_resource_next_available();
  const pucch_harq_resource_alloc_record realloc_ue3 = guard3.reserve_harq_set_0_resource_next_available();

  ASSERT_EQ(0U, realloc_ue1.pucch_res_indicator);
  ASSERT_EQ(2U, realloc_ue3.pucch_res_indicator);
}

TEST_F(test_pucch_resource_manager, reserve_harq_set_1_until_exhausted)
{
  const unsigned res_set_size = pucch_cfg_0().pucch_res_set[1].resources.size();
  for (unsigned i = 0; i != res_set_size; ++i) {
    pucch_resource_manager::ue_reservation_guard guard(
        &t_bench.res_manager, t_bench.slot_alloc, to_rnti(0x4601 + i), ue_cfg_0());
    const auto record = guard.reserve_harq_set_1_resource_next_available();
    ASSERT_EQ(i, record.pucch_res_indicator);
    const unsigned cell_res_id = pucch_cfg_0().pucch_res_set[1].resources[record.pucch_res_indicator].ded().cell_res_id;
    ASSERT_EQ(&t_bench.cell_res_list[cell_res_id], record.resource);
    guard.commit();
  }

  pucch_resource_manager::ue_reservation_guard guard(
      &t_bench.res_manager, t_bench.slot_alloc, to_rnti(0x4601 + res_set_size), ue_cfg_0());
  const pucch_harq_resource_alloc_record record = guard.reserve_harq_set_1_resource_next_available();
  ASSERT_EQ(nullptr, record.resource);
}

TEST_F(test_pucch_resource_manager, allocate_csi_resource)
{
  pucch_resource_manager::ue_reservation_guard guard(
      &t_bench.res_manager, t_bench.slot_alloc, to_rnti(0x4601), ue_cfg_0());
  const pucch_resource* res = guard.reserve_csi_resource();

  const unsigned expected_csi_cell_res_id = std::get<csi_report_config::periodic_or_semi_persistent_report_on_pucch>(
                                                ue_cfg_0().csi_meas_cfg()->csi_report_cfg_list[0].report_cfg_type)
                                                .pucch_csi_res_list[0]
                                                .pucch_res_id.ded()
                                                .cell_res_id;
  ASSERT_EQ(&t_bench.cell_res_list[expected_csi_cell_res_id], res);
}

TEST_F(test_pucch_resource_manager, release_and_reallocate_csi_resource)
{
  pucch_resource_manager::ue_reservation_guard guard(
      &t_bench.res_manager, t_bench.slot_alloc, to_rnti(0x4601), ue_cfg_0());
  // There is no allocated resource, expects false from the release.
  ASSERT_FALSE(guard.release_csi_resource());

  const unsigned expected_csi_cell_res_id = std::get<csi_report_config::periodic_or_semi_persistent_report_on_pucch>(
                                                ue_cfg_0().csi_meas_cfg()->csi_report_cfg_list[0].report_cfg_type)
                                                .pucch_csi_res_list[0]
                                                .pucch_res_id.ded()
                                                .cell_res_id;
  const pucch_resource* res = guard.reserve_csi_resource();
  ASSERT_EQ(&t_bench.cell_res_list[expected_csi_cell_res_id], res);

  const pucch_resource* res_second_allc_no_release = guard.reserve_csi_resource();
  ASSERT_EQ(res, res_second_allc_no_release);

  // This time the release it supposed to return true.
  ASSERT_TRUE(guard.release_csi_resource());

  const pucch_resource* res_reallocation = guard.reserve_csi_resource();
  ASSERT_EQ(&t_bench.cell_res_list[expected_csi_cell_res_id], res_reallocation);
}

TEST_F(test_pucch_resource_manager, get_format2_different_slot)
{
  allocate_ues(1, true);
  ++t_bench.sl_tx;
  t_bench.res_manager.slot_indication(t_bench.sl_tx);
  t_bench.slot_alloc.slot_indication(t_bench.sl_tx);

  pucch_resource_manager::ue_reservation_guard guard(
      &t_bench.res_manager, t_bench.slot_alloc, to_rnti(0x4602), ue_cfg_0());
  const pucch_harq_resource_alloc_record record = guard.reserve_harq_set_1_resource_next_available();

  // Expect that pucch_res_indicator = 0 is returned, as the UE 0x4602 is allocated in a different slot to UE 0x4601.
  ASSERT_EQ(0U, record.pucch_res_indicator);
  const unsigned cell_res_id = pucch_cfg_0().pucch_res_set[1].resources[record.pucch_res_indicator].ded().cell_res_id;
  ASSERT_EQ(&t_bench.cell_res_list[cell_res_id], record.resource);
}

TEST_F(test_pucch_resource_manager, allocate_and_release_f2)
{
  pucch_resource_manager::ue_reservation_guard guard(
      &t_bench.res_manager, t_bench.slot_alloc, to_rnti(0x4601), ue_cfg_0());
  const pucch_harq_resource_alloc_record record = guard.reserve_harq_set_1_resource_next_available();

  ASSERT_EQ(0U, record.pucch_res_indicator);
  unsigned cell_res_id = pucch_cfg_0().pucch_res_set[1].resources[record.pucch_res_indicator].ded().cell_res_id;
  ASSERT_EQ(&t_bench.cell_res_list[cell_res_id], record.resource);

  // Release the resource and verify the UE does not hold it anymore.
  ASSERT_TRUE(guard.release_harq_set_1_resource());

  // Re-allocate the resource.
  const pucch_harq_resource_alloc_record reallocation = guard.reserve_harq_set_1_resource_next_available();
  ASSERT_EQ(0, reallocation.pucch_res_indicator);
  ASSERT_EQ(record.resource, reallocation.resource);
}

TEST_F(test_pucch_resource_manager, allocate_and_release_f2_multiple_ues)
{
  // Allocate 6 UEs.
  allocate_ues(6, true);

  // Release the resource and verify that the UEs do not hold it anymore.
  pucch_resource_manager::ue_reservation_guard guard1(
      &t_bench.res_manager, t_bench.slot_alloc, to_rnti(0x4601), ue_cfg_0());
  ASSERT_TRUE(guard1.release_harq_set_1_resource());
  pucch_resource_manager::ue_reservation_guard guard3(
      &t_bench.res_manager, t_bench.slot_alloc, to_rnti(0x4603), ue_cfg_0());
  ASSERT_TRUE(guard3.release_harq_set_1_resource());
  pucch_resource_manager::ue_reservation_guard guard6(
      &t_bench.res_manager, t_bench.slot_alloc, to_rnti(0x4606), ue_cfg_0());
  ASSERT_TRUE(guard6.release_harq_set_1_resource());

  // Re-allocate the resources to UE1, UE3, UE6.
  const pucch_harq_resource_alloc_record realloc_ue1 = guard1.reserve_harq_set_1_resource_next_available();
  const pucch_harq_resource_alloc_record realloc_ue3 = guard3.reserve_harq_set_1_resource_next_available();
  const pucch_harq_resource_alloc_record realloc_ue6 = guard6.reserve_harq_set_1_resource_next_available();

  // Check whether the UEs get returned (again) the corresponding PUCCH resource indicator.
  ASSERT_EQ(0U, realloc_ue1.pucch_res_indicator);
  ASSERT_EQ(2U, realloc_ue3.pucch_res_indicator);
  ASSERT_EQ(5U, realloc_ue6.pucch_res_indicator);
}

TEST_F(test_pucch_resource_manager, test_allocation_sr_resource)
{
  pucch_resource_manager::ue_reservation_guard guard(
      &t_bench.res_manager, t_bench.slot_alloc, to_rnti(0x4601), ue_cfg_0());
  const pucch_resource* sr_resource = guard.reserve_sr_resource();

  const unsigned expected_sr_cell_res_id = pucch_cfg_0().sr_res_list[0].pucch_res_id.ded().cell_res_id;
  ASSERT_EQ(&t_bench.cell_res_list[expected_sr_cell_res_id], sr_resource);
}

TEST_F(test_pucch_resource_manager, test_allocation_release_sr_resource)
{
  pucch_resource_manager::ue_reservation_guard guard1(
      &t_bench.res_manager, t_bench.slot_alloc, to_rnti(0x4601), ue_cfg_0());
  guard1.reserve_sr_resource();

  // Release resource and verify it was successful.
  ASSERT_TRUE(guard1.release_sr_resource());

  // Allocate SR to another UE and verify it is assigned a resource.
  pucch_resource_manager::ue_reservation_guard guard2(
      &t_bench.res_manager, t_bench.slot_alloc, to_rnti(0x4602), ue_cfg_0());
  const pucch_resource* sr_resource_ue2 = guard2.reserve_sr_resource();
  ASSERT_FALSE(nullptr == sr_resource_ue2);
  const unsigned expected_sr_cell_res_id = pucch_cfg_0().sr_res_list[0].pucch_res_id.ded().cell_res_id;
  ASSERT_EQ(&t_bench.cell_res_list[expected_sr_cell_res_id], sr_resource_ue2);
}

TEST_F(test_pucch_resource_manager, test_allocation_2_sr_resource)
{
  pucch_resource_manager::ue_reservation_guard guard1(
      &t_bench.res_manager, t_bench.slot_alloc, to_rnti(0x4601), ue_cfg_0());
  ASSERT_NE(nullptr, guard1.reserve_sr_resource());
  pucch_resource_manager::ue_reservation_guard guard2(
      &t_bench.res_manager, t_bench.slot_alloc, to_rnti(0x4602), ue_cfg_1());
  ASSERT_NE(nullptr, guard2.reserve_sr_resource());

  // Attempt to allocate UEs over already used SR resources.
  pucch_resource_manager::ue_reservation_guard guard3(
      &t_bench.res_manager, t_bench.slot_alloc, to_rnti(0x4603), ue_cfg_0());
  ASSERT_EQ(nullptr, guard3.reserve_sr_resource());
  pucch_resource_manager::ue_reservation_guard guard4(
      &t_bench.res_manager, t_bench.slot_alloc, to_rnti(0x4604), ue_cfg_1());
  ASSERT_EQ(nullptr, guard4.reserve_sr_resource());

  // Release resource and verify it was successful.
  ASSERT_TRUE(guard1.release_sr_resource());
  ASSERT_TRUE(guard2.release_sr_resource());

  // Attempt to allocate UEs over released SR resources.
  ASSERT_NE(nullptr, guard3.reserve_sr_resource());
  ASSERT_NE(nullptr, guard4.reserve_sr_resource());
}

TEST_F(test_pucch_resource_manager, test_allocation_specific_f1)
{
  static constexpr unsigned res_indicator = 2;

  // Attempt to allocate PUCCH resource Format 2 with given resource indicator.
  pucch_resource_manager::ue_reservation_guard guard1(
      &t_bench.res_manager, t_bench.slot_alloc, to_rnti(0x4601), ue_cfg_0());
  ASSERT_TRUE(nullptr != guard1.reserve_harq_set_0_resource_by_res_indicator(res_indicator));

  // Attempt to allocate another UE to the same resource and verify it gets returned nullptr.
  pucch_resource_manager::ue_reservation_guard guard2(
      &t_bench.res_manager, t_bench.slot_alloc, to_rnti(0x4602), ue_cfg_0());
  ASSERT_TRUE(nullptr == guard2.reserve_harq_set_0_resource_by_res_indicator(res_indicator));

  // Attempt to allocate a third UE with wrong resource indicator and verify it gets returned nullptr.
  pucch_resource_manager::ue_reservation_guard guard3(
      &t_bench.res_manager, t_bench.slot_alloc, to_rnti(0x4603), ue_cfg_0());
  ASSERT_TRUE(nullptr == guard3.reserve_harq_set_0_resource_by_res_indicator(res_indicator));
}

TEST_F(test_pucch_resource_manager, test_allocation_specific_f2)
{
  static constexpr unsigned res_indicator = 3;

  // Attempt to allocate PUCCH resource Format 2 with given resource indicator.
  pucch_resource_manager::ue_reservation_guard guard1(
      &t_bench.res_manager, t_bench.slot_alloc, to_rnti(0x4601), ue_cfg_0());
  ASSERT_TRUE(nullptr != guard1.reserve_harq_set_1_resource_by_res_indicator(res_indicator));

  // Attempt to allocate another UE to the same resource and verify it gets returned nullptr.
  pucch_resource_manager::ue_reservation_guard guard2(
      &t_bench.res_manager, t_bench.slot_alloc, to_rnti(0x4602), ue_cfg_0());
  ASSERT_TRUE(nullptr == guard2.reserve_harq_set_1_resource_by_res_indicator(res_indicator));

  // Attempt to allocate a third UE with wrong resource indicator and verify it gets returned nullptr.
  pucch_resource_manager::ue_reservation_guard guard3(
      &t_bench.res_manager, t_bench.slot_alloc, to_rnti(0x4603), ue_cfg_0());
  ASSERT_TRUE(nullptr == guard3.reserve_harq_set_1_resource_by_res_indicator(res_indicator));
}

TEST_F(test_pucch_resource_manager, test_cancel_last_ue_res_reservations_for_specific_resources)
{
  pucch_resource_manager::ue_reservation_guard guard1(
      &t_bench.res_manager, t_bench.slot_alloc, to_rnti(0x4601), ue_cfg_0());
  auto alloc = guard1.reserve_harq_set_0_resource_next_available();
  ASSERT_EQ(0U, alloc.pucch_res_indicator);

  // Try to allocate a new UE using the PUCCH resource indicator assigned to UE 0x4601; it should fail.
  pucch_resource_manager::ue_reservation_guard guard2(
      &t_bench.res_manager, t_bench.slot_alloc, to_rnti(0x4602), ue_cfg_0());
  ASSERT_EQ(nullptr, guard2.reserve_harq_set_0_resource_by_res_indicator(alloc.pucch_res_indicator));

  // Release the resources of the first UE.
  guard1.rollback();

  // Try to allocate the new UE again, now it should succeed.
  ASSERT_NE(nullptr, guard2.reserve_harq_set_0_resource_by_res_indicator(alloc.pucch_res_indicator));

  alloc = guard1.reserve_harq_set_1_resource_next_available();

  // Try to allocate a new UE using the PUCCH resource indicator assigned to UE 0x4601; it should fail.
  ASSERT_EQ(nullptr, guard2.reserve_harq_set_1_resource_by_res_indicator(alloc.pucch_res_indicator));

  // Release the resources of the first UE.
  guard1.rollback();

  // Try to allocate the new UE again, now it should succeed.
  ASSERT_NE(nullptr, guard2.reserve_harq_set_1_resource_by_res_indicator(alloc.pucch_res_indicator));
}

TEST_F(test_pucch_resource_manager, test_cancel_last_ue_res_reservations_for_harq_resources_different_sets)
{
  pucch_resource_manager::ue_reservation_guard guard1_set0(
      &t_bench.res_manager, t_bench.slot_alloc, to_rnti(0x4601), ue_cfg_0());
  auto alloc = guard1_set0.reserve_harq_set_0_resource_next_available();
  ASSERT_EQ(0U, alloc.pucch_res_indicator);

  // Don't set this reservation in the tracker, as we want to preserve it.
  pucch_resource_manager::ue_reservation_guard guard1_set1(
      &t_bench.res_manager, t_bench.slot_alloc, to_rnti(0x4601), ue_cfg_0());
  alloc = guard1_set1.reserve_harq_set_1_resource_next_available();
  ASSERT_EQ(0U, alloc.pucch_res_indicator);

  // Try to allocate a new UE using the PUCCH resource indicator assigned to UE 0x4601; it should fail.
  pucch_resource_manager::ue_reservation_guard guard2(
      &t_bench.res_manager, t_bench.slot_alloc, to_rnti(0x4602), ue_cfg_0());
  ASSERT_EQ(nullptr, guard2.reserve_harq_set_0_resource_by_res_indicator(alloc.pucch_res_indicator));

  // Release the resources of the first UE.
  guard1_set0.rollback();

  // Try to allocate the new UE again, it should succeed.
  ASSERT_NE(nullptr, guard2.reserve_harq_set_0_resource_by_res_indicator(alloc.pucch_res_indicator));
}

TEST_F(test_pucch_resource_manager, test_cancel_last_ue_res_reservations_for_harq_and_csi)
{
  pucch_resource_manager::ue_reservation_guard guard1_harq(
      &t_bench.res_manager, t_bench.slot_alloc, to_rnti(0x4601), ue_cfg_0());
  auto alloc = guard1_harq.reserve_harq_set_1_resource_next_available();
  ASSERT_EQ(0U, alloc.pucch_res_indicator);

  pucch_resource_manager::ue_reservation_guard guard1_csi(
      &t_bench.res_manager, t_bench.slot_alloc, to_rnti(0x4601), ue_cfg_0());
  ASSERT_NE(nullptr, guard1_csi.reserve_csi_resource());

  pucch_resource_manager::ue_reservation_guard guard2(
      &t_bench.res_manager, t_bench.slot_alloc, to_rnti(0x4602), ue_cfg_0());
  ASSERT_EQ(nullptr, guard2.reserve_csi_resource());

  // Release the tracked resources of the first UE.
  guard1_csi.rollback();

  ASSERT_NE(nullptr, guard2.reserve_csi_resource());
}

TEST_F(test_pucch_resource_manager, test_cancel_last_ue_res_reservations_for_harq_and_sr)
{
  pucch_resource_manager::ue_reservation_guard guard1_harq(
      &t_bench.res_manager, t_bench.slot_alloc, to_rnti(0x4601), ue_cfg_0());
  auto alloc = guard1_harq.reserve_harq_set_0_resource_next_available();
  ASSERT_EQ(0U, alloc.pucch_res_indicator);

  pucch_resource_manager::ue_reservation_guard guard1_sr(
      &t_bench.res_manager, t_bench.slot_alloc, to_rnti(0x4601), ue_cfg_0());
  ASSERT_NE(nullptr, guard1_sr.reserve_sr_resource());

  pucch_resource_manager::ue_reservation_guard guard2(
      &t_bench.res_manager, t_bench.slot_alloc, to_rnti(0x4602), ue_cfg_0());
  ASSERT_EQ(nullptr, guard2.reserve_sr_resource());

  // Release the tracked resources of the first UE.
  guard1_sr.rollback();

  ASSERT_NE(nullptr, guard2.reserve_sr_resource());
}

////////////    Test the PUCCH resource manager: UEs with different configs     ////////////

class test_pucch_res_manager_multiple_cfg : public ::testing::Test
{
public:
  test_pucch_res_manager_multiple_cfg() = default;

protected:
  std::optional<res_manager_test_bench> t_bench;

  void create_ues(unsigned nof_ues, unsigned nof_res_per_ue_, unsigned nof_configurations)
  {
    t_bench.emplace(pucch_resource_builder_params{
        .res_set_size             = nof_res_per_ue_,
        .nof_cell_res_set_configs = nof_configurations,
        .nof_cell_sr_resources    = nof_configurations,
        .nof_cell_csi_resources   = nof_configurations,
    });

    for (unsigned ue_idx = 0; ue_idx != nof_ues; ++ue_idx) {
      const unsigned cfg_idx = ue_idx % nof_configurations;
      auto           ue_req  = cfg_idx != 0
                                   ? t_bench->make_ue_creation_req(pucch_sr_resource_id(cfg_idx), pucch_csi_resource_id(cfg_idx))
                                   : t_bench->make_ue_creation_req();

      t_bench->ue_ded_cfgs.push_back(t_bench->cfg_mng.add_ue(ue_req));
      t_bench->ues.add_ue(
          *t_bench->ue_ded_cfgs.back(), ue_req.starts_in_fallback, ue_req.ul_ccch_slot_rx, ue_req.cfra_enabled);
    }
  }

  void allocate_f1_specific_ue(unsigned ue_idx)
  {
    pucch_resource_manager::ue_reservation_guard guard(&t_bench->res_manager,
                                                       t_bench->slot_alloc,
                                                       t_bench->ue_ded_cfgs[ue_idx]->pcell_cfg().crnti,
                                                       t_bench->ue_ded_cfgs[ue_idx]->pcell_cfg());
    guard.reserve_harq_set_0_resource_next_available();
    guard.commit();
  }

  void allocate_f2_specific_ue(unsigned ue_idx)
  {
    pucch_resource_manager::ue_reservation_guard guard(&t_bench->res_manager,
                                                       t_bench->slot_alloc,
                                                       t_bench->ue_ded_cfgs[ue_idx]->pcell_cfg().crnti,
                                                       t_bench->ue_ded_cfgs[ue_idx]->pcell_cfg());
    guard.reserve_harq_set_1_resource_next_available();
    guard.commit();
  }
};

TEST_F(test_pucch_res_manager_multiple_cfg, test_2_ues_2_cfgs_alloc_f1_only)
{
  create_ues(4, 8, 2);

  // UE 0 and 1 will get assigned the same pucch_res_indicator, as they use different PUCCH configs.
  const auto&                                  ue_cfg_0 = t_bench->ue_ded_cfgs[0]->pcell_cfg();
  pucch_resource_manager::ue_reservation_guard guard0(
      &t_bench->res_manager, t_bench->slot_alloc, ue_cfg_0.crnti, ue_cfg_0);
  const pucch_harq_resource_alloc_record record_ue_0 = guard0.reserve_harq_set_0_resource_next_available();

  ASSERT_EQ(0U, record_ue_0.pucch_res_indicator);
  ASSERT_EQ(&t_bench->cell_res_list[0], record_ue_0.resource);
  ASSERT_EQ(0U, record_ue_0.resource->res_id.ded().cell_res_id);

  const auto&                                  ue_cfg_1 = t_bench->ue_ded_cfgs[1]->pcell_cfg();
  pucch_resource_manager::ue_reservation_guard guard1(
      &t_bench->res_manager, t_bench->slot_alloc, ue_cfg_1.crnti, ue_cfg_1);
  const pucch_harq_resource_alloc_record record_ue_1 = guard1.reserve_harq_set_0_resource_next_available();

  ASSERT_EQ(0U, record_ue_1.pucch_res_indicator);
  ASSERT_EQ(&t_bench->cell_res_list[8], record_ue_1.resource);
  ASSERT_EQ(8U, record_ue_1.resource->res_id.ded().cell_res_id);

  // UE 2 and 3 will get assigned the different pucch_res_indicator from UE 0 and 1, as they share the PUCCH configs.
  const auto&                                  ue_cfg_2 = t_bench->ue_ded_cfgs[2]->pcell_cfg();
  pucch_resource_manager::ue_reservation_guard guard2(
      &t_bench->res_manager, t_bench->slot_alloc, ue_cfg_2.crnti, ue_cfg_2);
  const pucch_harq_resource_alloc_record record_ue_2 = guard2.reserve_harq_set_0_resource_next_available();

  ASSERT_EQ(1U, record_ue_2.pucch_res_indicator);
  ASSERT_EQ(&t_bench->cell_res_list[1], record_ue_2.resource);
  ASSERT_EQ(1U, record_ue_2.resource->res_id.ded().cell_res_id);

  const auto&                                  ue_cfg_3 = t_bench->ue_ded_cfgs[3]->pcell_cfg();
  pucch_resource_manager::ue_reservation_guard guard3(
      &t_bench->res_manager, t_bench->slot_alloc, ue_cfg_3.crnti, ue_cfg_3);
  const pucch_harq_resource_alloc_record record_ue_3 = guard3.reserve_harq_set_0_resource_next_available();

  ASSERT_EQ(1U, record_ue_3.pucch_res_indicator);
  ASSERT_EQ(&t_bench->cell_res_list[9], record_ue_3.resource);
  ASSERT_EQ(9U, record_ue_3.resource->res_id.ded().cell_res_id);
}

TEST_F(test_pucch_res_manager_multiple_cfg, test_2_ues_2_cfgs_alloc_f2_only)
{
  create_ues(4, 8, 2);

  // UE 0 and 1 will get assigned the same pucch_res_indicator, as they use different PUCCH configs.
  const auto&                                  ue_cfg_0 = t_bench->ue_ded_cfgs[0]->pcell_cfg();
  pucch_resource_manager::ue_reservation_guard guard0(
      &t_bench->res_manager, t_bench->slot_alloc, ue_cfg_0.crnti, ue_cfg_0);
  const pucch_harq_resource_alloc_record record_ue_0 = guard0.reserve_harq_set_1_resource_next_available();

  ASSERT_EQ(0U, record_ue_0.pucch_res_indicator);
  ASSERT_EQ(&t_bench->cell_res_list[18], record_ue_0.resource);
  ASSERT_EQ(18U, record_ue_0.resource->res_id.ded().cell_res_id);

  const auto&                                  ue_cfg_1 = t_bench->ue_ded_cfgs[1]->pcell_cfg();
  pucch_resource_manager::ue_reservation_guard guard1(
      &t_bench->res_manager, t_bench->slot_alloc, ue_cfg_1.crnti, ue_cfg_1);
  const pucch_harq_resource_alloc_record record_ue_1 = guard1.reserve_harq_set_1_resource_next_available();

  ASSERT_EQ(0U, record_ue_1.pucch_res_indicator);
  ASSERT_EQ(&t_bench->cell_res_list[26], record_ue_1.resource);
  ASSERT_EQ(26U, record_ue_1.resource->res_id.ded().cell_res_id);

  // UE 2 and 3 will get assigned the different pucch_res_indicator from UE 0 and 1, as they share the PUCCH configs.
  const auto&                                  ue_cfg_2 = t_bench->ue_ded_cfgs[2]->pcell_cfg();
  pucch_resource_manager::ue_reservation_guard guard2(
      &t_bench->res_manager, t_bench->slot_alloc, ue_cfg_2.crnti, ue_cfg_2);
  const pucch_harq_resource_alloc_record record_ue_2 = guard2.reserve_harq_set_1_resource_next_available();

  ASSERT_EQ(1, record_ue_2.pucch_res_indicator);
  ASSERT_EQ(&t_bench->cell_res_list[19], record_ue_2.resource);
  ASSERT_EQ(19U, record_ue_2.resource->res_id.ded().cell_res_id);

  const auto&                                  ue_cfg_3 = t_bench->ue_ded_cfgs[3]->pcell_cfg();
  pucch_resource_manager::ue_reservation_guard guard3(
      &t_bench->res_manager, t_bench->slot_alloc, ue_cfg_3.crnti, ue_cfg_3);
  const pucch_harq_resource_alloc_record record_ue_3 = guard3.reserve_harq_set_1_resource_next_available();

  ASSERT_EQ(1U, record_ue_3.pucch_res_indicator);
  ASSERT_EQ(&t_bench->cell_res_list[27], record_ue_3.resource);
  ASSERT_EQ(27U, record_ue_3.resource->res_id.ded().cell_res_id);
}

TEST_F(test_pucch_res_manager_multiple_cfg, test_2_ues_2_cfgs_alloc_fetch_release)
{
  create_ues(4, 8, 2);

  // UE 0 and 1 will get assigned the same pucch_res_indicator, as they use different PUCCH configs.
  const auto&                                  ue_cfg_0 = t_bench->ue_ded_cfgs[0]->pcell_cfg();
  pucch_resource_manager::ue_reservation_guard guard0(
      &t_bench->res_manager, t_bench->slot_alloc, ue_cfg_0.crnti, ue_cfg_0);
  const pucch_harq_resource_alloc_record record_ue_0 = guard0.reserve_harq_set_0_resource_next_available();

  ASSERT_EQ(0U, record_ue_0.pucch_res_indicator);
  ASSERT_EQ(&t_bench->cell_res_list[0], record_ue_0.resource);
  ASSERT_EQ(0U, record_ue_0.resource->res_id.ded().cell_res_id);

  const auto&                                  ue_cfg_1 = t_bench->ue_ded_cfgs[1]->pcell_cfg();
  pucch_resource_manager::ue_reservation_guard guard1(
      &t_bench->res_manager, t_bench->slot_alloc, ue_cfg_1.crnti, ue_cfg_1);
  const pucch_harq_resource_alloc_record record_ue_1 = guard1.reserve_harq_set_0_resource_next_available();

  ASSERT_EQ(0U, record_ue_1.pucch_res_indicator);
  ASSERT_EQ(&t_bench->cell_res_list[8], record_ue_1.resource);
  ASSERT_EQ(8U, record_ue_1.resource->res_id.ded().cell_res_id);

  ASSERT_TRUE(guard0.release_harq_set_0_resource());
  ASSERT_TRUE(guard1.release_harq_set_0_resource());
}

TEST_F(test_pucch_res_manager_multiple_cfg, test_2_ues_2_cfgs_alloc_fetch_release_f2)
{
  create_ues(4, 8, 2);

  // UE 0 and 1 will get assigned the same pucch_res_indicator, as they use different PUCCH configs.
  const auto&                                  ue_cfg_0 = t_bench->ue_ded_cfgs[0]->pcell_cfg();
  pucch_resource_manager::ue_reservation_guard guard0(
      &t_bench->res_manager, t_bench->slot_alloc, ue_cfg_0.crnti, ue_cfg_0);
  const pucch_harq_resource_alloc_record record_ue_0 = guard0.reserve_harq_set_1_resource_next_available();

  ASSERT_EQ(0U, record_ue_0.pucch_res_indicator);
  ASSERT_EQ(&t_bench->cell_res_list[18], record_ue_0.resource);
  ASSERT_EQ(18U, record_ue_0.resource->res_id.ded().cell_res_id);

  const auto&                                  ue_cfg_1 = t_bench->ue_ded_cfgs[1]->pcell_cfg();
  pucch_resource_manager::ue_reservation_guard guard1(
      &t_bench->res_manager, t_bench->slot_alloc, ue_cfg_1.crnti, ue_cfg_1);
  const pucch_harq_resource_alloc_record record_ue_1 = guard1.reserve_harq_set_1_resource_next_available();

  ASSERT_EQ(0U, record_ue_1.pucch_res_indicator);
  ASSERT_EQ(&t_bench->cell_res_list[26], record_ue_1.resource);
  ASSERT_EQ(26U, record_ue_1.resource->res_id.ded().cell_res_id);

  // Release the resource and verify the UE does not hold it anymore.
  ASSERT_TRUE(guard0.release_harq_set_1_resource());

  // Release the resource and verify the UE does not hold it anymore.
  ASSERT_TRUE(guard1.release_harq_set_1_resource());
}

TEST_F(test_pucch_res_manager_multiple_cfg, test_2_ues_2_cfgs_alloc_specific_f2)
{
  create_ues(4, 8, 2);

  // UE 0 and 1 will get assigned the same pucch_res_indicator, as they use different PUCCH configs.
  const auto&                                  ue_cfg_0 = t_bench->ue_ded_cfgs[0]->pcell_cfg();
  pucch_resource_manager::ue_reservation_guard guard0(
      &t_bench->res_manager, t_bench->slot_alloc, ue_cfg_0.crnti, ue_cfg_0);
  const pucch_resource* res_ue_0 = guard0.reserve_harq_set_1_resource_by_res_indicator(5);

  ASSERT_EQ(&t_bench->cell_res_list[23], res_ue_0);
  ASSERT_EQ(23U, res_ue_0->res_id.ded().cell_res_id);

  const auto&                                  ue_cfg_1 = t_bench->ue_ded_cfgs[1]->pcell_cfg();
  pucch_resource_manager::ue_reservation_guard guard1(
      &t_bench->res_manager, t_bench->slot_alloc, ue_cfg_1.crnti, ue_cfg_1);
  const pucch_resource* res_ue_1 = guard1.reserve_harq_set_1_resource_by_res_indicator(5);

  ASSERT_EQ(&t_bench->cell_res_list[31], res_ue_1);
  ASSERT_EQ(31U, res_ue_1->res_id.ded().cell_res_id);

  // Try to allocate the same PUCCH resource (already reserved to UE 0 and 1) and check that the allocation fails.
  const auto&                                  ue_cfg_2 = t_bench->ue_ded_cfgs[2]->pcell_cfg();
  pucch_resource_manager::ue_reservation_guard guard2(
      &t_bench->res_manager, t_bench->slot_alloc, ue_cfg_2.crnti, ue_cfg_2);
  const pucch_resource* res_ue_2 = guard2.reserve_harq_set_1_resource_by_res_indicator(5);
  ASSERT_EQ(nullptr, res_ue_2);

  const auto&                                  ue_cfg_3 = t_bench->ue_ded_cfgs[3]->pcell_cfg();
  pucch_resource_manager::ue_reservation_guard guard3(
      &t_bench->res_manager, t_bench->slot_alloc, ue_cfg_3.crnti, ue_cfg_3);
  const pucch_resource* res_ue_3 = guard3.reserve_harq_set_1_resource_by_res_indicator(5);
  ASSERT_EQ(nullptr, res_ue_3);
}

TEST_F(test_pucch_res_manager_multiple_cfg, test_8_ues_2_cfgs_allocate_all_resources)
{
  create_ues(8, 3, 2);

  // Allocate the first 3 UEs (sharing the same config) to occupy all available resources.
  allocate_f1_specific_ue(0);
  allocate_f1_specific_ue(2);
  allocate_f1_specific_ue(4);

  const auto&                                  ue_cfg_6 = t_bench->ue_ded_cfgs[6]->pcell_cfg();
  pucch_resource_manager::ue_reservation_guard guard6(
      &t_bench->res_manager, t_bench->slot_alloc, ue_cfg_6.crnti, ue_cfg_6);
  pucch_harq_resource_alloc_record record_ue_6 = guard6.reserve_harq_set_0_resource_next_available();
  ASSERT_EQ(nullptr, record_ue_6.resource);

  allocate_f1_specific_ue(1);
  allocate_f1_specific_ue(3);
  allocate_f1_specific_ue(5);

  const auto&                                  ue_cfg_7 = t_bench->ue_ded_cfgs[7]->pcell_cfg();
  pucch_resource_manager::ue_reservation_guard guard7(
      &t_bench->res_manager, t_bench->slot_alloc, ue_cfg_7.crnti, ue_cfg_7);
  pucch_harq_resource_alloc_record record_ue_7 = guard7.reserve_harq_set_0_resource_next_available();
  ASSERT_EQ(nullptr, record_ue_7.resource);

  // Release one resource and check the next allocation is successful.
  const auto&                                  ue_cfg_2 = t_bench->ue_ded_cfgs[2]->pcell_cfg();
  pucch_resource_manager::ue_reservation_guard guard2(
      &t_bench->res_manager, t_bench->slot_alloc, ue_cfg_2.crnti, ue_cfg_2);
  ASSERT_TRUE(guard2.release_harq_set_0_resource());

  record_ue_6 = guard6.reserve_harq_set_0_resource_next_available();
  ASSERT_EQ(1U, record_ue_6.pucch_res_indicator);
  ASSERT_EQ(&t_bench->cell_res_list[1], record_ue_6.resource);
  ASSERT_EQ(1U, record_ue_6.resource->res_id.ded().cell_res_id);

  const auto&                                  ue_cfg_3 = t_bench->ue_ded_cfgs[3]->pcell_cfg();
  pucch_resource_manager::ue_reservation_guard guard3(
      &t_bench->res_manager, t_bench->slot_alloc, ue_cfg_3.crnti, ue_cfg_3);
  ASSERT_TRUE(guard3.release_harq_set_0_resource());

  record_ue_7 = guard7.reserve_harq_set_0_resource_next_available();
  ASSERT_EQ(1U, record_ue_7.pucch_res_indicator);
  ASSERT_EQ(&t_bench->cell_res_list[4], record_ue_7.resource);
  ASSERT_EQ(4U, record_ue_7.resource->res_id.ded().cell_res_id);
}

TEST_F(test_pucch_res_manager_multiple_cfg, test_8_ues_2_cfgs_allocate_all_resources_f2)
{
  create_ues(8, 3, 2);

  // Allocate the first 3 UEs (sharing the same config) to occupy all available resources.
  allocate_f2_specific_ue(0);
  allocate_f2_specific_ue(2);
  allocate_f2_specific_ue(4);

  const auto&                                  ue_cfg_6 = t_bench->ue_ded_cfgs[6]->pcell_cfg();
  pucch_resource_manager::ue_reservation_guard guard6(
      &t_bench->res_manager, t_bench->slot_alloc, ue_cfg_6.crnti, ue_cfg_6);
  pucch_harq_resource_alloc_record record_ue_6 = guard6.reserve_harq_set_1_resource_next_available();
  ASSERT_EQ(nullptr, record_ue_6.resource);

  allocate_f2_specific_ue(1);
  allocate_f2_specific_ue(3);
  allocate_f2_specific_ue(5);

  const auto&                                  ue_cfg_7 = t_bench->ue_ded_cfgs[7]->pcell_cfg();
  pucch_resource_manager::ue_reservation_guard guard7(
      &t_bench->res_manager, t_bench->slot_alloc, ue_cfg_7.crnti, ue_cfg_7);
  pucch_harq_resource_alloc_record record_ue_7 = guard7.reserve_harq_set_1_resource_next_available();
  ASSERT_EQ(nullptr, record_ue_7.resource);

  // Release one resource and check the next allocation is successful.
  const auto&                                  ue_cfg_2 = t_bench->ue_ded_cfgs[2]->pcell_cfg();
  pucch_resource_manager::ue_reservation_guard guard2(
      &t_bench->res_manager, t_bench->slot_alloc, ue_cfg_2.crnti, ue_cfg_2);
  ASSERT_TRUE(guard2.release_harq_set_1_resource());

  record_ue_6 = guard6.reserve_harq_set_1_resource_next_available();
  ASSERT_EQ(1, record_ue_6.pucch_res_indicator);
  ASSERT_EQ(&t_bench->cell_res_list[9], record_ue_6.resource);
  ASSERT_EQ(9U, record_ue_6.resource->res_id.ded().cell_res_id);

  const auto&                                  ue_cfg_3 = t_bench->ue_ded_cfgs[3]->pcell_cfg();
  pucch_resource_manager::ue_reservation_guard guard3(
      &t_bench->res_manager, t_bench->slot_alloc, ue_cfg_3.crnti, ue_cfg_3);
  ASSERT_TRUE(guard3.release_harq_set_1_resource());

  record_ue_7 = guard7.reserve_harq_set_1_resource_next_available();
  ASSERT_EQ(1U, record_ue_7.pucch_res_indicator);
  ASSERT_EQ(&t_bench->cell_res_list[12], record_ue_7.resource);
  ASSERT_EQ(12U, record_ue_7.resource->res_id.ded().cell_res_id);
}

TEST_F(test_pucch_res_manager_multiple_cfg, test_4_ues_2_cfgs_allocate_sr)
{
  create_ues(4, 8, 2);

  // Allocate SR for UE 0.
  const auto&                                  ue_cfg_0 = t_bench->ue_ded_cfgs[0]->pcell_cfg();
  pucch_resource_manager::ue_reservation_guard guard0(
      &t_bench->res_manager, t_bench->slot_alloc, ue_cfg_0.crnti, ue_cfg_0);
  const pucch_resource* sr_resource = guard0.reserve_sr_resource();
  ASSERT_EQ(&t_bench->cell_res_list[16], sr_resource);

  const auto&                                  ue_cfg_2 = t_bench->ue_ded_cfgs[2]->pcell_cfg();
  pucch_resource_manager::ue_reservation_guard guard2(
      &t_bench->res_manager, t_bench->slot_alloc, ue_cfg_2.crnti, ue_cfg_2);
  ASSERT_EQ(nullptr, guard2.reserve_sr_resource());
  ASSERT_EQ(16U, sr_resource->res_id.ded().cell_res_id);

  // Release resource and verify it was successful.
  ASSERT_TRUE(guard0.release_sr_resource());

  // Allocate SR to another UE and verify it is assigned a resource.
  const pucch_resource* sr_resource_ue2 = guard2.reserve_sr_resource();
  ASSERT_EQ(&t_bench->cell_res_list[16], sr_resource_ue2);
  ASSERT_EQ(16U, sr_resource_ue2->res_id.ded().cell_res_id);

  const auto&                                  ue_cfg_1 = t_bench->ue_ded_cfgs[1]->pcell_cfg();
  pucch_resource_manager::ue_reservation_guard guard1(
      &t_bench->res_manager, t_bench->slot_alloc, ue_cfg_1.crnti, ue_cfg_1);
  const pucch_resource* sr_resource_ue1 = guard1.reserve_sr_resource();
  ASSERT_EQ(&t_bench->cell_res_list[17], sr_resource_ue1);
  ASSERT_EQ(sr_resource_ue1, guard1.reserve_sr_resource());
  ASSERT_EQ(17U, sr_resource_ue1->res_id.ded().cell_res_id);

  // Release resource and verify it was successful.
  ASSERT_TRUE(guard1.release_sr_resource());

  // Allocate SR to another UE and verify it is assigned a resource.
  const auto&                                  ue_cfg_3 = t_bench->ue_ded_cfgs[3]->pcell_cfg();
  pucch_resource_manager::ue_reservation_guard guard3(
      &t_bench->res_manager, t_bench->slot_alloc, ue_cfg_3.crnti, ue_cfg_3);
  const pucch_resource* sr_resource_ue3 = guard3.reserve_sr_resource();
  ASSERT_EQ(&t_bench->cell_res_list[17], sr_resource_ue3);
  ASSERT_EQ(17U, sr_resource_ue3->res_id.ded().cell_res_id);
}

TEST_F(test_pucch_res_manager_multiple_cfg, test_4_ues_2_cfgs_allocate_csi)
{
  create_ues(4, 8, 2);

  // Allocate CSI for UE 0.
  const auto&                                  ue_cfg_0 = t_bench->ue_ded_cfgs[0]->pcell_cfg();
  pucch_resource_manager::ue_reservation_guard guard0(
      &t_bench->res_manager, t_bench->slot_alloc, ue_cfg_0.crnti, ue_cfg_0);
  const pucch_resource* csi_resource = guard0.reserve_csi_resource();
  ASSERT_EQ(&t_bench->cell_res_list[34], csi_resource);
  ASSERT_EQ(34U, csi_resource->res_id.ded().cell_res_id);

  const auto&                                  ue_cfg_2 = t_bench->ue_ded_cfgs[2]->pcell_cfg();
  pucch_resource_manager::ue_reservation_guard guard2(
      &t_bench->res_manager, t_bench->slot_alloc, ue_cfg_2.crnti, ue_cfg_2);
  ASSERT_EQ(nullptr, guard2.reserve_csi_resource());

  // Release resource and verify it was successful.
  ASSERT_TRUE(guard0.release_csi_resource());

  // Allocate CSI to another UE and verify it is assigned a resource.
  const pucch_resource* csi_resource_ue2 = guard2.reserve_csi_resource();
  ASSERT_EQ(&t_bench->cell_res_list[34], csi_resource_ue2);
  ASSERT_EQ(34U, csi_resource_ue2->res_id.ded().cell_res_id);

  const auto&                                  ue_cfg_1 = t_bench->ue_ded_cfgs[1]->pcell_cfg();
  pucch_resource_manager::ue_reservation_guard guard1(
      &t_bench->res_manager, t_bench->slot_alloc, ue_cfg_1.crnti, ue_cfg_1);
  const pucch_resource* csi_resource_ue1 = guard1.reserve_csi_resource();
  ASSERT_EQ(&t_bench->cell_res_list[35], csi_resource_ue1);
  ASSERT_EQ(35U, csi_resource_ue1->res_id.ded().cell_res_id);

  const auto&                                  ue_cfg_3 = t_bench->ue_ded_cfgs[3]->pcell_cfg();
  pucch_resource_manager::ue_reservation_guard guard3(
      &t_bench->res_manager, t_bench->slot_alloc, ue_cfg_3.crnti, ue_cfg_3);
  ASSERT_EQ(nullptr, guard3.reserve_csi_resource());

  // Release resource and verify it was successful.
  ASSERT_TRUE(guard1.release_csi_resource());

  // Allocate CSI to another UE and verify it is assigned a resource.
  const pucch_resource* sr_resource_ue3 = guard3.reserve_csi_resource();
  ASSERT_EQ(&t_bench->cell_res_list[35], sr_resource_ue3);
  ASSERT_EQ(35U, sr_resource_ue3->res_id.ded().cell_res_id);
}
