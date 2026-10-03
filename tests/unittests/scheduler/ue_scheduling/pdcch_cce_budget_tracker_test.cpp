// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/scheduler/cell/resource_grid.h"
#include "lib/scheduler/support/pdcch/pdcch_mapping.h"
#include "lib/scheduler/ue_scheduling/pdcch_cce_budget_tracker.h"
#include "tests/test_doubles/scheduler/cell_config_builder_profiles.h"
#include "tests/test_doubles/scheduler/scheduler_config_helper.h"
#include "tests/unittests/scheduler/test_utils/config_generators.h"
#include <gtest/gtest.h>
#include <numeric>

using namespace ocudu;

namespace {

struct table_test_params {
  unsigned                               min_k;
  std::optional<tdd_ul_dl_config_common> tdd_cfg;
};

const cell_configuration& make_cell_cfg(test_helpers::test_sched_config_manager& cfg_mng,
                                        unsigned                                 min_k,
                                        std::optional<tdd_ul_dl_config_common>   tdd_cfg)
{
  cell_config_builder_params params =
      cell_config_builder_profiles::create(tdd_cfg.has_value() ? duplex_mode::TDD : duplex_mode::FDD);
  params.tdd_ul_dl_cfg_common = tdd_cfg;
  params.min_k1               = min_k;
  params.min_k2               = min_k;
  params.auto_derive_params();
  return *cfg_mng.add_cell(sched_config_helper::make_default_sched_cell_configuration_request(params));
}

class nof_pusch_slots_per_pdcch_slot_test : public ::testing::TestWithParam<table_test_params>
{
protected:
  nof_pusch_slots_per_pdcch_slot_test() :
    cfg_mng(expert_cfg),
    cell_cfg(make_cell_cfg(cfg_mng, GetParam().min_k, GetParam().tdd_cfg)),
    table(compute_nof_pusch_slots_per_pdcch_slot(cell_cfg))
  {
  }

  scheduler_expert_config                 expert_cfg;
  test_helpers::test_sched_config_manager cfg_mng;
  const cell_configuration&               cell_cfg;
  std::vector<uint8_t>                    table;
};

TEST_P(nof_pusch_slots_per_pdcch_slot_test, table_matches_reachable_pusch_slots)
{
  if (not cell_cfg.is_tdd()) {
    ASSERT_EQ(table, std::vector<uint8_t>{1});
    return;
  }

  const tdd_ul_dl_config_common& tdd_cfg = *cell_cfg.params.tdd_cfg;
  ASSERT_EQ(table.size(), nof_slots_per_tdd_period(tdd_cfg));
  for (unsigned sl_idx = 0; sl_idx != table.size(); ++sl_idx) {
    if (not has_active_tdd_dl_symbols(tdd_cfg, sl_idx)) {
      ASSERT_EQ(table[sl_idx], 0) << "UL slot cannot carry a PDCCH";
    }
  }

  // Every full UL slot is reachable from at least one PDCCH slot.
  const unsigned nof_reachable = std::accumulate(table.begin(), table.end(), 0U);
  ASSERT_GE(nof_reachable, nof_full_ul_slots_per_tdd_period(tdd_cfg));

  const unsigned max_n = *std::max_element(table.begin(), table.end());
  if (nof_dl_slots_per_tdd_period(tdd_cfg) >= nof_full_ul_slots_per_tdd_period(tdd_cfg)) {
    // DL-heavy: each PDCCH slot schedules at most one PUSCH slot, so some DL slots cannot schedule UL at all.
    ASSERT_EQ(max_n, 1);
    ASSERT_NE(std::find(table.begin(), table.end(), 0), table.end());
  } else {
    // UL-heavy: at least one PDCCH slot schedules more than one PUSCH slot.
    ASSERT_GE(max_n, 2);
  }
}

INSTANTIATE_TEST_SUITE_P(
    pdcch_cce_budget_tracker_test,
    nof_pusch_slots_per_pdcch_slot_test,
    testing::Values(
        // clang-format off
        table_test_params{4, std::nullopt},
        table_test_params{4, tdd_ul_dl_config_common{subcarrier_spacing::kHz30, {5, 3, 9, 1, 0}}}, // DDDSU
        table_test_params{2, tdd_ul_dl_config_common{subcarrier_spacing::kHz30, {5, 3, 9, 1, 0}}}, // DDDSU
        table_test_params{4, tdd_ul_dl_config_common{subcarrier_spacing::kHz30, {10, 7, 5, 2, 4}}}, // DDDDDDDSUU
        table_test_params{2, tdd_ul_dl_config_common{subcarrier_spacing::kHz30, {5, 1, 10, 3, 0}}}, // DSUUU
        table_test_params{2, tdd_ul_dl_config_common{subcarrier_spacing::kHz30, {10, 3, 5, 6, 0}}}  // DDDSUUUUUU
        // clang-format on
        ));

/// Tests the CCE budgets of DL and UL on a resource grid whose PDCCHs are filled manually.
class pdcch_cce_budget_test : public ::testing::Test
{
protected:
  void setup(unsigned min_k, std::optional<tdd_ul_dl_config_common> tdd_cfg)
  {
    cell_cfg = &make_cell_cfg(cfg_mng, min_k, tdd_cfg);
    res_grid.emplace(*cell_cfg);
    distrib.emplace(*res_grid);
    table = compute_nof_pusch_slots_per_pdcch_slot(*cell_cfg);
    // The UE-dedicated SearchSpaces of the default config use a single CORESET.
    cs_cfg  = &cell_cfg->bwp_res[to_bwp_id(0)].dl().ded_pdcchs.front().coresets.front();
    cs0_cfg = &cell_cfg->params.dl_cfg_common.init_dl_bwp.pdcch_common.coreset0.value();
    next_sl = slot_point{cell_cfg->scs_common(), 0};
  }

  /// Advances to the next slot whose PDCCH can schedule the given number of PUSCH slots.
  void run_until_slot_with(unsigned nof_pusch_slots)
  {
    for (unsigned i = 0; i != 2 * table.size(); ++i) {
      const slot_point sl = next_sl++;
      res_grid->slot_indication(sl);
      distrib->slot_indication(sl);
      next_ncce.fill(0);
      if (table[sl.count() % table.size()] == nof_pusch_slots and cell_cfg->is_dl_enabled(sl)) {
        return;
      }
    }
    FAIL() << "No slot found with the requested nof. PUSCH slots";
  }

  /// Marks the CRBs of the next CCEs of the given CORESET as used in the resource grid.
  void fill_pdcch_crbs(const coreset_configuration& pdcch_cs_cfg, aggregation_level aggr_lvl)
  {
    const bwp_configuration& bwp_cfg = cell_cfg->params.dl_cfg_common.init_dl_bwp.generic_params;
    const unsigned           L       = to_nof_cces(aggr_lvl);
    unsigned&                ncce    = next_ncce[pdcch_cs_cfg.get_id()];
    ncce                             = (ncce + L - 1) / L * L;
    ASSERT_LE(ncce + L, pdcch_cs_cfg.get_nof_cces()) << "No more CCEs available in the CORESET";
    std::vector<uint16_t> crbs;
    for (uint16_t prb : pdcch_helper::cce_to_prb_mapping(bwp_cfg, pdcch_cs_cfg, cell_cfg->params.pci, aggr_lvl, ncce)) {
      crbs.push_back(prb_to_crb(bwp_cfg.crbs, prb));
    }
    (*res_grid)[0].dl_res_grid.fill(bwp_cfg.scs, {0, pdcch_cs_cfg.duration()}, crbs);
    ncce += L;
  }

  void add_dl_pdcch(aggregation_level aggr_lvl, const coreset_configuration* pdcch_cs_cfg = nullptr)
  {
    auto& pdcch             = (*res_grid)[0].result.dl.dl_pdcchs.emplace_back();
    pdcch.ctx.coreset_cfg   = pdcch_cs_cfg != nullptr ? pdcch_cs_cfg : cs_cfg;
    pdcch.ctx.cces.aggr_lvl = aggr_lvl;
    fill_pdcch_crbs(*pdcch.ctx.coreset_cfg, aggr_lvl);
  }

  void add_ul_pdcch(aggregation_level aggr_lvl)
  {
    auto& pdcch             = (*res_grid)[0].result.dl.ul_pdcchs.emplace_back();
    pdcch.ctx.coreset_cfg   = cs_cfg;
    pdcch.ctx.cces.aggr_lvl = aggr_lvl;
    fill_pdcch_crbs(*cs_cfg, aggr_lvl);
  }

  slot_point pusch_slot(unsigned k) const { return (*res_grid)[0].slot + k; }

  scheduler_expert_config                 expert_cfg;
  test_helpers::test_sched_config_manager cfg_mng{expert_cfg};
  const cell_configuration*               cell_cfg = nullptr;
  std::optional<cell_resource_allocator>  res_grid;
  std::optional<pdcch_cce_budget_tracker> distrib;
  std::vector<uint8_t>                    table;
  const coreset_configuration*            cs_cfg  = nullptr;
  const coreset_configuration*            cs0_cfg = nullptr;
  slot_point                              next_sl;
  // Next free CCE of each CORESET in the current slot.
  std::array<unsigned, MAX_NOF_CORESETS> next_ncce{};
};

TEST_F(pdcch_cce_budget_test, when_fdd_then_dl_and_ul_get_half_of_the_cces)
{
  setup(4, std::nullopt);
  run_until_slot_with(1);
  const unsigned total = cs_cfg->get_nof_cces();

  ASSERT_EQ(distrib->remaining_dl_cces(), total / 2);
  while (distrib->remaining_dl_cces() > 0) {
    add_dl_pdcch(aggregation_level::n1);
  }
  ASSERT_EQ(distrib->remaining_ul_cces(pusch_slot(4)), total - total / 2);
}

TEST_F(pdcch_cce_budget_test, when_pdcch_slot_cannot_schedule_pusch_then_dl_gets_all_cces)
{
  setup(4, tdd_ul_dl_config_common{subcarrier_spacing::kHz30, {5, 3, 9, 1, 0}});
  run_until_slot_with(0);
  const unsigned total = cs_cfg->get_nof_cces();

  ASSERT_EQ(distrib->remaining_dl_cces(), total);
  add_dl_pdcch(aggregation_level::n4);
  ASSERT_EQ(distrib->remaining_dl_cces(), total - 4);
}

TEST_F(pdcch_cce_budget_test, when_dl_does_not_use_its_share_then_ul_gets_the_leftover)
{
  setup(4, tdd_ul_dl_config_common{subcarrier_spacing::kHz30, {5, 3, 9, 1, 0}});
  run_until_slot_with(1);
  const unsigned total = cs_cfg->get_nof_cces();
  const unsigned share = total / 2;

  // DL is capped at its share.
  add_dl_pdcch(aggregation_level::n2);
  ASSERT_EQ(distrib->remaining_dl_cces(), share - 2);

  // UL gets its share plus what DL left unused.
  const slot_point ul_slot = pusch_slot(4);
  ASSERT_EQ(distrib->remaining_ul_cces(ul_slot), std::min(share + (share - 2), total - 2));
  add_ul_pdcch(aggregation_level::n4);
  ASSERT_EQ(distrib->remaining_ul_cces(ul_slot), std::min(share + (share - 2), total - 2) - 4);
}

TEST_F(pdcch_cce_budget_test, when_pdcch_slot_schedules_multiple_pusch_slots_then_later_ones_keep_their_share)
{
  setup(2, tdd_ul_dl_config_common{subcarrier_spacing::kHz30, {5, 1, 10, 3, 0}});
  const uint8_t n = *std::max_element(table.begin(), table.end());
  ASSERT_GE(n, 2);
  run_until_slot_with(n);
  const unsigned total = cs_cfg->get_nof_cces();
  const unsigned share = total / (n + 1);

  // DL uses its full share, so there is no leftover.
  while (distrib->remaining_dl_cces() > 0) {
    add_dl_pdcch(aggregation_level::n1);
  }
  const unsigned free_cces = n * share;
  ASSERT_EQ(distrib->remaining_ul_cces(pusch_slot(2)), free_cces - share);

  // Exhausting the budget of the first PUSCH slot leaves the share of the second one.
  for (unsigned i = 0; i != free_cces - share; ++i) {
    add_ul_pdcch(aggregation_level::n1);
  }
  ASSERT_EQ(distrib->remaining_ul_cces(pusch_slot(2)), 0);
  ASSERT_EQ(distrib->remaining_ul_cces(pusch_slot(3)), share);
}

TEST_F(pdcch_cce_budget_test, pdcchs_of_overlapping_coresets_consume_budget)
{
  setup(4, tdd_ul_dl_config_common{subcarrier_spacing::kHz30, {5, 3, 9, 1, 0}});
  ASSERT_NE(cs_cfg->get_id(), cs0_cfg->get_id());
  run_until_slot_with(0);
  const unsigned total = cs_cfg->get_nof_cces();
  ASSERT_EQ(distrib->remaining_dl_cces(), total);

  // In the default config, the UE-dedicated CORESET overlaps CORESET#0 in frequency.
  add_dl_pdcch(aggregation_level::n4, cs0_cfg);
  ASSERT_LT(distrib->remaining_dl_cces(), total);
}

TEST_F(pdcch_cce_budget_test, when_all_shares_are_used_then_no_cce_is_left_unused)
{
  setup(2, tdd_ul_dl_config_common{subcarrier_spacing::kHz30, {5, 1, 10, 3, 0}});
  const uint8_t n = *std::max_element(table.begin(), table.end());
  run_until_slot_with(n);
  const unsigned total = cs_cfg->get_nof_cces();
  const unsigned share = total / (n + 1);

  // DL gets its share plus the remainder of the division.
  const unsigned dl_cces = distrib->remaining_dl_cces();
  ASSERT_EQ(dl_cces, total - n * share);
  for (unsigned i = 0; i != dl_cces; ++i) {
    add_dl_pdcch(aggregation_level::n1);
  }

  // Each PUSCH slot but the last one keeps the shares of the PUSCH slots yet to be scheduled.
  unsigned used = dl_cces;
  for (unsigned k = 0; k != n; ++k) {
    const unsigned rem = distrib->remaining_ul_cces(pusch_slot(2 + k));
    ASSERT_EQ(rem, total - used - (n - 1 - k) * share);
    for (unsigned i = 0; i != rem; ++i) {
      add_ul_pdcch(aggregation_level::n1);
    }
    used += rem;
  }
  ASSERT_EQ(used, total);
}

TEST_F(pdcch_cce_budget_test, when_earlier_pusch_slots_are_skipped_then_later_pusch_slot_uses_their_share)
{
  setup(2, tdd_ul_dl_config_common{subcarrier_spacing::kHz30, {5, 1, 10, 3, 0}});
  const uint8_t n = *std::max_element(table.begin(), table.end());
  ASSERT_GE(n, 2);
  run_until_slot_with(n);
  const unsigned total = cs_cfg->get_nof_cces();
  const unsigned share = total / (n + 1);

  // The first PUSCH slot keeps the shares of the later PUSCH slots, while the last PUSCH slot can use all CCEs.
  ASSERT_EQ(distrib->remaining_ul_cces(pusch_slot(2)), total - (n - 1) * share);
  ASSERT_EQ(distrib->remaining_ul_cces(pusch_slot(2 + n - 1)), total);
}

} // namespace
