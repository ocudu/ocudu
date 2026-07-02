// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/scheduler/cell/resource_grid.h"
#include "lib/scheduler/config/cell_configuration.h"
#include "lib/scheduler/config/du_cell_group_config_pool.h"
#include "lib/scheduler/pucch_scheduling/pucch_collision_manager.h"
#include "lib/scheduler/support/bwp_helpers.h"
#include "tests/test_doubles/scheduler/cell_config_builder_profiles.h"
#include "tests/test_doubles/scheduler/scheduler_config_helper.h"
#include "tests/unittests/scheduler/test_utils/scheduler_test_suite.h"
#include "ocudu/ran/pucch/pucch_constants.h"
#include "ocudu/ran/slot_point.h"
#include "ocudu/scheduler/config/pucch_default_resource.h"
#include "ocudu/scheduler/config/pucch_resource_builder_params.h"
#include "ocudu/scheduler/config/scheduler_expert_config_factory.h"
#include "ocudu/scheduler/resource_grid_util.h"
#include <gtest/gtest.h>
#include <memory>

using namespace ocudu;

static std::unique_ptr<du_cell_config_pool>
make_test_cell_config_pool(const pucch_resource_builder_params& ded_resources         = pucch_resource_builder_params{},
                           unsigned                             pucch_resource_common = 11,
                           const cell_config_builder_params&    cell_params           = {})
{
  const auto expert_cfg = config_helpers::make_default_scheduler_expert_config();
  auto       sched_req  = sched_config_helper::make_default_sched_cell_configuration_request(cell_params);
  sched_req.ran.init_bwp.pucch.resources                                                 = ded_resources;
  sched_req.ran.ul_cfg_common.init_ul_bwp.pucch_cfg_common.value().pucch_resource_common = pucch_resource_common;
  return std::make_unique<du_cell_config_pool>(expert_cfg, sched_req);
}

// Computes the grant_info for one hop of a pucch_resource against the given BWP.
static grant_info pucch_hop_grant(const pucch_resource& res, const bwp_configuration& bwp_cfg, bool first_hop)
{
  const unsigned half = res.syms.length() / 2;
  if (!res.second_hop_prb.has_value()) {
    return {bwp_cfg.scs, res.syms, prb_to_crb(bwp_cfg, res.prbs())};
  }
  if (first_hop) {
    return {bwp_cfg.scs, {res.syms.start(), res.syms.start() + half}, prb_to_crb(bwp_cfg, res.prbs())};
  }
  return {bwp_cfg.scs,
          {res.syms.start() + half, res.syms.stop()},
          prb_to_crb(bwp_cfg, prb_interval::start_and_len(*res.second_hop_prb, res.prbs().length()))};
}

static void check_rg_has_expected_grants(const cell_slot_resource_grid& ul_res_grid,
                                         const bwp_configuration&       bwp_cfg,
                                         span<const pucch_resource>     expected_resources)
{
  cell_slot_resource_grid expected_rg = ul_res_grid;
  expected_rg.clear();

  for (const auto& res : expected_resources) {
    expected_rg.fill(pucch_hop_grant(res, bwp_cfg, true));
    if (res.second_hop_prb.has_value()) {
      expected_rg.fill(pucch_hop_grant(res, bwp_cfg, false));
    }
  }

  ASSERT_EQ(ul_res_grid, expected_rg);
}

TEST(pucch_collision_manager_test, check_mux_regions_count_for_common_resources)
{
  auto expected_regions_from_number_of_cs = [](unsigned nof_cs) -> std::vector<unsigned> {
    switch (nof_cs) {
      case 2:
        return {2, 2, 2, 2, 2, 2, 2, 2};
      case 3:
        return {3, 3, 2, 3, 3, 2};
      case 4:
        return {4, 4, 4, 4};
      default:
        ocudu_assertion_failure("Unexpected number of cyclic shifts for common PUCCH resources");
        return {};
    }
  };

  for (unsigned pucch_resource_common = 0; pucch_resource_common != 16; ++pucch_resource_common) {
    auto cfg_pool = make_test_cell_config_pool(
        pucch_resource_builder_params{
            .f0_or_f1_params =
                pucch_f1_params{
                    .nof_cyc_shifts = pucch_nof_cyclic_shifts::no_cyclic_shift,
                    .occ_supported  = false,
                },
        },
        pucch_resource_common);
    const auto& cell_cfg = cfg_pool->cell_cfg();

    pucch_collision_manager col_manager(cell_cfg);
    const auto              mux_matrix = detail::make_mux_regions_matrix(cell_cfg.bwp_res[to_bwp_id(0)].ul().pucch);

    const auto& default_res           = get_pucch_default_resource(pucch_resource_common, cell_cfg.nof_ul_prbs);
    const auto  expected_region_sizes = expected_regions_from_number_of_cs(default_res.cs_indexes.size());

    unsigned r_pucch = 0;
    ASSERT_EQ(expected_region_sizes.size(), mux_matrix.size());
    for (unsigned i = 0; i != mux_matrix.size(); ++i) {
      ASSERT_EQ(expected_region_sizes[i], mux_matrix[i].count());

      for (unsigned j = 0; j != expected_region_sizes[i]; ++j) {
        ASSERT_TRUE(mux_matrix[i].test(r_pucch + j));
      }

      r_pucch += expected_region_sizes[i];
    }
  }
}

TEST(pucch_collision_manager_test, handles_max_dedicated_resources_with_mux_regions)
{
  auto        cfg_pool   = make_test_cell_config_pool(pucch_resource_builder_params{
               .res_set_size             = 8,
               .nof_cell_res_set_configs = 8,
               .nof_cell_sr_resources    = 96,
               .nof_cell_csi_resources   = 32,
               .f0_or_f1_params =
          pucch_f1_params{
                       .nof_syms       = pucch_constants::f1::MIN_NOF_SYMS,
                       .nof_cyc_shifts = pucch_nof_cyclic_shifts::twelve,
                       .occ_supported  = true,
          },
               .f2_or_f3_or_f4_params = pucch_f2_params{.nof_syms = 1, .max_nof_rbs = 1},
  });
  const auto& cell_cfg   = cfg_pool->cell_cfg();
  const auto  mux_matrix = detail::make_mux_regions_matrix(cell_cfg.bwp_res[to_bwp_id(0)].ul().pucch);
  ASSERT_GT(mux_matrix.size(), 8U);
}

TEST(pucch_collision_manager_test, alloc_fails_if_slot_incompatible_with_resource_symbols)
{
  // DDDSU pattern: period of 5 slots, slot index 3 is the special slot, with only the last 2 symbols enabled for UL.
  cell_config_builder_params cell_params = cell_config_builder_profiles::create(duplex_mode::TDD);
  cell_params.tdd_ul_dl_cfg_common       = cell_config_builder_profiles::create_tdd_pattern(
      cell_config_builder_profiles::tdd_pattern_profile_fr1_30khz::DDDSU);

  auto        cfg_pool = make_test_cell_config_pool(pucch_resource_builder_params{}, 11, cell_params);
  const auto& cell_cfg = cfg_pool->cell_cfg();
  const auto& ded_res  = cell_cfg.bwp_res[to_bwp_id(0)].ul().pucch.dedicated;

  pucch_collision_manager      col_manager(cell_cfg);
  cell_slot_resource_allocator slot_alloc(cell_cfg);

  const slot_point special_slot{subcarrier_spacing::kHz30, 3};
  col_manager.slot_indication(special_slot);
  slot_alloc.slot_indication(special_slot);

  // The F1 resources span 14 symbols (the whole slot), which does not fit within the special slot's 2 UL symbols.
  static constexpr rnti_t rnti = to_rnti(0x4601);
  auto                    res  = col_manager.alloc(slot_alloc, ded_res[0], rnti);
  ASSERT_FALSE(res.has_value());
  ASSERT_EQ(pucch_alloc_failure::INCOMPATIBLE_SLOT, res.error());
}

class pucch_collision_manager_rg_test : public ::testing::Test
{
protected:
  pucch_collision_manager_rg_test() :
    cfg_pool(make_test_cell_config_pool()),
    cell_cfg(cfg_pool->cell_cfg()),
    bwp_cfg(cell_cfg.params.ul_cfg_common.init_ul_bwp.generic_params),
    common_res(cell_cfg.bwp_res[to_bwp_id(0)].ul().pucch.common),
    ded_res(cell_cfg.bwp_res[to_bwp_id(0)].ul().pucch.dedicated),
    col_manager(cell_cfg),
    slot_alloc(cell_cfg),
    sl(0, 0)
  {
    col_manager.slot_indication(sl);
    slot_alloc.slot_indication(sl);
  }

  void run_slot()
  {
    ++sl;
    col_manager.slot_indication(sl);
    slot_alloc.slot_indication(sl);
  }

  void check_rg(span<const pucch_resource> expected)
  {
    check_rg_has_expected_grants(slot_alloc.ul_res_grid, bwp_cfg, expected);
  }

  std::unique_ptr<du_cell_config_pool>                                                    cfg_pool;
  const cell_configuration&                                                               cell_cfg;
  const bwp_configuration&                                                                bwp_cfg;
  const std::array<pucch_resource, pucch_constants::MAX_NOF_CELL_COMMON_PUCCH_RESOURCES>& common_res;
  const std::vector<pucch_resource>&                                                      ded_res;

  pucch_collision_manager      col_manager;
  cell_slot_resource_allocator slot_alloc;
  slot_point                   sl;

  static constexpr rnti_t rnti = to_rnti(0x4601);
};

TEST_F(pucch_collision_manager_rg_test, alloc_fills_grants_in_ul_res_grid)
{
  for (unsigned r_pucch = 0; r_pucch != pucch_constants::MAX_NOF_CELL_COMMON_PUCCH_RESOURCES; ++r_pucch) {
    ASSERT_TRUE(col_manager.alloc(slot_alloc, common_res[r_pucch], rnti).has_value());
    check_rg(span<const pucch_resource>(common_res).first(r_pucch + 1));
  }

  run_slot();

  // Allocate all dedicated resources one by one.
  for (unsigned i = 0; i != ded_res.size(); ++i) {
    ASSERT_TRUE(col_manager.alloc(slot_alloc, ded_res[i], rnti).has_value());
    check_rg(span<const pucch_resource>(ded_res).first(i + 1));
  }
}

TEST_F(pucch_collision_manager_rg_test, alloc_fails_if_ul_res_grid_occupied)
{
  auto expect_ul_grant_collision = [&](grant_info grant, auto allocator) {
    slot_alloc.ul_res_grid.fill(grant);
    const auto expected_rg = slot_alloc.ul_res_grid;

    auto res = allocator();
    ASSERT_FALSE(res.has_value());
    ASSERT_EQ(pucch_alloc_failure::UL_GRANT_COLLISION, res.error());
    ASSERT_EQ(slot_alloc.ul_res_grid, expected_rg);

    slot_alloc.ul_res_grid.clear(grant);
  };

  for (unsigned r_pucch = 0; r_pucch != pucch_constants::MAX_NOF_CELL_COMMON_PUCCH_RESOURCES; ++r_pucch) {
    expect_ul_grant_collision(pucch_hop_grant(common_res[r_pucch], bwp_cfg, true),
                              [&]() { return col_manager.alloc(slot_alloc, common_res[r_pucch], rnti); });
    expect_ul_grant_collision(pucch_hop_grant(common_res[r_pucch], bwp_cfg, false),
                              [&]() { return col_manager.alloc(slot_alloc, common_res[r_pucch], rnti); });
  }

  for (const auto& res : ded_res) {
    // Simulate UL grant collision when allocating the dedicated PUCCH resource.
    expect_ul_grant_collision(pucch_hop_grant(res, bwp_cfg, true),
                              [&]() { return col_manager.alloc(slot_alloc, res, rnti); });
  }
}

TEST_F(pucch_collision_manager_rg_test, alloc_fails_if_resource_in_use)
{
  // Allocate the same resource twice.
  ASSERT_TRUE(col_manager.alloc(slot_alloc, ded_res[0], rnti).has_value());
  ASSERT_FALSE(col_manager.alloc(slot_alloc, ded_res[0], rnti).has_value());
  ASSERT_EQ(pucch_alloc_failure::RESOURCE_IN_USE, col_manager.alloc(slot_alloc, ded_res[0], rnti).error());
}

TEST_F(pucch_collision_manager_rg_test, alloc_fails_if_pucch_collision)
{
  // Note: Common Res 0 collides with the first dedicated resource as both start at the edges of the BWP.

  // First common, then dedicated.
  ASSERT_TRUE(col_manager.alloc(slot_alloc, common_res[0], rnti).has_value());
  ASSERT_FALSE(col_manager.alloc(slot_alloc, ded_res[0], rnti).has_value());
  ASSERT_EQ(pucch_alloc_failure::PUCCH_COLLISION, col_manager.alloc(slot_alloc, ded_res[0], rnti).error());

  run_slot();

  // First dedicated, then common.
  ASSERT_TRUE(col_manager.alloc(slot_alloc, ded_res[0], rnti).has_value());
  ASSERT_FALSE(col_manager.alloc(slot_alloc, common_res[0], rnti).has_value());
  ASSERT_EQ(pucch_alloc_failure::PUCCH_COLLISION, col_manager.alloc(slot_alloc, common_res[0], rnti).error());
}

TEST_F(pucch_collision_manager_rg_test, free_clears_grants_in_ul_res_grid)
{
  for (unsigned r_pucch = 0; r_pucch != pucch_constants::MAX_NOF_CELL_COMMON_PUCCH_RESOURCES; ++r_pucch) {
    ASSERT_TRUE(col_manager.alloc(slot_alloc, common_res[r_pucch], rnti).has_value());
    ASSERT_TRUE(col_manager.free(slot_alloc, common_res[r_pucch], rnti));
    check_rg(span<const pucch_resource>{});
  }

  // Allocate and free all dedicated resources one by one.
  for (const auto& res : ded_res) {
    ASSERT_TRUE(col_manager.alloc(slot_alloc, res, rnti).has_value());
    ASSERT_TRUE(col_manager.free(slot_alloc, res, rnti));
    check_rg(span<const pucch_resource>{});
  }
}

TEST_F(pucch_collision_manager_rg_test, free_doesnt_clear_grants_if_resource_is_being_muxed)
{
  // Note: Common Res 0 and 1 are multiplexed together for the tested configuration.
  ASSERT_TRUE(col_manager.alloc(slot_alloc, common_res[0], rnti).has_value());
  ASSERT_TRUE(col_manager.alloc(slot_alloc, common_res[1], rnti).has_value());

  // Note: the grants are the same for both resources.
  check_rg(span<const pucch_resource>(common_res).first(1));

  // Free the first resource. Grants should remain because the second resource is still allocated.
  ASSERT_TRUE(col_manager.free(slot_alloc, common_res[0], rnti));
  check_rg(span<const pucch_resource>(common_res).first(1));

  // Free the second resource. Grants should be cleared now.
  ASSERT_TRUE(col_manager.free(slot_alloc, common_res[1], rnti));
  check_rg(span<const pucch_resource>{});
}

TEST_F(pucch_collision_manager_rg_test, free_doesnt_clear_grants_if_resource_was_not_allocated)
{
  // Simulate a non-PUCCH grant over the dedicated resource grant.
  slot_alloc.ul_res_grid.fill(pucch_hop_grant(ded_res[0], bwp_cfg, true));
  if (ded_res[0].second_hop_prb.has_value()) {
    slot_alloc.ul_res_grid.fill(pucch_hop_grant(ded_res[0], bwp_cfg, false));
  }

  // Try to free the dedicated resource. It should return false and not clear the grant.
  ASSERT_FALSE(col_manager.free(slot_alloc, ded_res[0], rnti));

  // Check that the resource grid was not modified.
  check_rg(span<const pucch_resource>(ded_res).first(1));
}

TEST_F(pucch_collision_manager_rg_test, slot_indication_clears_pucch_res_grid)
{
  const unsigned ring_size = get_allocator_ring_size_gt_min(get_max_slot_ul_alloc_delay(0));
  for (unsigned i = 0; i != ring_size; ++i) {
    ASSERT_TRUE(col_manager.alloc(slot_alloc, ded_res[0], rnti).has_value());
    run_slot();
  }

  for (unsigned i = 0; i != ring_size; ++i) {
    slot_alloc.ul_res_grid.fill(pucch_hop_grant(ded_res[0], bwp_cfg, true));
    if (ded_res[0].second_hop_prb.has_value()) {
      slot_alloc.ul_res_grid.fill(pucch_hop_grant(ded_res[0], bwp_cfg, false));
    }

    auto res = col_manager.alloc(slot_alloc, ded_res[0], rnti);
    ASSERT_FALSE(res.has_value());
    ASSERT_EQ(pucch_alloc_failure::UL_GRANT_COLLISION, res.error());

    run_slot();
  }
}
