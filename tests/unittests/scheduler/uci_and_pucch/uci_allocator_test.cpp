// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "tests/unittests/scheduler/test_utils/scheduler_test_suite.h"
#include "uci_test_utils.h"
#include <gtest/gtest.h>

using namespace ocudu;

////////////    Structs with expected parameters and PUCCH sched INPUT     ////////////

// Expected results parameters.
struct expected_output_cfg_params {
  alpha_scaling_opt alpha{alpha_scaling_opt::f1};
  uint8_t           beta_offset_harq_ack{9};
  uint8_t           beta_offset_csi_part1{9};
  uint8_t           beta_offset_csi_part2{6};
};

class uci_alloc_test : public ::testing::Test
{
public:
  uci_alloc_test(const test_bench_params& params = {}) : t_bench(params), k2{t_bench.k0 + default_k1} {}

protected:
  test_bench                 t_bench;
  unsigned                   k2;
  expected_output_cfg_params output_params{};

  static constexpr unsigned default_k1 = 4U;

  bool check_pusch_out_param(const ul_sched_info& pusch_pdu) const
  {
    if (not pusch_pdu.uci.has_value()) {
      return false;
    }
    const bool harq_beta_offset_ok =
        pusch_pdu.uci.value().harq.has_value()
            ? pusch_pdu.uci.value().harq.value().beta_offset_harq_ack == output_params.beta_offset_harq_ack
            : true;
    const bool csi_beta_offset_ok =
        pusch_pdu.uci.value().csi.has_value()
            ? pusch_pdu.uci.value().csi.value().beta_offset_csi_1 == output_params.beta_offset_csi_part1
            : true;

    return pusch_pdu.uci.value().alpha == output_params.alpha and harq_beta_offset_ok and csi_beta_offset_ok;
  }

  void add_sr_grant()
  {
    t_bench.pucch_alloc.alloc_sr_opportunity(t_bench.res_grid[t_bench.k0 + default_k1],
                                             t_bench.get_main_ue().get_pcell().cfg());
  }

  void add_harq_grant_on_pucch(unsigned nof_harq_ack_bits = 1)
  {
    for (unsigned n = 0; n != nof_harq_ack_bits; ++n) {
      t_bench.pucch_alloc.alloc_ded_harq_ack(
          t_bench.res_grid, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, default_k1);
    }
  }

  void add_format2_grant_on_pucch(unsigned nof_harq_ack_bits = 3, sr_nof_bits sr_bits = ocudu::sr_nof_bits::no_sr)
  {
    ocudu_assert(nof_harq_ack_bits > 2, "At least 3 HARQ bits are required to trigger a Format 2 PUCCH grant.");
    t_bench.pucch_alloc.alloc_ded_harq_ack(
        t_bench.res_grid, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, default_k1);
    t_bench.pucch_alloc.alloc_ded_harq_ack(
        t_bench.res_grid, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, default_k1);
    t_bench.pucch_alloc.alloc_ded_harq_ack(
        t_bench.res_grid, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, default_k1);
    auto& pdu                      = *t_bench.res_grid[t_bench.k0 + default_k1].result.ul.pucchs.begin();
    pdu.uci_bits.harq_ack_nof_bits = nof_harq_ack_bits;
    pdu.uci_bits.sr_bits           = sr_bits;
  }

  void add_csi_grant()
  {
    t_bench.pucch_alloc.alloc_csi_opportunity(t_bench.res_grid[t_bench.k0 + default_k1],
                                              t_bench.get_main_ue().get_pcell().cfg());
  }

  void add_pusch_alloc(unsigned pusch_slot_number)
  {
    auto& puschs = t_bench.res_grid[pusch_slot_number].result.ul.puschs;
    puschs.emplace_back(ul_sched_info{});
    puschs.back().pusch_cfg.rnti = t_bench.get_main_ue().crnti;
  }
};

///////   UCI allocate SR    ///////

TEST_F(uci_alloc_test, alloc_sr_only)
{
  auto& slot_grid = t_bench.res_grid[k2];
  t_bench.uci_alloc.alloc_sr_opportunity(slot_grid, t_bench.get_main_ue().get_pcell().cfg());

  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  const auto ue_pucchs =
      test_helpers::find_ue_pucchs(slot_grid.result.ul.pucchs.unsorted(), t_bench.get_main_ue().crnti);
  ASSERT_EQ(1, ue_pucchs.size());
  ASSERT_EQ(sr_nof_bits::one, ue_pucchs[0]->uci_bits.sr_bits);
  ASSERT_EQ(0, ue_pucchs[0]->uci_bits.harq_ack_nof_bits);
}

TEST_F(uci_alloc_test, alloc_sr_over_existing_pusch)
{
  add_pusch_alloc(t_bench.k0 + k2);
  auto& slot_grid = t_bench.res_grid[k2];
  t_bench.uci_alloc.alloc_sr_opportunity(slot_grid, t_bench.get_main_ue().get_pcell().cfg());

  // No grants expected on PUCCH.
  ASSERT_EQ(0, slot_grid.result.ul.pucchs.size());
}

///////   UCI allocate CSI    ///////

TEST_F(uci_alloc_test, alloc_csi_only)
{
  // We assume k2 = k0 + k1;
  auto& slot_grid = t_bench.res_grid[k2];
  t_bench.uci_alloc.alloc_csi_opportunity(slot_grid, t_bench.get_main_ue().get_pcell().cfg());

  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  const auto ue_pucchs =
      test_helpers::find_ue_pucchs(slot_grid.result.ul.pucchs.unsorted(), t_bench.get_main_ue().crnti);
  ASSERT_EQ(1, ue_pucchs.size());
  ASSERT_EQ(4, ue_pucchs[0]->uci_bits.csi_part1_nof_bits);
  ASSERT_EQ(0, ue_pucchs[0]->uci_bits.harq_ack_nof_bits);
  ASSERT_EQ(sr_nof_bits::no_sr, ue_pucchs[0]->uci_bits.sr_bits);
}

TEST_F(uci_alloc_test, alloc_csi_over_sr)
{
  // We assume k2 = k0 + k1;
  auto& slot_grid = t_bench.res_grid[k2];
  t_bench.uci_alloc.alloc_sr_opportunity(slot_grid, t_bench.get_main_ue().get_pcell().cfg());
  t_bench.uci_alloc.alloc_csi_opportunity(slot_grid, t_bench.get_main_ue().get_pcell().cfg());

  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  const auto ue_pucchs =
      test_helpers::find_ue_pucchs(slot_grid.result.ul.pucchs.unsorted(), t_bench.get_main_ue().crnti);
  ASSERT_EQ(1, ue_pucchs.size());
  ASSERT_EQ(4, ue_pucchs[0]->uci_bits.csi_part1_nof_bits);
  ASSERT_EQ(0, ue_pucchs[0]->uci_bits.harq_ack_nof_bits);
  ASSERT_EQ(sr_nof_bits::one, ue_pucchs[0]->uci_bits.sr_bits);
}

TEST_F(uci_alloc_test, csi_over_existing_pusch)
{
  add_pusch_alloc(t_bench.k0 + k2);
  auto& slot_grid = t_bench.res_grid[k2];
  t_bench.uci_alloc.alloc_csi_opportunity(slot_grid, t_bench.get_main_ue().get_pcell().cfg());

  // No grants expected on PUCCH.
  ASSERT_EQ(0, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(1, slot_grid.result.ul.puschs.size());
  ASSERT_TRUE(slot_grid.result.ul.puschs.back().uci.has_value());
  ASSERT_FALSE(slot_grid.result.ul.puschs.back().uci.value().harq.has_value());
  ASSERT_TRUE(slot_grid.result.ul.puschs.back().uci.value().csi.has_value());
  ASSERT_EQ(4, slot_grid.result.ul.puschs.back().uci.value().csi.value().csi_part1_nof_bits);
  ASSERT_FALSE(slot_grid.result.ul.puschs.back().uci.value().csi.value().beta_offset_csi_2.has_value());
}

///////   UCI allocation on PUCCH    ///////

TEST_F(uci_alloc_test, uci_harq_alloc_with_no_pusch_grants)
{
  const std::vector<uint8_t> k1_candidates = {static_cast<uint8_t>(default_k1)};
  t_bench.uci_alloc.alloc_harq_ack(
      t_bench.res_grid, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, k1_candidates);

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + default_k1];

  //  No grants expected on PUSCH.
  ASSERT_EQ(0, slot_grid.result.ul.puschs.size());
  // 1 PUCCH grant expected.
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  const auto ue_pucchs =
      test_helpers::find_ue_pucchs(slot_grid.result.ul.pucchs.unsorted(), t_bench.get_main_ue().crnti);
  ASSERT_EQ(1, ue_pucchs.size());
  ASSERT_EQ(1, ue_pucchs[0]->uci_bits.harq_ack_nof_bits);
  ASSERT_EQ(sr_nof_bits::no_sr, ue_pucchs[0]->uci_bits.sr_bits);
  // Note: no need to check other PUCCH grant values, as this is part of pucch_allocator test.
}

TEST_F(uci_alloc_test, uci_harq_alloc_over_existing_pucch_harq)
{
  add_harq_grant_on_pucch();
  const std::vector<uint8_t> k1_candidates = {static_cast<uint8_t>(default_k1)};
  t_bench.uci_alloc.alloc_harq_ack(
      t_bench.res_grid, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, k1_candidates);

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + default_k1];

  //  No grants expected on PUSCH.
  ASSERT_EQ(0, slot_grid.result.ul.puschs.size());
  // 1 PUCCH grant expected.
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  const auto ue_pucchs =
      test_helpers::find_ue_pucchs(slot_grid.result.ul.pucchs.unsorted(), t_bench.get_main_ue().crnti);
  ASSERT_EQ(1, ue_pucchs.size());
  ASSERT_EQ(2, ue_pucchs[0]->uci_bits.harq_ack_nof_bits);
  ASSERT_EQ(sr_nof_bits::no_sr, ue_pucchs[0]->uci_bits.sr_bits);
  // Note: no need to check other PUCCH grant values, as this is part of pucch_allocator test.
}

TEST_F(uci_alloc_test, uci_harq_alloc_over_existing_sr)
{
  add_sr_grant();
  const std::vector<uint8_t> k1_candidates = {static_cast<uint8_t>(default_k1)};
  t_bench.uci_alloc.alloc_harq_ack(
      t_bench.res_grid, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, k1_candidates);

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + default_k1];

  //  No grants expected on PUSCH.
  ASSERT_EQ(0, slot_grid.result.ul.puschs.size());
  // 2 PUCCH grants expected.
  ASSERT_EQ(2, slot_grid.result.ul.pucchs.size());
  ASSERT_TRUE(find_pucch_pdu(slot_grid.result.ul.pucchs.unsorted(), [](const pucch_info& pdu) {
    return pdu.format() == pucch_format::FORMAT_1 and pdu.uci_bits.harq_ack_nof_bits == 1U and
           pdu.uci_bits.sr_bits == sr_nof_bits::one;
  }));
  ASSERT_TRUE(find_pucch_pdu(slot_grid.result.ul.pucchs.unsorted(), [](const pucch_info& pdu) {
    return pdu.format() == pucch_format::FORMAT_1 and pdu.uci_bits.harq_ack_nof_bits == 1U and
           pdu.uci_bits.sr_bits == sr_nof_bits::no_sr;
  }));
  // Note: no need to check other PUCCH grant values, as this is part of pucch_allocator test.
}

TEST_F(uci_alloc_test, uci_harq_alloc_on_existing_pucch_harq_plus_sr)
{
  add_sr_grant();
  add_harq_grant_on_pucch();
  const std::vector<uint8_t> k1_candidates = {static_cast<uint8_t>(default_k1)};
  t_bench.uci_alloc.alloc_harq_ack(
      t_bench.res_grid, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, k1_candidates);

  //  No grants expected on PUSCH.
  const auto&            pusch_pdus = t_bench.res_grid[t_bench.k0 + default_k1].result.ul.puschs;
  span<const pucch_info> pucch_pdus = t_bench.res_grid[t_bench.k0 + default_k1].result.ul.pucchs.unsorted();
  ASSERT_EQ(0, pusch_pdus.size());
  // 2 PUCCH grants expected, both with HARQ and SR bits.
  ASSERT_EQ(2, pucch_pdus.size());
  ASSERT_TRUE(find_pucch_pdu(pucch_pdus, [](const pucch_info& pdu) {
    return pdu.format() == pucch_format::FORMAT_1 and pdu.uci_bits.harq_ack_nof_bits == 2U and
           pdu.uci_bits.sr_bits == sr_nof_bits::one;
  }));
  ASSERT_TRUE(find_pucch_pdu(pucch_pdus, [](const pucch_info& pdu) {
    return pdu.format() == pucch_format::FORMAT_1 and pdu.uci_bits.harq_ack_nof_bits == 2U and
           pdu.uci_bits.sr_bits == sr_nof_bits::no_sr;
  }));
  // Note: no need to check other PUCCH grant values, as this is part of pucch_allocator test.
}

TEST_F(uci_alloc_test, uci_harq_alloc_on_existing_harq_2_bits)
{
  add_harq_grant_on_pucch(/* Nof. HARQ bits */ 2);
  const std::vector<uint8_t> k1_candidates = {static_cast<uint8_t>(default_k1)};
  t_bench.uci_alloc.alloc_harq_ack(
      t_bench.res_grid, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, k1_candidates);

  const auto&            pusch_pdus = t_bench.res_grid[t_bench.k0 + default_k1].result.ul.puschs;
  span<const pucch_info> pucch_pdus = t_bench.res_grid[t_bench.k0 + default_k1].result.ul.pucchs.unsorted();
  ASSERT_EQ(0, pusch_pdus.size());
  // 1 PUCCH grant expected.
  ASSERT_EQ(1, pucch_pdus.size());
  ASSERT_TRUE(find_pucch_pdu(pucch_pdus, [](const pucch_info& pdu) {
    return pdu.format() == pucch_format::FORMAT_2 and pdu.uci_bits.harq_ack_nof_bits == 3U and
           pdu.uci_bits.sr_bits == sr_nof_bits::no_sr;
  }));
}

///////   UCI allocation on PUSCH    ///////

TEST_F(uci_alloc_test, uci_harq_alloc_on_existing_pusch)
{
  add_pusch_alloc(t_bench.k0 + k2);
  const std::vector<uint8_t> k1_candidates = {static_cast<uint8_t>(default_k1)};
  t_bench.uci_alloc.alloc_harq_ack(
      t_bench.res_grid, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, k1_candidates);

  auto& slot_grid = t_bench.res_grid[k2];

  // No PUCCH grant expected.
  ASSERT_EQ(0, slot_grid.result.ul.pucchs.size());
  //  1 grant expected on PUSCH.
  ASSERT_EQ(1, slot_grid.result.ul.puschs.size());
  // ... but without any UCI.
  ASSERT_FALSE(slot_grid.result.ul.puschs.back().uci.has_value());
}

// A PUSCH belonging to a different UE must not block PUCCH allocation for the target UE.

TEST_F(uci_alloc_test, uci_harq_alloc_not_blocked_by_different_ue_pusch)
{
  // Place a PUSCH for a different UE in the target HARQ-ACK slot.
  const rnti_t other_rnti = to_rnti(0x4602);
  auto&        puschs     = t_bench.res_grid[k2].result.ul.puschs;
  puschs.emplace_back(ul_sched_info{});
  puschs.back().pusch_cfg.rnti = other_rnti;

  const std::vector<uint8_t> k1_candidates = {static_cast<uint8_t>(default_k1)};
  const auto                 result        = t_bench.uci_alloc.alloc_harq_ack(
      t_bench.res_grid, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, k1_candidates);

  // The other UE's PUSCH must not prevent allocation of UE0's PUCCH HARQ-ACK.
  ASSERT_TRUE(result.has_value()) << "HARQ-ACK allocation failed: slot has been skipped due to another UE's PUSCH";
  ASSERT_EQ(default_k1, result->k1);

  auto& slot_grid = t_bench.res_grid[k2];
  // The other UE's PUSCH is still present.
  ASSERT_EQ(1, slot_grid.result.ul.puschs.size());
  ASSERT_EQ(other_rnti, slot_grid.result.ul.puschs.front().pusch_cfg.rnti);
  // UE0's PUCCH HARQ-ACK is allocated alongside the other UE's PUSCH.
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  const auto ue_pucchs =
      test_helpers::find_ue_pucchs(slot_grid.result.ul.pucchs.unsorted(), t_bench.get_main_ue().crnti);
  ASSERT_EQ(1, ue_pucchs.size());
  ASSERT_EQ(1, ue_pucchs[0]->uci_bits.harq_ack_nof_bits);
}

TEST_F(uci_alloc_test, uci_harq_alloc_picks_closest_k1_ignoring_different_ue_pusch)
{
  // Place a PUSCH for a different UE at the closest k1=4 slot.
  const rnti_t other_rnti = to_rnti(0x4602);
  // near_k1 = 4
  const auto near_k1 = static_cast<uint8_t>(default_k1);
  // far_k1 = 8
  const auto far_k1              = static_cast<uint8_t>(default_k1 + 4U);
  auto&      puschs_at_near_slot = t_bench.res_grid[t_bench.k0 + near_k1].result.ul.puschs;
  puschs_at_near_slot.emplace_back(ul_sched_info{});
  puschs_at_near_slot.back().pusch_cfg.rnti = other_rnti;

  // Prepare k1_candidates with expected one and not expected.
  const std::vector<uint8_t> k1_candidates = {near_k1, far_k1};
  const auto                 result        = t_bench.uci_alloc.alloc_harq_ack(
      t_bench.res_grid, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, k1_candidates);

  ASSERT_TRUE(result.has_value());
  // Shall use k1=4, not fall back to k1=8.
  ASSERT_EQ(near_k1, result->k1) << "k1=" << (unsigned)near_k1 << " was skipped due to another UE's PUSCH; "
                                 << "got k1=" << result->k1 << " instead";

  // PUCCH shall be at the near slot, not the far slot.
  ASSERT_EQ(1, t_bench.res_grid[t_bench.k0 + near_k1].result.ul.pucchs.size());
  ASSERT_EQ(0, t_bench.res_grid[t_bench.k0 + far_k1].result.ul.pucchs.size());
}

///////   UCI multiplexing on PUSCH    ///////

TEST_F(uci_alloc_test, uci_mplexing_on_pusch_with_no_pucch_grants)
{
  add_pusch_alloc(t_bench.k0 + k2);
  auto& slot_grid = t_bench.res_grid[k2];

  // 1 PUSCH (without UCI) before multiplexing.
  ASSERT_EQ(1, slot_grid.result.ul.puschs.size());

  t_bench.uci_alloc.multiplex_uci_on_pusch(
      slot_grid.result.ul.puschs.back(), slot_grid, t_bench.get_main_ue().get_pcell().cfg(), false);

  // No grants expected on PUCCH.
  ASSERT_EQ(0, slot_grid.result.ul.pucchs.size());
  // 1 expected PUSCH grant.
  ASSERT_EQ(1, slot_grid.result.ul.puschs.size());
  ASSERT_FALSE(slot_grid.result.ul.puschs.back().uci.has_value());
}

TEST_F(uci_alloc_test, uci_mplexing_harq_on_pusch)
{
  add_pusch_alloc(t_bench.k0 + k2);
  add_harq_grant_on_pucch();
  auto& slot_grid = t_bench.res_grid[k2];

  // 1 PUSCH (without UCI) and 1 PUCCH grant expected before multiplexing.
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(1, slot_grid.result.ul.puschs.size());

  t_bench.uci_alloc.multiplex_uci_on_pusch(
      slot_grid.result.ul.puschs.back(), slot_grid, t_bench.get_main_ue().get_pcell().cfg(), false);

  // No grants expected on PUCCH.
  ASSERT_EQ(0, slot_grid.result.ul.pucchs.size());
  // 1 expected PUSCH grant.
  ASSERT_EQ(1, slot_grid.result.ul.puschs.size());
  ASSERT_TRUE(slot_grid.result.ul.puschs.back().uci.value().harq.has_value());
  ASSERT_EQ(1, slot_grid.result.ul.puschs.back().uci.value().harq.value().harq_ack_nof_bits);
  ASSERT_FALSE(slot_grid.result.ul.puschs.back().uci.value().csi.has_value());
  ASSERT_TRUE(check_pusch_out_param(slot_grid.result.ul.puschs.back()));
}

TEST_F(uci_alloc_test, uci_mplexing_3_bit_harq_on_pusch)
{
  add_pusch_alloc(t_bench.k0 + k2);
  add_format2_grant_on_pucch();
  auto& slot_grid = t_bench.res_grid[k2];

  // 1 PUSCH (without UCI) and 1 PUCCH grant expected before multiplexing.
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(1, slot_grid.result.ul.puschs.size());
  ASSERT_FALSE(slot_grid.result.ul.puschs.back().uci.has_value());

  t_bench.uci_alloc.multiplex_uci_on_pusch(
      slot_grid.result.ul.puschs.back(), slot_grid, t_bench.get_main_ue().get_pcell().cfg(), false);

  // No grants expected on PUCCH.
  ASSERT_EQ(0, slot_grid.result.ul.pucchs.size());
  // 1 expected PUSCH grant.
  ASSERT_EQ(1, slot_grid.result.ul.puschs.size());
  ASSERT_TRUE(slot_grid.result.ul.puschs.back().uci.has_value());
  ASSERT_TRUE(slot_grid.result.ul.puschs.back().uci.value().harq.has_value());
  ASSERT_EQ(3, slot_grid.result.ul.puschs.back().uci.value().harq.value().harq_ack_nof_bits);
  ASSERT_FALSE(slot_grid.result.ul.puschs.back().uci.value().csi.has_value());
  ASSERT_TRUE(check_pusch_out_param(slot_grid.result.ul.puschs.back()));
}

TEST_F(uci_alloc_test, uci_mplexing_harq_sr_on_pusch)
{
  add_pusch_alloc(t_bench.k0 + k2);
  add_sr_grant();
  add_harq_grant_on_pucch();
  auto& slot_grid = t_bench.res_grid[k2];

  // 1 PUSCH grant (without UCI) and 2 PUCCH grants expected before multiplexing.
  ASSERT_EQ(2, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(1, slot_grid.result.ul.puschs.size());
  ASSERT_FALSE(slot_grid.result.ul.puschs.back().uci.has_value());

  t_bench.uci_alloc.multiplex_uci_on_pusch(
      slot_grid.result.ul.puschs.back(), slot_grid, t_bench.get_main_ue().get_pcell().cfg(), false);

  // No grants expected on PUCCH.
  ASSERT_EQ(0, slot_grid.result.ul.pucchs.size());
  // 1 expected PUSCH grant.
  ASSERT_EQ(1, slot_grid.result.ul.puschs.size());
  ASSERT_TRUE(slot_grid.result.ul.puschs.back().uci.has_value());
  ASSERT_TRUE(slot_grid.result.ul.puschs.back().uci.value().harq.has_value());
  ASSERT_EQ(1, slot_grid.result.ul.puschs.back().uci.value().harq.value().harq_ack_nof_bits);
  ASSERT_FALSE(slot_grid.result.ul.puschs.back().uci.value().csi.has_value());
  ASSERT_TRUE(check_pusch_out_param(slot_grid.result.ul.puschs.back()));
}

TEST_F(uci_alloc_test, uci_multiplexing_3_bit_harq_sr_on_pusch)
{
  add_pusch_alloc(t_bench.k0 + k2);
  add_format2_grant_on_pucch(3, ocudu::sr_nof_bits::one);
  auto& slot_grid = t_bench.res_grid[k2];

  // 1 PUSCH grant (without UCI) and 2 PUCCH grants expected before multiplexing.
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(1, slot_grid.result.ul.puschs.size());
  ASSERT_FALSE(slot_grid.result.ul.puschs.back().uci.has_value());

  t_bench.uci_alloc.multiplex_uci_on_pusch(
      slot_grid.result.ul.puschs.back(), slot_grid, t_bench.get_main_ue().get_pcell().cfg(), false);

  // No grants expected on PUCCH.
  ASSERT_EQ(0, slot_grid.result.ul.pucchs.size());
  // 1 expected PUSCH grant.
  ASSERT_EQ(1, slot_grid.result.ul.puschs.size());
  ASSERT_TRUE(slot_grid.result.ul.puschs.back().uci.has_value());
  ASSERT_TRUE(slot_grid.result.ul.puschs.back().uci.value().harq.has_value());
  ASSERT_EQ(3, slot_grid.result.ul.puschs.back().uci.value().harq.value().harq_ack_nof_bits);
  ASSERT_FALSE(slot_grid.result.ul.puschs.back().uci.value().csi.has_value());
  ASSERT_TRUE(check_pusch_out_param(slot_grid.result.ul.puschs.back()));
}

TEST_F(uci_alloc_test, uci_multiplexing_2_bit_harq_sr_csi_on_pusch)
{
  add_pusch_alloc(t_bench.k0 + k2);
  add_csi_grant();
  add_sr_grant();
  add_harq_grant_on_pucch(2);
  auto& slot_grid = t_bench.res_grid[k2];

  // 1 PUSCH grant (without UCI) and 1 PUCCH grants expected before multiplexing.
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(1, slot_grid.result.ul.puschs.size());
  ASSERT_FALSE(slot_grid.result.ul.puschs.back().uci.has_value());

  t_bench.uci_alloc.multiplex_uci_on_pusch(
      slot_grid.result.ul.puschs.back(), slot_grid, t_bench.get_main_ue().get_pcell().cfg(), false);

  // No grants expected on PUCCH.
  ASSERT_EQ(0, slot_grid.result.ul.pucchs.size());
  // 1 expected PUSCH grant.
  ASSERT_EQ(1, slot_grid.result.ul.puschs.size());
  const auto& pusch = slot_grid.result.ul.puschs.back();
  ASSERT_TRUE(pusch.uci.has_value());
  ASSERT_TRUE(pusch.uci->harq.has_value());
  ASSERT_EQ(2, pusch.uci->harq->harq_ack_nof_bits);
  ASSERT_TRUE(pusch.uci->csi.has_value());
  ASSERT_EQ(4, pusch.uci->csi->csi_part1_nof_bits);
  ASSERT_FALSE(pusch.uci->csi->beta_offset_csi_2.has_value());
  ASSERT_TRUE(check_pusch_out_param(slot_grid.result.ul.puschs.back()));
}

///////   UCI allocation in TDD ///////

class uci_alloc_tdd_test : public uci_alloc_test
{
protected:
  static constexpr size_t DAI_MOD = 4;

  uci_alloc_tdd_test() : uci_alloc_test(test_bench_params{.tdd = true}) {}
};

TEST_F(uci_alloc_tdd_test, when_tdd_cfg_then_harq_bit_index_increases_with_number_of_allocs)
{
  const std::vector<uint8_t> k1_candidates = {static_cast<uint8_t>(default_k1)};
  for (unsigned i = 0; i != DAI_MOD * 2; ++i) {
    std::optional<uci_allocation> alloc = t_bench.uci_alloc.alloc_harq_ack(
        t_bench.res_grid, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, k1_candidates);

    if (alloc.has_value()) {
      ASSERT_EQ(alloc.value().harq_bit_idx, i);
    }
  }
}

class uci_alloc_mimo_4x4_test : public ::testing::Test
{
public:
  uci_alloc_mimo_4x4_test() :
    t_bench(

        test_bench_params{
            .pucch_ded_params{.f2_or_f3_or_f4_params =
                                  pucch_f2_params{.max_nof_rbs = 2U, .max_code_rate = max_pucch_code_rate::dot_35}},
            .nof_ul_dl_ports = 4}),
    k2{t_bench.k0 + default_k1}
  {
  }

protected:
  test_bench                 t_bench;
  unsigned                   k2;
  expected_output_cfg_params output_params{};

  static constexpr unsigned default_k1 = 4U;

  bool check_pusch_out_param(const ul_sched_info& pusch_pdu) const
  {
    if (not pusch_pdu.uci.has_value()) {
      return false;
    }
    const bool harq_beta_offset_ok =
        pusch_pdu.uci.value().harq.has_value()
            ? pusch_pdu.uci.value().harq.value().beta_offset_harq_ack == output_params.beta_offset_harq_ack
            : true;
    const bool csi_p1_beta_offset_ok =
        pusch_pdu.uci.value().csi.has_value()
            ? pusch_pdu.uci.value().csi.value().beta_offset_csi_1 == output_params.beta_offset_csi_part1
            : true;
    const bool csi_p2_beta_offset_ok =
        pusch_pdu.uci.value().csi.has_value() and pusch_pdu.uci.value().csi.value().beta_offset_csi_2.has_value()
            ? pusch_pdu.uci.value().csi.value().beta_offset_csi_2.value() == output_params.beta_offset_csi_part2
            : true;

    return pusch_pdu.uci.value().alpha == output_params.alpha and harq_beta_offset_ok and csi_p1_beta_offset_ok and
           csi_p2_beta_offset_ok;
  }

  void add_pusch_alloc(unsigned pusch_slot_number)
  {
    auto& puschs = t_bench.res_grid[pusch_slot_number].result.ul.puschs;
    puschs.emplace_back(ul_sched_info{});
    puschs.back().pusch_cfg.rnti = t_bench.get_main_ue().crnti;
  }

  void add_csi_grant()
  {
    t_bench.pucch_alloc.alloc_csi_opportunity(t_bench.res_grid[t_bench.k0 + default_k1],
                                              t_bench.get_main_ue().get_pcell().cfg());
  }
};

TEST_F(uci_alloc_mimo_4x4_test, uci_alloc_csi_part2_over_existing_pusch)
{
  add_pusch_alloc(t_bench.k0 + k2);
  auto& slot_grid = t_bench.res_grid[k2];
  t_bench.uci_alloc.alloc_csi_opportunity(slot_grid, t_bench.get_main_ue().get_pcell().cfg());

  // No grants expected on PUCCH.
  ASSERT_EQ(0, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(1, slot_grid.result.ul.puschs.size());
  ASSERT_TRUE(slot_grid.result.ul.puschs.back().uci.has_value());
  ASSERT_FALSE(slot_grid.result.ul.puschs.back().uci.value().harq.has_value());
  ASSERT_TRUE(slot_grid.result.ul.puschs.back().uci.value().csi.has_value());
  ASSERT_EQ(11, slot_grid.result.ul.puschs.back().uci.value().csi.value().csi_part1_nof_bits);
  ASSERT_FALSE(slot_grid.result.ul.puschs.back().uci.value().csi.value().beta_offset_csi_2.has_value());
}

TEST_F(uci_alloc_mimo_4x4_test, uci_mplex_csi_part2_over_existing_pusch)
{
  add_csi_grant();
  add_pusch_alloc(t_bench.k0 + k2);
  auto& slot_grid = t_bench.res_grid[k2];

  // 1 PUSCH grant (without UCI) and 2 PUCCH grants expected before multiplexing.
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(1, slot_grid.result.ul.puschs.size());
  ASSERT_FALSE(slot_grid.result.ul.puschs.back().uci.has_value());

  t_bench.uci_alloc.multiplex_uci_on_pusch(
      slot_grid.result.ul.puschs.back(), slot_grid, t_bench.get_main_ue().get_pcell().cfg(), false);

  // No grants expected on PUCCH.
  ASSERT_EQ(0, slot_grid.result.ul.pucchs.size());
  // 1 expected PUSCH grant.
  ASSERT_EQ(1, slot_grid.result.ul.puschs.size());
  ASSERT_TRUE(slot_grid.result.ul.puschs.back().uci.has_value());
  ASSERT_FALSE(slot_grid.result.ul.puschs.back().uci.value().harq.has_value());
  ASSERT_TRUE(slot_grid.result.ul.puschs.back().uci.value().csi.has_value());
  ASSERT_EQ(11, slot_grid.result.ul.puschs.back().uci.value().csi.value().csi_part1_nof_bits);
  ASSERT_FALSE(slot_grid.result.ul.puschs.back().uci.value().csi.value().beta_offset_csi_2.has_value());
  ASSERT_TRUE(check_pusch_out_param(slot_grid.result.ul.puschs.back()));
}
