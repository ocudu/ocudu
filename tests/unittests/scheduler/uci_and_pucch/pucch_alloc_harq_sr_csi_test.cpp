// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "../test_utils/scheduler_test_suite.h"
#include "pucch_alloc_base_tester.h"
#include "uci_test_utils.h"
#include <gtest/gtest.h>

using namespace ocudu;

class pucch_alloc_multiple_rbs_test : public ::testing::Test, public pucch_allocator_base_test
{
public:
  pucch_alloc_multiple_rbs_test() :
    pucch_allocator_base_test(
        test_bench_params{.pucch_ded_params = {.f2_or_f3_or_f4_params = pucch_f2_params{.max_nof_rbs = 3U}}})
  {
    // This PUCCH grant will be for 5 HARQ bits, which fit in 1 PRB.
    pucch_expected_harq_only = test_helpers::make_pucch_info(
        t_bench.cell_cfg, t_bench.cell_cfg.bwp_res[to_bwp_id(0)].ul().pucch.dedicated[8], {.harq_ack_nof_bits = 5U});
    std::get<pucch_info::f2_config>(pucch_expected_harq_only.format_params).nof_prbs = 1;

    // This PUCCH grant will be for 4 CSI bits only, which are encoded in the maximum number of PRBs.
    pucch_expected_csi_only =
        test_helpers::make_pucch_info(t_bench.cell_cfg,
                                      t_bench.cell_cfg.bwp_res[to_bwp_id(0)].ul().pucch.dedicated[14],
                                      {.csi_part1_nof_bits = default_csi_part1_bits});
    std::get<pucch_info::f2_config>(pucch_expected_csi_only.format_params).nof_prbs = 3;

    // This PUCCH grant will be for 2 HARQ bits + 4 CSI bits, which fit in 1 PRB.
    pucch_expected_harq_csi                             = pucch_expected_harq_only;
    pucch_expected_harq_csi.uci_bits.harq_ack_nof_bits  = 2U;
    pucch_expected_harq_csi.uci_bits.csi_part1_nof_bits = default_csi_part1_bits;
  }

protected:
  // Parameters that are passed by the routine to run the tests.
  pucch_info pucch_expected_harq_only;
  pucch_info pucch_expected_harq_csi;
  pucch_info pucch_expected_csi_only;

  static constexpr unsigned default_csi_part1_bits = 4U;
};

///////   Test PUCCH Format 2 PRB configuration.    ///////

TEST_F(pucch_alloc_multiple_rbs_test, test_prb_allocation_csi_only)
{
  ASSERT_TRUE(alloc_csi_opportunity(t_bench.get_main_ue()));

  // Expect 1 PUCCH grant with CSI.
  ASSERT_EQ(1U, default_slot_grid.result.ul.pucchs.size());
  ASSERT_TRUE(find_pucch_pdu(default_slot_grid.result.ul.pucchs.unsorted(),
                             [&expected = pucch_expected_csi_only](const auto& pdu) {
                               return pucch_info_match(expected, pdu) and pdu.csi_rep_cfg.has_value();
                             }));
}

TEST_F(pucch_alloc_multiple_rbs_test, test_prb_allocation_csi_sr)
{
  pucch_expected_csi_only.uci_bits.sr_bits = sr_nof_bits::one;

  ASSERT_TRUE(alloc_sr_opportunity(t_bench.get_main_ue()));
  ASSERT_TRUE(alloc_csi_opportunity(t_bench.get_main_ue()));

  ASSERT_EQ(1U, default_slot_grid.result.ul.pucchs.size());
  ASSERT_TRUE(find_pucch_pdu(default_slot_grid.result.ul.pucchs.unsorted(),
                             [&expected = pucch_expected_csi_only](const auto& pdu) {
                               return pucch_info_match(expected, pdu) and pdu.csi_rep_cfg.has_value();
                             }));
}

TEST_F(pucch_alloc_multiple_rbs_test, test_prb_allocation_harq_only)
{
  for (unsigned i = 0; i != 5; ++i) {
    ASSERT_TRUE(alloc_ded_harq_ack(t_bench.get_main_ue()));
  }

  ASSERT_EQ(1U, default_slot_grid.result.ul.pucchs.size());
  ASSERT_TRUE(find_pucch_pdu(
      default_slot_grid.result.ul.pucchs.unsorted(),
      [&expected = pucch_expected_harq_only](const auto& pdu) { return pucch_info_match(expected, pdu); }));
}

TEST_F(pucch_alloc_multiple_rbs_test, test_prb_allocation_harq_csi_only)
{
  ASSERT_TRUE(alloc_csi_opportunity(t_bench.get_main_ue()));
  for (unsigned i = 0; i != 2; ++i) {
    ASSERT_TRUE(alloc_ded_harq_ack(t_bench.get_main_ue()));
  }

  ASSERT_TRUE(find_pucch_pdu(
      default_slot_grid.result.ul.pucchs.unsorted(),
      [&expected = pucch_expected_harq_csi](const auto& pdu) { return pucch_info_match(expected, pdu); }));
}

///////   Test UL grants reached and PUCCH fails.    ///////

class pucch_alloc_reach_ul_grant_limits_test : public ::testing::Test, public pucch_allocator_base_test
{
public:
  pucch_alloc_reach_ul_grant_limits_test() :
    pucch_allocator_base_test(
        test_bench_params{.pucch_res_common = 11, .n_cces = 1, .max_pucchs_per_slot = 3U, .max_ul_grants_per_slot = 6U})
  {
  }

protected:
  static constexpr unsigned default_csi_part1_bits = 4U;
};

TEST_F(pucch_alloc_reach_ul_grant_limits_test, test_max_pucch_allocation_reached)
{
  ASSERT_TRUE(alloc_csi_opportunity(t_bench.get_main_ue()));
  ASSERT_EQ(1U, default_slot_grid.result.ul.pucchs.size());

  t_bench.add_ue();
  ASSERT_TRUE(alloc_ded_harq_ack(t_bench.get_ue(t_bench.last_added_ue_idx)));
  ASSERT_EQ(2U, default_slot_grid.result.ul.pucchs.size());

  t_bench.add_ue();
  ASSERT_TRUE(alloc_ded_harq_ack(t_bench.get_ue(t_bench.last_added_ue_idx)));
  ASSERT_EQ(3U, default_slot_grid.result.ul.pucchs.size());

  t_bench.add_ue();
  ASSERT_FALSE(alloc_ded_harq_ack(t_bench.get_ue(t_bench.last_added_ue_idx)));
  ASSERT_EQ(3U, default_slot_grid.result.ul.pucchs.size());
}

TEST_F(pucch_alloc_reach_ul_grant_limits_test, test_max_ul_allocations_reached)
{
  auto& slot_grid = t_bench.res_grid[t_bench.k0 + default_k1];

  slot_grid.result.ul.puschs.emplace_back();
  slot_grid.result.ul.puschs.emplace_back();
  slot_grid.result.ul.puschs.emplace_back();

  ASSERT_TRUE(alloc_csi_opportunity(t_bench.get_main_ue()));
  ASSERT_EQ(1U, slot_grid.result.ul.pucchs.size());

  t_bench.add_ue();
  ASSERT_TRUE(alloc_ded_harq_ack(t_bench.get_ue(t_bench.last_added_ue_idx)));
  ASSERT_EQ(2U, slot_grid.result.ul.pucchs.size());

  t_bench.add_ue();
  ASSERT_TRUE(alloc_ded_harq_ack(t_bench.get_ue(t_bench.last_added_ue_idx)));
  ASSERT_EQ(3U, slot_grid.result.ul.pucchs.size());

  t_bench.add_ue();
  ASSERT_FALSE(alloc_ded_harq_ack(t_bench.get_ue(t_bench.last_added_ue_idx)));
  ASSERT_EQ(3U, slot_grid.result.ul.pucchs.size());
}

TEST_F(pucch_alloc_reach_ul_grant_limits_test, test_sr_max_ul_allocations_reached)
{
  auto& slot_grid = t_bench.res_grid[t_bench.k0 + default_k1];

  for (unsigned n = 0; n != 6U; ++n) {
    slot_grid.result.ul.puschs.emplace_back();
  }

  ASSERT_FALSE(alloc_sr_opportunity(t_bench.get_main_ue()));
  ASSERT_EQ(0U, slot_grid.result.ul.pucchs.size());
}

TEST_F(pucch_alloc_reach_ul_grant_limits_test, test_csi_max_ul_allocations_reached)
{
  auto& slot_grid = t_bench.res_grid[t_bench.k0 + default_k1];

  for (unsigned n = 0; n != 6U; ++n) {
    slot_grid.result.ul.puschs.emplace_back();
  }

  ASSERT_FALSE(alloc_csi_opportunity(t_bench.get_main_ue()));
  ASSERT_EQ(0U, slot_grid.result.ul.pucchs.size());
}
