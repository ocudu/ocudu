// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "../test_utils/scheduler_test_suite.h"
#include "lib/scheduler/cell/resource_grid.h"
#include "lib/scheduler/support/sched_result_helpers.h"
#include "pucch_alloc_base_tester.h"
#include "uci_test_utils.h"
#include <gtest/gtest.h>

using namespace ocudu;

///////   Test PUCCH Format 0.    ///////

class pucch_alloc_format_0_test : public ::testing::Test, public pucch_allocator_base_test
{
public:
  pucch_alloc_format_0_test() :
    pucch_allocator_base_test(
        test_bench_params{.pucch_ded_params = {.f0_or_f1_params = pucch_f0_params{}}, .pucch_res_common = 0U})
  {
    const auto& pucch_res_list = t_bench.get_main_ue().get_pcell().cfg().init_bwp().ul.pucch_ded()->pucch_res_list;
    const auto& res_params     = t_bench.params.pucch_ded_params;

    // Set the expected SR grant to the SR resource.
    pucch_expected_sr = test_helpers::make_ded_pucch_info(
        t_bench.cell_cfg, pucch_res_list[res_params.get_sr_ue_res_idx()], {.sr_bits = sr_nof_bits::one});

    // Set the expected HARQ F1 grant to the first resource in Resource Set ID 0.
    pucch_expected_res_set_0 = test_helpers::make_ded_pucch_info(
        t_bench.cell_cfg, pucch_res_list[res_params.get_res_set_ue_res_idx<0>(0)], {.harq_ack_nof_bits = 1U});

    // Set the expected Resource Set ID 1 HARQ grant to the first resource in Resource Set ID 1.
    pucch_expected_res_set_1 = test_helpers::make_ded_pucch_info(
        t_bench.cell_cfg, pucch_res_list[res_params.get_res_set_ue_res_idx<1>(0)], {.harq_ack_nof_bits = 3U});

    // This PUCCH resource is located on the same symbols and PRBs as the PUCCH Format 0 resource for SR.
    // Resource of Resource Set ID 1.
    pucch_expected_sr_f2 = test_helpers::make_ded_pucch_info(
        t_bench.cell_cfg, pucch_res_list[res_params.get_sr_f2_ue_res_idx()], {.harq_ack_nof_bits = 3U});

    // Set the expected HARQ CSI grant to the CSI resource.
    pucch_expected_csi = test_helpers::make_ded_pucch_info(t_bench.cell_cfg,
                                                           pucch_res_list[res_params.get_csi_ue_res_idx()],
                                                           {.csi_part1_nof_bits = default_csi_part1_bits});
  }

protected:
  // Parameters that are passed by the routine to run the tests.
  pucch_info pucch_expected_sr;
  pucch_info pucch_expected_res_set_0;
  pucch_info pucch_expected_sr_f2;
  pucch_info pucch_expected_res_set_1;
  pucch_info pucch_expected_csi;

  static constexpr unsigned default_csi_part1_bits = 4U;
};

TEST_F(pucch_alloc_format_0_test, test_sr_allocation_only)
{
  alloc_sr_opportunity(t_bench.get_main_ue());

  ASSERT_EQ(1U, default_slot_grid.result.ul.pucchs.size());
  ASSERT_TRUE(find_pucch_pdu(default_slot_grid.result.ul.pucchs, [&expected = pucch_expected_sr](const auto& pdu) {
    return pucch_info_match(expected, pdu);
  }));
}

TEST_F(pucch_alloc_format_0_test, test_harq_allocation_only)
{
  auto pri = alloc_ded_harq_ack(t_bench.get_main_ue());
  ASSERT_TRUE(pri.has_value());
  ASSERT_EQ(0U, pri);

  ASSERT_EQ(1U, default_slot_grid.result.ul.pucchs.size());
  ASSERT_TRUE(
      find_pucch_pdu(default_slot_grid.result.ul.pucchs, [&expected = pucch_expected_res_set_0](const auto& pdu) {
        return pucch_info_match(expected, pdu);
      }));
}

TEST_F(pucch_alloc_format_0_test, test_harq_allocation_2_bits)
{
  auto pri = alloc_ded_harq_ack(t_bench.get_main_ue());
  ASSERT_TRUE(pri.has_value());
  ASSERT_EQ(0U, pri);

  ASSERT_EQ(1U, default_slot_grid.result.ul.pucchs.size());

  auto pri_new = alloc_ded_harq_ack(t_bench.get_main_ue());
  ASSERT_TRUE(pri_new.has_value());
  ASSERT_EQ(pri, pri_new);

  // PUCCH resource indicator after the second allocation should not have changed.
  pucch_expected_res_set_0.uci_bits.harq_ack_nof_bits = 2U;
  ASSERT_EQ(1, default_slot_grid.result.ul.pucchs.size());
  ASSERT_TRUE(
      find_pucch_pdu(default_slot_grid.result.ul.pucchs, [&expected = pucch_expected_res_set_0](const auto& pdu) {
        return pucch_info_match(expected, pdu);
      }));
}

TEST_F(pucch_alloc_format_0_test, alloc_ded_harq_ack_with_existing_sr_succeeds_until_max_payload_reached)
{
  const unsigned max_payload_f2 = t_bench.params.pucch_ded_params.max_payload_234();

  // Add HARQ grants to reach the max_payload.
  alloc_sr_opportunity(t_bench.get_main_ue());
  for (unsigned n = 0; n != (max_payload_f2 - 1); ++n) {
    // These grants should be allocated successfully, as we are below the max payload.
    auto pri = alloc_ded_harq_ack(t_bench.get_main_ue());
    ASSERT_TRUE(pri.has_value());

    ASSERT_EQ(1U, default_slot_grid.result.ul.pucchs.size());
    const unsigned harq_ack_nof_bits = n + 1;
    if (harq_ack_nof_bits <= 2U) {
      // According to the multiplexing procedure defined by TS 38.213, Section 9.2.5, the resource to use to report 1
      // HARQ-ACK bit + 1 SR bit is the HARQ-ACK resource. However, to circumvent the lack of capability of some UES
      // (that cannot transmit more than 1 PUCCH), we set last resource of PUCCH resource set 0 to be the SR resource
      // and the UE will use this.
      pucch_expected_sr.uci_bits.harq_ack_nof_bits = harq_ack_nof_bits;
      ASSERT_TRUE(
          find_pucch_pdu(default_slot_grid.result.ul.pucchs, [&expected = pucch_expected_sr](const pucch_info& pdu) {
            return pucch_info_match(expected, pdu);
          }));
    } else {
      pucch_expected_sr_f2.uci_bits.harq_ack_nof_bits = harq_ack_nof_bits;
      pucch_expected_sr_f2.uci_bits.sr_bits           = sr_nof_bits::one;
      ASSERT_EQ(1U, default_slot_grid.result.ul.pucchs.size());
      ASSERT_TRUE(
          find_pucch_pdu(default_slot_grid.result.ul.pucchs, [&expected = pucch_expected_sr_f2](const auto& pdu) {
            return pucch_info_match(expected, pdu);
          }));
    }

    // Check that the resources were reserved in the resource grid.
    const auto grants = get_pucch_grant_info(default_slot_grid.result.ul.pucchs.front());
    ASSERT_TRUE(default_slot_grid.ul_res_grid.all_set(grants.first));
    if (grants.second.has_value()) {
      ASSERT_TRUE(default_slot_grid.ul_res_grid.all_set(*grants.second));
    }
  }

  // The last HARQ allocation should fail, as the max PUCCH payload has been reached.
  auto pri = alloc_ded_harq_ack(t_bench.get_main_ue());
  ASSERT_FALSE(pri.has_value());
}

TEST_F(pucch_alloc_format_0_test, alloc_ded_harq_ack_with_existing_csi_succeeds_until_max_payload_reached)
{
  const unsigned max_payload_f2 = t_bench.params.pucch_ded_params.max_payload_234();

  // Add HARQ grants to reach the max_payload.
  alloc_csi_opportunity(t_bench.get_main_ue(), default_csi_part1_bits);
  for (unsigned n = 0; n != (max_payload_f2 - default_csi_part1_bits); ++n) {
    // These grants should be allocated successfully, as we are below the max payload.
    auto pri = alloc_ded_harq_ack(t_bench.get_main_ue());
    ASSERT_TRUE(pri.has_value());

    ASSERT_EQ(1U, default_slot_grid.result.ul.pucchs.size());
    const unsigned harq_ack_nof_bits = n + 1;
    // After the multiplexing, the PUCCH F2 resource is that one that have the same PUCCH resource indicator as
    // pucch_res_idx_f0_for_csi; we need to update the PRBs and symbols accordingly. With the given configuration,
    // this resource will have the same PRBs and symbols as the F2 resource for SR.
    pucch_expected_csi.uci_bits.harq_ack_nof_bits = harq_ack_nof_bits;
    ASSERT_TRUE(
        find_pucch_pdu(default_slot_grid.result.ul.pucchs, [&expected = pucch_expected_csi](const pucch_info& pdu) {
          return pucch_info_match(expected, pdu);
        }));

    // Check that the resources were reserved in the resource grid.
    const auto grants = get_pucch_grant_info(default_slot_grid.result.ul.pucchs.front());
    ASSERT_TRUE(default_slot_grid.ul_res_grid.all_set(grants.first));
    if (grants.second.has_value()) {
      ASSERT_TRUE(default_slot_grid.ul_res_grid.all_set(*grants.second));
    }
  }

  // The last HARQ allocation should fail, as the max PUCCH payload has been reached.
  auto pri = alloc_ded_harq_ack(t_bench.get_main_ue());
  ASSERT_FALSE(pri.has_value());
}
