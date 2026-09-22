// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "pucch_alloc_base_tester.h"
#include "tests/ocudu_test_requirements.h"
#include "uci_test_utils.h"
#include "ocudu/ran/band_helper.h"
#include "ocudu/ran/pucch/pucch_configuration.h"
#include "ocudu/scheduler/config/pucch_resource_builder_params.h"
#include "ocudu/scheduler/rrm/ue_capability_summary.h"
#include <gtest/gtest.h>

using namespace ocudu;

///////   Test PUCCH HARQ-ACK repetition scheduling    ///////

// Builds cell PUCCH resource params with HARQ-ACK repetition configured: the first 2 PRIs of the resource set use
// n4, the next 2 use n2, and the last 2 use n1 (default res_set_size == 6). If \c uniform_factor is set, all the PRIs
// are configured with that factor instead.
static pucch_resource_builder_params
make_rep_pucch_params(std::optional<pucch_repetition_factor> uniform_factor = std::nullopt)
{
  pucch_resource_builder_params params{.res_set_size = 6};
  params.harq_ack_rep = pucch_harq_ack_rep_params{.sinr_thresholds = {-5.0F, 0.0F},
                                                  .factors_per_res = {pucch_repetition_factor::n4,
                                                                      pucch_repetition_factor::n4,
                                                                      pucch_repetition_factor::n2,
                                                                      pucch_repetition_factor::n2,
                                                                      pucch_repetition_factor::n1,
                                                                      pucch_repetition_factor::n1}};
  if (uniform_factor.has_value()) {
    params.harq_ack_rep->factors_per_res.assign(params.res_set_size.value(), *uniform_factor);
  }
  return params;
}

// UE capabilities that support dynamic PUCCH repetition for Format 1 (the default HARQ-ACK Resource Set 0 format).
static ue_capability_summary make_rep_supported_caps()
{
  ue_capability_summary caps;
  caps.pucch_repeat_f1_3_4_supported          = true;
  caps.slot_based_dyn_pucch_rep_r17_supported = true;
  return caps;
}

// UE capabilities that additionally support dynamic PUCCH repetition for Format 0/2 (Resource Set 1's format in this
// bench). This is reported per-band, so the band of the test bench's cell must be used: the DL ARFCN is 365000 for
// the default (non-TDD) cell, and 520000 for the TDD one.
static ue_capability_summary make_rep_supported_caps_with_f0_2(unsigned dl_arfcn = 365000)
{
  ue_capability_summary caps = make_rep_supported_caps();
  const nr_band         band = band_helper::get_band_from_dl_arfcn(dl_arfcn);
  caps.bands.emplace(band, ue_capability_summary::supported_band{.pucch_repeat_f0_2_r17_supported = true});
  return caps;
}

// Asserts that each of the given slot delays holds exactly one PUCCH grant of the UE, carrying \c nof_harq_bits
// HARQ-ACK bits and (if given) \c expected_format, and marked with its position within the repetition burst and the
// slot the burst starts at.
static void expect_burst(test_bench&                  t_bench,
                         const ue&                    ue,
                         const std::vector<unsigned>& delays,
                         unsigned                     nof_harq_bits,
                         std::optional<pucch_format>  expected_format = std::nullopt)
{
  for (unsigned idx = 0; idx != delays.size(); ++idx) {
    SCOPED_TRACE("burst slot at delay " + std::to_string(delays[idx]));
    const auto ue_pucchs =
        test_helpers::find_ue_pucchs(t_bench.res_grid[delays[idx]].result.ul.pucchs.unsorted(), ue.crnti);
    ASSERT_EQ(ue_pucchs.size(), 1);
    EXPECT_EQ(ue_pucchs[0]->uci_bits.harq_ack_nof_bits, nof_harq_bits);
    if (expected_format.has_value()) {
      EXPECT_EQ(ue_pucchs[0]->format(), *expected_format);
    }

    pucch_repetition_tx_slot expected_rep_state = pucch_repetition_tx_slot::continues;
    if (idx == 0) {
      expected_rep_state = pucch_repetition_tx_slot::starts;
    } else if (idx + 1 == delays.size()) {
      expected_rep_state = pucch_repetition_tx_slot::ends;
    }
    ASSERT_TRUE(ue_pucchs[0]->repetition.has_value());
    EXPECT_EQ(ue_pucchs[0]->repetition->position, expected_rep_state);
    EXPECT_EQ(ue_pucchs[0]->repetition->anchor_slot, t_bench.res_grid[delays.front()].slot);
  }
}

class pucch_alloc_repetition_test : public ::testing::Test, public pucch_allocator_base_test
{
public:
  pucch_alloc_repetition_test() :
    pucch_allocator_base_test({.pucch_ded_params = make_rep_pucch_params(), .ue_caps = make_rep_supported_caps()})
  {
  }
};

TEST_F(pucch_alloc_repetition_test, successful_repetition_burst_uses_cell_configured_factor)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-BW-17-1");

  const ue& ue = t_bench.get_main_ue();

  const std::optional<unsigned> pri = alloc_ded_harq_ack(ue, pucch_repetition_factor::n4);
  ASSERT_TRUE(pri.has_value());

  // The anchor and the following 3 slots (n4 == 4 slots) must each carry exactly 1 HARQ-ACK PUCCH PDU for the UE,
  // with the correct repetition-burst position.
  const unsigned anchor = t_bench.k0 + default_k1;
  expect_burst(t_bench, ue, {anchor, anchor + 1, anchor + 2, anchor + 3}, 1U);

  // No PUCCH grant should leak into the slot right after the burst.
  const auto& after_grid = t_bench.res_grid[t_bench.k0 + default_k1 + 4];
  EXPECT_TRUE(test_helpers::find_ue_pucchs(after_grid.result.ul.pucchs.unsorted(), ue.crnti).empty());
}

TEST_F(pucch_alloc_repetition_test, falls_back_to_smaller_factor_when_a_future_slot_is_unavailable)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-BW-17-1");

  const ue& ue = t_bench.get_main_ue();

  // Block the 4th slot of a would-be n4 burst (delay +3) with an unrelated SR grant for the same UE. This leaves
  // enough room for an n2 burst (which only needs the first 2 slots), but not for n4.
  ASSERT_TRUE(
      t_bench.pucch_alloc.alloc_sr_opportunity(t_bench.res_grid[t_bench.k0 + default_k1 + 3], ue.get_pcell().cfg()));

  const std::optional<unsigned> pri = alloc_ded_harq_ack(ue, pucch_repetition_factor::n4);
  ASSERT_TRUE(pri.has_value());

  const unsigned anchor = t_bench.k0 + default_k1;
  expect_burst(t_bench, ue, {anchor, anchor + 1}, 1U);

  // The blocked slot must still only contain the pre-existing SR grant, untouched by the repetition burst.
  const auto& blocked_grid   = t_bench.res_grid[t_bench.k0 + default_k1 + 3];
  const auto  blocked_pucchs = test_helpers::find_ue_pucchs(blocked_grid.result.ul.pucchs.unsorted(), ue.crnti);
  ASSERT_EQ(blocked_pucchs.size(), 1);
  EXPECT_NE(blocked_pucchs[0]->uci_bits.sr_bits, sr_nof_bits::no_sr);
}

TEST_F(pucch_alloc_repetition_test, falls_back_to_legacy_single_slot_allocation_when_no_factor_fits)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-BW-17-1");

  const ue& ue = t_bench.get_main_ue();

  // Block both the 2nd slot (delay +1, needed by n2) and the 4th slot (delay +3, needed by n4) for the same UE, so
  // that no repetition factor greater than n1 can find enough free slots.
  ASSERT_TRUE(
      t_bench.pucch_alloc.alloc_sr_opportunity(t_bench.res_grid[t_bench.k0 + default_k1 + 1], ue.get_pcell().cfg()));

  const std::optional<unsigned> pri = alloc_ded_harq_ack(ue, pucch_repetition_factor::n4);
  ASSERT_TRUE(pri.has_value());

  // Falls back to the regular single-slot (legacy) path: only the anchor slot is used, and it is not marked as part
  // of a repetition burst.
  const auto& anchor_grid   = t_bench.res_grid[t_bench.k0 + default_k1];
  const auto  anchor_pucchs = test_helpers::find_ue_pucchs(anchor_grid.result.ul.pucchs.unsorted(), ue.crnti);
  ASSERT_EQ(anchor_pucchs.size(), 1);
  EXPECT_EQ(anchor_pucchs[0]->uci_bits.harq_ack_nof_bits, 1U);
  EXPECT_FALSE(anchor_pucchs[0]->repetition.has_value());
}

TEST_F(pucch_alloc_repetition_test, repetition_burst_rejects_sr_and_csi_grants_in_its_slots)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-BW-17-1");

  const ue& ue = t_bench.get_main_ue();

  ASSERT_TRUE(alloc_ded_harq_ack(ue, pucch_repetition_factor::n4).has_value());

  // Any SR or CSI allocation attempt landing on a slot already claimed by the repetition burst must be rejected,
  // since repetition PUCCH cannot be multiplexed with other UCI types and the UE must not transmit more than one
  // PUCCH per slot.
  EXPECT_FALSE(
      t_bench.pucch_alloc.alloc_sr_opportunity(t_bench.res_grid[t_bench.k0 + default_k1 + 1], ue.get_pcell().cfg()));
  EXPECT_FALSE(
      t_bench.pucch_alloc.alloc_csi_opportunity(t_bench.res_grid[t_bench.k0 + default_k1 + 2], ue.get_pcell().cfg()));
}

TEST_F(pucch_alloc_repetition_test, harq_ack_bit_targeting_a_non_anchor_burst_slot_is_rejected)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-BW-17-1");

  const ue& ue = t_bench.get_main_ue();

  ASSERT_TRUE(alloc_ded_harq_ack(ue, pucch_repetition_factor::n4).has_value());

  // A different HARQ-ACK bit whose own feedback timing (k1) resolves to the burst's 2nd slot is a distinct PUCCH
  // occasion that merely collides physically with the ongoing repetition burst; it isn't the same occasion as the
  // bit that started the burst (that one's k1 resolves to the anchor slot only), so it must be rejected rather than
  // merged into the burst.
  const std::optional<unsigned> pri = t_bench.pucch_alloc.alloc_ded_harq_ack(
      t_bench.res_grid, ue.get_pcell().cfg(), t_bench.k0, default_k1 + 1, pucch_repetition_factor::n4);
  EXPECT_FALSE(pri.has_value());

  // The colliding slot must still only contain the original burst's grant, with its bit count unchanged.
  const auto& slot_grid = t_bench.res_grid[t_bench.k0 + default_k1 + 1];
  const auto  ue_pucchs = test_helpers::find_ue_pucchs(slot_grid.result.ul.pucchs.unsorted(), ue.crnti);
  ASSERT_EQ(ue_pucchs.size(), 1);
  EXPECT_EQ(ue_pucchs[0]->uci_bits.harq_ack_nof_bits, 1U);
}

TEST_F(pucch_alloc_repetition_test, every_slot_of_the_burst_is_reported_as_barred_for_pusch)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-BW-17-1");

  const ue& ue = t_bench.get_main_ue();

  // As per TS 38.213, Section 9.2.6, a UE whose PUCCH with repetitions overlaps a PUSCH transmits the PUCCH and drops
  // the PUSCH in the overlapping slots; Section 9.2.5 scopes UCI multiplexing on PUSCH to PUCCHs over a single slot
  // without repetitions, so the UCI cannot be moved either. Every slot of the burst must therefore be reported to the
  // UL scheduler as unusable for a PUSCH of this UE, not just the anchor.
  ASSERT_TRUE(alloc_ded_harq_ack(ue, pucch_repetition_factor::n4).has_value());

  const slot_point anchor_slot = t_bench.res_grid[t_bench.k0 + default_k1].slot;
  for (unsigned i = 0; i != 4; ++i) {
    const slot_point             burst_slot = t_bench.res_grid[t_bench.k0 + default_k1 + i].slot;
    const span<const slot_point> rep_slots  = t_bench.pucch_alloc.get_pucch_repetition_slots(ue.crnti, burst_slot);
    EXPECT_EQ(rep_slots.size(), 4U) << "slot " << i << " of the burst was not reported as part of a repetition burst";
    if (not rep_slots.empty()) {
      // Every slot of the burst reports the same span of slots, in ascending order.
      EXPECT_EQ(rep_slots.front(), anchor_slot);
      EXPECT_EQ(rep_slots.back(), anchor_slot + 3);
    }
  }

  // The slot right after the burst is unaffected, and the check is per-UE: the burst must not bar a PUSCH of a UE that
  // holds no grant in those slots.
  EXPECT_TRUE(
      t_bench.pucch_alloc.get_pucch_repetition_slots(ue.crnti, t_bench.res_grid[t_bench.k0 + default_k1 + 4].slot)
          .empty());
  EXPECT_TRUE(t_bench.pucch_alloc.get_pucch_repetition_slots(to_rnti(0x4602), anchor_slot).empty());
}

TEST_F(pucch_alloc_repetition_test, single_slot_harq_ack_grant_is_not_reported_as_a_repetition_burst)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-BW-17-1");

  const ue& ue = t_bench.get_main_ue();

  // A plain single-slot grant can have its UCI multiplexed on a PUSCH as usual, so it must not bar the slot.
  ASSERT_TRUE(alloc_ded_harq_ack(ue).has_value());
  EXPECT_TRUE(
      t_bench.pucch_alloc.get_pucch_repetition_slots(ue.crnti, t_bench.res_grid[t_bench.k0 + default_k1].slot).empty());
}

TEST_F(pucch_alloc_repetition_test, additional_harq_ack_bit_is_propagated_to_every_slot_of_the_burst)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-BW-17-1");

  const ue& ue = t_bench.get_main_ue();

  const std::optional<unsigned> first_pri = alloc_ded_harq_ack(ue, pucch_repetition_factor::n4);
  ASSERT_TRUE(first_pri.has_value());

  // A second HARQ-ACK bit landing on the burst's anchor slot doesn't need other UCI multiplexing, so it must be
  // accepted and propagated to every slot of the burst, keeping the repeated payload identical across all of them.
  const std::optional<unsigned> second_pri = alloc_ded_harq_ack(ue, pucch_repetition_factor::n4);
  ASSERT_TRUE(second_pri.has_value());
  EXPECT_EQ(*second_pri, *first_pri);

  const unsigned anchor = t_bench.k0 + default_k1;
  expect_burst(t_bench, ue, {anchor, anchor + 1, anchor + 2, anchor + 3}, 2U);
}

// UE bench identical to pucch_alloc_repetition_test, but the UE also reports support for dynamic PUCCH repetition
// on Format 0/2 (Resource Set 1's format in this bench), so that promotion to Resource Set 1 can be exercised.
class pucch_alloc_repetition_promotion_test : public ::testing::Test, public pucch_allocator_base_test
{
public:
  pucch_alloc_repetition_promotion_test() :
    pucch_allocator_base_test(
        {.pucch_ded_params = make_rep_pucch_params(), .ue_caps = make_rep_supported_caps_with_f0_2()})
  {
  }
};

TEST_F(pucch_alloc_repetition_promotion_test, third_harq_ack_bit_promotes_the_whole_burst_to_resource_set_1)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-BW-17-1");

  const ue& ue = t_bench.get_main_ue();

  ASSERT_TRUE(alloc_ded_harq_ack(ue, pucch_repetition_factor::n4).has_value());
  ASSERT_TRUE(alloc_ded_harq_ack(ue, pucch_repetition_factor::n4).has_value());

  // The 3rd HARQ-ACK bit crosses the 2-bit threshold that requires Resource Set 1. The resource used so far is
  // fixed for the whole burst, so the allocator must move the burst over to a Resource Set 1 resource (configured
  // with the same repetition factor), rather than reject the extra bit or abort the burst.
  const std::optional<unsigned> third_pri = alloc_ded_harq_ack(ue, pucch_repetition_factor::n4);
  ASSERT_TRUE(third_pri.has_value());

  // Resource Set 1 uses Format 2 (the cell default), unlike Resource Set 0's Format 1.
  const unsigned anchor = t_bench.k0 + default_k1;
  expect_burst(t_bench, ue, {anchor, anchor + 1, anchor + 2, anchor + 3}, 3U, pucch_format::FORMAT_2);
}

TEST_F(pucch_alloc_repetition_test, promotion_falls_back_to_a_single_slot_grant_when_no_res_set_1_factor_fits)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-BW-17-1");

  const ue& ue = t_bench.get_main_ue();

  ASSERT_TRUE(alloc_ded_harq_ack(ue, pucch_repetition_factor::n4).has_value());
  ASSERT_TRUE(alloc_ded_harq_ack(ue, pucch_repetition_factor::n4).has_value());

  // This UE doesn't report support for repetition on Format 0/2, which is Resource Set 1's format in this bench, so no
  // Resource Set 1 resource can carry the burst with repetitions. The 3rd HARQ-ACK bit is still accepted, but the
  // burst is torn down in favour of a single-slot grant, allocated through the regular (non-repetition) path.
  const std::optional<unsigned> third_pri = alloc_ded_harq_ack(ue, pucch_repetition_factor::n4);
  ASSERT_TRUE(third_pri.has_value());

  const auto anchor_pucchs =
      test_helpers::find_ue_pucchs(t_bench.res_grid[t_bench.k0 + default_k1].result.ul.pucchs.unsorted(), ue.crnti);
  ASSERT_EQ(anchor_pucchs.size(), 1);
  EXPECT_EQ(anchor_pucchs[0]->uci_bits.harq_ack_nof_bits, 3U);
  EXPECT_EQ(anchor_pucchs[0]->format(), pucch_format::FORMAT_2);
  EXPECT_FALSE(anchor_pucchs[0]->repetition.has_value());

  // The rest of the original burst's slots must have been released.
  for (unsigned i = 1; i != 4; ++i) {
    const auto& slot_grid = t_bench.res_grid[t_bench.k0 + default_k1 + i];
    EXPECT_TRUE(test_helpers::find_ue_pucchs(slot_grid.result.ul.pucchs.unsorted(), ue.crnti).empty());
  }
}

TEST_F(pucch_alloc_repetition_test, ongoing_burst_is_kept_when_promotion_finds_no_usable_resource)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-BW-17-1");

  const ue& ue = t_bench.get_main_ue();

  ASSERT_TRUE(alloc_ded_harq_ack(ue, pucch_repetition_factor::n4).has_value());
  ASSERT_TRUE(alloc_ded_harq_ack(ue, pucch_repetition_factor::n4).has_value());

  // Fill the UL grid of the burst's anchor slot, so that no Resource Set 1 resource can be allocated there, either
  // with or without repetition. The 3rd HARQ-ACK bit must then be rejected, leaving the ongoing burst (and the 2 bits
  // already allocated to it) exactly as it was.
  t_bench.fill_all_grid(t_bench.res_grid[t_bench.k0 + default_k1].slot);

  EXPECT_FALSE(alloc_ded_harq_ack(ue, pucch_repetition_factor::n4).has_value());

  const unsigned anchor = t_bench.k0 + default_k1;
  expect_burst(t_bench, ue, {anchor, anchor + 1, anchor + 2, anchor + 3}, 2U, pucch_format::FORMAT_1);
}

// UE bench with the same cell-level repetition config, but no capability update applied to the UE — mirrors the
// point in production before the DU has learned the UE's reported capabilities.
class pucch_alloc_repetition_no_caps_test : public ::testing::Test, public pucch_allocator_base_test
{
public:
  pucch_alloc_repetition_no_caps_test() : pucch_allocator_base_test({.pucch_ded_params = make_rep_pucch_params()}) {}
};

TEST_F(pucch_alloc_repetition_no_caps_test, capability_unaware_ue_never_gets_repetition)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-BW-17-1");

  // pucch_resource_manager::alloc_resources() alone always clamps rep_factor to n1, regardless of the cell's
  // harq_ack_rep configuration, until update_resources() is called with the UE's reported capabilities.
  const ue& ue = t_bench.get_main_ue();

  const std::optional<unsigned> pri = alloc_ded_harq_ack(ue, pucch_repetition_factor::n4);
  ASSERT_TRUE(pri.has_value());

  const auto& anchor_grid   = t_bench.res_grid[t_bench.k0 + default_k1];
  const auto  anchor_pucchs = test_helpers::find_ue_pucchs(anchor_grid.result.ul.pucchs.unsorted(), ue.crnti);
  ASSERT_EQ(anchor_pucchs.size(), 1);
  EXPECT_FALSE(anchor_pucchs[0]->repetition.has_value());

  const auto& next_grid = t_bench.res_grid[t_bench.k0 + default_k1 + 1];
  EXPECT_TRUE(test_helpers::find_ue_pucchs(next_grid.result.ul.pucchs.unsorted(), ue.crnti).empty());
}

// UE bench with a TDD pattern (1 period == 10 slots at the 30 kHz reference SCS: 6 DL-only slots, 1 special slot
// with no UL symbols, and 3 full-UL slots), so that a burst spanning a DL-only gap can be exercised.
class pucch_alloc_repetition_tdd_test : public ::testing::Test, public pucch_allocator_base_test
{
public:
  pucch_alloc_repetition_tdd_test() :
    pucch_allocator_base_test(
        {.pucch_ded_params = make_rep_pucch_params(), .tdd = true, .ue_caps = make_rep_supported_caps()})
  {
  }
};

// Cell PUCCH resource params where only the last PRI of the HARQ-ACK resource sets is configured with n4. In the TDD
// bench below, that PRI's Resource Set 1 (Format 2) resource is the only one of the set whose symbols fit the special
// slot, whereas its Resource Set 0 (Format 1) resource spans the whole slot and doesn't.
static pucch_resource_builder_params make_last_pri_n4_pucch_params()
{
  pucch_resource_builder_params params        = make_rep_pucch_params(pucch_repetition_factor::n2);
  params.harq_ack_rep->factors_per_res.back() = pucch_repetition_factor::n4;
  return params;
}

// UE bench with a TDD pattern whose special slot has 4 UL symbols (i.e. symbols 10 to 13), so that it can host some
// of the (2-symbol) Format 2 resources of Resource Set 1, but none of the (14-symbol) Format 1 resources of Resource
// Set 0. The UE supports repetition on both format groups, so bursts of either resource set can be allocated.
class pucch_alloc_repetition_tdd_partial_ul_test : public ::testing::Test, public pucch_allocator_base_test
{
public:
  pucch_alloc_repetition_tdd_partial_ul_test() :
    pucch_allocator_base_test({.pucch_ded_params   = make_last_pri_n4_pucch_params(),
                               .tdd                = true,
                               .tdd_nof_ul_symbols = 4,
                               .ue_caps            = make_rep_supported_caps_with_f0_2(520000)})
  {
  }

protected:
  // Returns the UL symbols available for PUCCH in the slot at the given delay.
  ofdm_symbol_range ul_syms(unsigned delay) const
  {
    const unsigned nof_ul_syms = t_bench.cell_cfg.get_nof_ul_symbol_per_slot(t_bench.res_grid[delay].slot);
    return {NOF_OFDM_SYM_PER_SLOT_NORMAL_CP - nof_ul_syms, NOF_OFDM_SYM_PER_SLOT_NORMAL_CP};
  }

  // Returns the delays, within [first_delay, last_delay], of the slots holding a PUCCH grant for the given UE.
  std::vector<unsigned> find_grant_delays(const ue& ue, unsigned first_delay, unsigned last_delay) const
  {
    std::vector<unsigned> delays;
    for (unsigned delay = first_delay; delay <= last_delay; ++delay) {
      if (not test_helpers::find_ue_pucchs(t_bench.res_grid[delay].result.ul.pucchs.unsorted(), ue.crnti).empty()) {
        delays.push_back(delay);
      }
    }
    return delays;
  }
};

TEST_F(pucch_alloc_repetition_tdd_partial_ul_test, promotion_re_derives_the_burst_slots_for_the_new_resource)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-BW-17-1");

  const ue& ue = t_bench.get_main_ue();

  // Anchor on delay 7, the first full-UL slot of the TDD period.
  const unsigned anchor_delay = 7;
  auto           alloc_bit    = [&]() {
    return t_bench.pucch_alloc.alloc_ded_harq_ack(
        t_bench.res_grid, ue.get_pcell().cfg(), t_bench.k0, anchor_delay - t_bench.k0, pucch_repetition_factor::n4);
  };

  // Delay 16 is the special slot: it has UL symbols, but not enough for a Format 1 resource.
  ASSERT_TRUE(t_bench.cell_cfg.is_ul_enabled(t_bench.res_grid[16].slot));
  ASSERT_EQ(ul_syms(16), ofdm_symbol_range(10, 14));

  // The Resource Set 0 burst (Format 1, spanning all 14 symbols) cannot be transmitted in the special slot, so its 4
  // repetitions land on delays {7, 8, 9, 17}, skipping it.
  ASSERT_TRUE(alloc_bit().has_value());
  ASSERT_TRUE(alloc_bit().has_value());
  ASSERT_EQ(find_grant_delays(ue, anchor_delay, 19), std::vector<unsigned>({7, 8, 9, 17}));

  // The 3rd HARQ-ACK bit promotes the burst to Resource Set 1, whose resource for this PRI only spans symbols 10 to
  // 11 and therefore does fit the special slot. The burst's slots must be re-derived for it: the 4th repetition now
  // lands on the special slot (delay 16) instead of delay 17.
  ASSERT_TRUE(alloc_bit().has_value());
  ASSERT_EQ(find_grant_delays(ue, anchor_delay, 19), std::vector<unsigned>({7, 8, 9, 16}));

  const std::vector<unsigned> burst_delays = {7, 8, 9, 16};
  expect_burst(t_bench, ue, burst_delays, 3U, pucch_format::FORMAT_2);

  // Every repetition must fit in the UL symbols its slot actually has.
  for (unsigned delay : burst_delays) {
    const auto pucchs = test_helpers::find_ue_pucchs(t_bench.res_grid[delay].result.ul.pucchs.unsorted(), ue.crnti);
    ASSERT_EQ(pucchs.size(), 1);
    EXPECT_TRUE(ul_syms(delay).contains(pucchs[0]->res->syms));
  }
}

TEST_F(pucch_alloc_repetition_tdd_test, burst_skips_dl_only_slots_to_reach_the_next_ul_slot)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-BW-17-1");

  const ue& ue = t_bench.get_main_ue();

  // With this TDD pattern, only slot delays {7,8,9} and {17,18,19} (mod 10) are UL-enabled. Anchoring on delay 9
  // (the last UL slot of the first window) and requesting a 2-slot (n2) burst forces the allocator to skip the 7
  // DL-only slots (delays 10-16) and land the 2nd repetition on delay 17.
  const unsigned anchor_delay = 9;
  ASSERT_TRUE(t_bench.cell_cfg.is_ul_enabled(t_bench.res_grid[anchor_delay].slot));

  const std::optional<unsigned> pri = t_bench.pucch_alloc.alloc_ded_harq_ack(
      t_bench.res_grid, ue.get_pcell().cfg(), t_bench.k0, anchor_delay - t_bench.k0, pucch_repetition_factor::n2);
  ASSERT_TRUE(pri.has_value());

  // The anchor (delay 9) must start the burst.
  const auto& anchor_pucchs =
      test_helpers::find_ue_pucchs(t_bench.res_grid[anchor_delay].result.ul.pucchs.unsorted(), ue.crnti);
  ASSERT_EQ(anchor_pucchs.size(), 1);
  ASSERT_TRUE(anchor_pucchs[0]->repetition.has_value());
  EXPECT_EQ(anchor_pucchs[0]->repetition->position, pucch_repetition_tx_slot::starts);

  // The 7 DL-only slots in between must be left untouched by the burst.
  for (unsigned delay = anchor_delay + 1; delay != 17; ++delay) {
    ASSERT_FALSE(t_bench.cell_cfg.is_ul_enabled(t_bench.res_grid[delay].slot));
    EXPECT_TRUE(test_helpers::find_ue_pucchs(t_bench.res_grid[delay].result.ul.pucchs.unsorted(), ue.crnti).empty());
  }

  // Delay 17, the next UL-enabled slot, must end the burst.
  const auto& end_pucchs = test_helpers::find_ue_pucchs(t_bench.res_grid[17].result.ul.pucchs.unsorted(), ue.crnti);
  ASSERT_EQ(end_pucchs.size(), 1);
  ASSERT_TRUE(end_pucchs[0]->repetition.has_value());
  EXPECT_EQ(end_pucchs[0]->repetition->position, pucch_repetition_tx_slot::ends);

  // No PUCCH grant should leak into the UL slot right after the burst.
  EXPECT_TRUE(test_helpers::find_ue_pucchs(t_bench.res_grid[18].result.ul.pucchs.unsorted(), ue.crnti).empty());
}
