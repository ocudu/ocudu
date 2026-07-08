// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "include/ocudu/scheduler/config/pusch_td_resource_indices.h"
#include "lib/scheduler/config/cell_configuration.h"
#include "lib/scheduler/support/pusch/pusch_default_time_allocation.h"
#include "tests/test_doubles/scheduler/cell_config_builder_profiles.h"
#include "tests/test_doubles/scheduler/scheduler_config_helper.h"
#include "tests/unittests/scheduler/test_utils/config_generators.h"
#include "ocudu/ran/tdd/tdd_ul_dl_config_formatters.h"
#include "ocudu/scheduler/config/cell_config_builder_params.h"
#include "ocudu/support/format/custom_formattable.h"
#include "fmt/std.h"
#include <gtest/gtest.h>
#include <ostream>

using namespace ocudu;

namespace {

struct test_params {
  unsigned min_k = 4;
  /// If present duplex mode is TDD, FDD otherwise.
  std::optional<tdd_ul_dl_config_common> tdd_cfg;
};

class pusch_td_resource_indices_test : public ::testing::TestWithParam<test_params>
{
protected:
  pusch_td_resource_indices_test() : cfg_mng(expert_cfg)
  {
    test_params testparams = GetParam();
    params = cell_config_builder_profiles::create(testparams.tdd_cfg.has_value() ? duplex_mode::TDD : duplex_mode::FDD);
    params.tdd_ul_dl_cfg_common = testparams.tdd_cfg;
    params.min_k1               = testparams.min_k;
    params.min_k2               = testparams.min_k;
    params.auto_derive_params();

    sched_cell_configuration_request_message sched_cell_cfg_req =
        sched_config_helper::make_default_sched_cell_configuration_request(params);

    // Generate cell configuration.
    cell_cfg = cfg_mng.add_cell(sched_cell_cfg_req);

    // Generate the list to verify.
    ocudu_assert(cell_cfg->params.ul_cfg_common.init_ul_bwp.pusch_cfg_common.has_value(),
                 "PUSCH Config Common expected");
    pusch_td_res_indxes_list_per_slot = get_pusch_td_resource_indices_per_slot(
        cell_cfg->scs_common(),
        cell_cfg->params.tdd_cfg,
        cell_cfg->params.ul_cfg_common.init_ul_bwp.pusch_cfg_common.value().pusch_td_alloc_list,
        cell_cfg->dl_data_to_ul_ack.front());

    // Populate slot indexes.
    if (cell_cfg->is_tdd()) {
      for (unsigned slot_idx = 0, e = nof_slots_per_tdd_period(*cell_cfg->params.tdd_cfg); slot_idx != e; ++slot_idx) {
        if (has_active_tdd_dl_symbols(*cell_cfg->params.tdd_cfg, slot_idx)) {
          dl_slot_indexes.push_back(slot_idx);
        }
        if (is_tdd_full_ul_slot(*cell_cfg->params.tdd_cfg, slot_idx)) {
          ul_slot_indexes.push_back(slot_idx);
        }
      }
    } else {
      // NOTE: [Implementation-defined] In case of FDD, we consider only one slot as all slots are similar unlike in TDD
      // where there can be DL/UL full or partial slots.
      dl_slot_indexes.push_back(0);
      ul_slot_indexes.push_back(params.min_k2);
    }
  }

  scheduler_expert_config    expert_cfg;
  cell_config_builder_params params{};

  test_helpers::test_sched_config_manager cfg_mng;
  const cell_configuration*               cell_cfg = nullptr;
  // Contains slot indexes of the DL enabled slots.
  std::vector<unsigned> dl_slot_indexes;
  // Contains slot indexes of fully UL enabled slots.
  std::vector<unsigned> ul_slot_indexes;

  // List circularly indexed by slot with the list of applicable PUSCH Time Domain resource indexes per slot.
  std::vector<static_vector<uint8_t, pusch_constants::MAX_NOF_PUSCH_TD_RES_ALLOCS>> pusch_td_res_indxes_list_per_slot;
};

// This test is only for DL-heavy TDD pattern.
TEST_P(pusch_td_resource_indices_test, in_dl_heavy_tdd_dl_to_ul_index_ordering_is_verified)
{
  // Fetch the relevant PUSCH time domain resource list.
  span<const pusch_time_domain_resource_allocation> pusch_time_domain_list =
      get_c_rnti_pusch_time_domain_list(true, to_coreset_id(1), cell_cfg->init_bwp);

  if (not GetParam().tdd_cfg.has_value()) {
    GTEST_SKIP();
  }

  const unsigned nof_dl_slots      = nof_dl_slots_per_tdd_period(cell_cfg->params.tdd_cfg.value());
  const unsigned nof_full_ul_slots = nof_full_ul_slots_per_tdd_period(cell_cfg->params.tdd_cfg.value());
  const unsigned nof_slots         = nof_slots_per_tdd_period(cell_cfg->params.tdd_cfg.value());

  // Return the UL index given DL index and its corresponding k2.
  // NOTE: we don't apply any mod operation on purpose, as the mod is applied to by function caller.
  auto get_ul_slot_index = [&pusch_time_domain_list, this](unsigned dl_sl_idx) {
    return dl_sl_idx + pusch_time_domain_list[pusch_td_res_indxes_list_per_slot[dl_sl_idx].front()].k2;
  };

  if (nof_dl_slots < nof_full_ul_slots) {
    GTEST_SKIP();
  }

  for (unsigned dl_idx_n = 0; dl_idx_n != nof_slots - 1; ++dl_idx_n) {
    const auto& dl_idx_n_list = pusch_td_res_indxes_list_per_slot[dl_idx_n];
    ASSERT_LE(dl_idx_n_list.size(), 1);
    if (dl_idx_n_list.empty()) {
      continue;
    }
    const unsigned ul_idx_n = get_ul_slot_index(dl_idx_n);
    for (unsigned dl_idx_k = dl_idx_n + 1; dl_idx_k != nof_slots; ++dl_idx_k) {
      const auto& dl_idx_k_list = pusch_td_res_indxes_list_per_slot[dl_idx_k];
      ASSERT_LE(dl_idx_k_list.size(), 1);
      if (dl_idx_k_list.empty()) {
        continue;
      }
      const unsigned ul_idx_k = get_ul_slot_index(dl_idx_k);
      // The smaller UL index >= TDD period.
      if (ul_idx_n >= nof_slots) {
        ASSERT_GT(ul_idx_k, nof_slots);
        ASSERT_LT(ul_idx_n, ul_idx_k);
      }
      // In the next cases, UL index < TDD period.
      else if (ul_idx_k >= nof_slots) {
        ASSERT_LT(ul_idx_k % nof_slots, ul_idx_n);
      } else {
        ASSERT_LT(ul_idx_n, ul_idx_k);
      }
    }
  }
}

TEST_P(pusch_td_resource_indices_test, all_ul_slots_have_one_pdcch_slot_to_schedule_pusch)
{
  // Fetch the relevant PUSCH time domain resource list.
  span<const pusch_time_domain_resource_allocation> pusch_time_domain_list =
      get_c_rnti_pusch_time_domain_list(false, to_coreset_id(1), cell_cfg->init_bwp);

  unsigned slot_mod = cell_cfg->is_tdd() ? nof_slots_per_tdd_period(*cell_cfg->params.tdd_cfg) : SCHEDULER_MAX_K2;

  auto ul_slot_idx_it = ul_slot_indexes.begin();
  while (ul_slot_idx_it != ul_slot_indexes.end()) {
    bool pdcch_slot_found = false;
    for (const auto dl_slot_idx : dl_slot_indexes) {
      span<const uint8_t> pusch_td_res_indxes_list = pusch_td_res_indxes_list_per_slot[dl_slot_idx];
      pdcch_slot_found =
          std::any_of(pusch_td_res_indxes_list.begin(),
                      pusch_td_res_indxes_list.end(),
                      [&pusch_time_domain_list, dl_slot_idx, ul_slot_idx = *ul_slot_idx_it, slot_mod](
                          const unsigned pusch_td_res_idx) {
                        return ul_slot_idx == (dl_slot_idx + pusch_time_domain_list[pusch_td_res_idx].k2) % slot_mod;
                      });
      if (pdcch_slot_found) {
        break;
      }
    }
    ASSERT_TRUE(pdcch_slot_found) << fmt::format("No PDCCH slot found for UL slot={}", *ul_slot_idx_it);
    ul_slot_idx_it = ul_slot_indexes.erase(ul_slot_idx_it);
  }
}

TEST_P(pusch_td_resource_indices_test, pusch_td_resources_are_fairly_distributed_across_pdcch_slots)
{
  // For FDD and DL heavy TDD pattern, only one UL PDCCH is allowed per PDCCH slot.
  auto est_max_nof_ul_pdcchs_per_dl_slot = 1;
  if (cell_cfg->is_tdd() and ul_slot_indexes.size() > dl_slot_indexes.size()) {
    // Estimate the nof. UL PDCCHs that can be scheduled in each PDCCH slot in UL heavy TDD pattern.
    est_max_nof_ul_pdcchs_per_dl_slot = static_cast<unsigned>(
        std::round(static_cast<double>(ul_slot_indexes.size()) / static_cast<double>(dl_slot_indexes.size())));
    // Adjustment for corner case such as DDSUUUUUUU, where there are uneven nof. DL and UL slots resulting in one DL
    // slot having to schedule more than previously estimated nof. nof. UL PDCCHs per PDCCH slot.
    if ((est_max_nof_ul_pdcchs_per_dl_slot * dl_slot_indexes.size()) < ul_slot_indexes.size()) {
      est_max_nof_ul_pdcchs_per_dl_slot +=
          (ul_slot_indexes.size() - (est_max_nof_ul_pdcchs_per_dl_slot * dl_slot_indexes.size()));
    }
  };

  for (const auto dl_slot_idx : dl_slot_indexes) {
    ASSERT_LE(pusch_td_res_indxes_list_per_slot[dl_slot_idx].size(), est_max_nof_ul_pdcchs_per_dl_slot)
        << fmt::format("Nof. PUSCH TD resource indices for PDCCH slot={} exceeds max. value={}",
                       dl_slot_idx,
                       est_max_nof_ul_pdcchs_per_dl_slot);
  }
}

// For DL-heavy TDD:
// - each UL slot must be targeted by exactly 1 DL slot (via its assigned k2);
// - each DL slot's resource list must use a single k2 value (entries may differ only in OFDM symbol range).
TEST_P(pusch_td_resource_indices_test, dl_heavy_tdd_ul_slot_coverage_and_unique_k2_per_dl_slot)
{
  // Only runs for DL-heavy TDD.
  if (not GetParam().tdd_cfg.has_value()) {
    GTEST_SKIP();
  }
  const tdd_ul_dl_config_common& tdd_cfg           = GetParam().tdd_cfg.value();
  const unsigned                 nof_dl_slots      = nof_dl_slots_per_tdd_period(tdd_cfg);
  const unsigned                 nof_full_ul_slots = nof_full_ul_slots_per_tdd_period(tdd_cfg);
  if (nof_dl_slots < nof_full_ul_slots) {
    GTEST_SKIP();
  }

  const unsigned                                    nof_slots = nof_slots_per_tdd_period(tdd_cfg);
  span<const pusch_time_domain_resource_allocation> pusch_td_list =
      cell_cfg->params.ul_cfg_common.init_ul_bwp.pusch_cfg_common.value().pusch_td_alloc_list;

  // (iii) Every non-empty DL slot must use a single k2 across all its TD resource entries (entries may differ in OFDM
  // symbol range, but must target the same UL slot).
  for (unsigned dl_idx : dl_slot_indexes) {
    const auto& td_res_indices = pusch_td_res_indxes_list_per_slot[dl_idx];
    if (td_res_indices.empty()) {
      continue;
    }
    const unsigned expected_k2 = pusch_td_list[td_res_indices.front()].k2;
    for (unsigned res_idx : td_res_indices) {
      EXPECT_EQ(pusch_td_list[res_idx].k2, expected_k2)
          << fmt::format("DL slot={} has TD resource entries with different k2 values", dl_idx);
    }
  }

  // (ii) Every full UL slot must be covered by exactly 1 DL slot.
  for (unsigned ul_idx : ul_slot_indexes) {
    unsigned nof_covering_dl_slots = 0;
    for (unsigned dl_idx : dl_slot_indexes) {
      const auto& td_res_indices = pusch_td_res_indxes_list_per_slot[dl_idx];
      const bool  covers_this_ul = std::any_of(td_res_indices.begin(), td_res_indices.end(), [&](unsigned res_idx) {
        return (dl_idx + pusch_td_list[res_idx].k2) % nof_slots == ul_idx;
      });
      if (covers_this_ul) {
        ++nof_covering_dl_slots;
      }
    }
    EXPECT_EQ(nof_covering_dl_slots, 1u) << fmt::format(
        "UL slot={} is covered by {} DL slot(s), expected exactly 1", ul_idx, nof_covering_dl_slots);
  }
}

/// Formatter for test params.
void PrintTo(const test_params& value, ::std::ostream* os)
{
  *os << fmt::format("min_k={} {}{}",
                     value.min_k,
                     value.tdd_cfg.has_value() ? "tdd" : "fdd",
                     add_prefix_if_set(" pattern=", value.tdd_cfg));
}

} // namespace

INSTANTIATE_TEST_SUITE_P(
    pusch_td_resource_indices_test,
    pusch_td_resource_indices_test,
    testing::Values(
        // clang-format off
        // min_k, {ref_scs, pattern1={slot_period, DL_slots, DL_symbols, UL_slots, UL_symbols}, pattern2={...}}
         // FDD
        test_params{4, {}},
        test_params{3, {}},
        test_params{2, {}},
        // TDD - DL Heavy
        test_params{4, tdd_ul_dl_config_common{subcarrier_spacing::kHz30, {10, 6, 5, 3, 4}}}, // DDDDDDSUUU
        test_params{3, tdd_ul_dl_config_common{subcarrier_spacing::kHz30, {10, 6, 5, 3, 4}}}, // DDDDDDSUUU
        test_params{2, tdd_ul_dl_config_common{subcarrier_spacing::kHz30, {10, 6, 5, 3, 4}}}, // DDDDDDSUUU
        test_params{4, tdd_ul_dl_config_common{subcarrier_spacing::kHz30, {10, 7, 5, 2, 4}}}, // DDDDDDDSUU
        test_params{3, tdd_ul_dl_config_common{subcarrier_spacing::kHz30, {10, 7, 5, 2, 4}}}, // DDDDDDDSUU
        test_params{2, tdd_ul_dl_config_common{subcarrier_spacing::kHz30, {10, 7, 5, 2, 4}}}, // DDDDDDDSUU
        test_params{2, tdd_ul_dl_config_common{subcarrier_spacing::kHz30, {10, 8, 5, 1, 4}}}, // DDDDDDDDSU
        test_params{4, tdd_ul_dl_config_common{subcarrier_spacing::kHz30, {6, 3, 5, 2, 0}, tdd_ul_dl_pattern{4, 4, 0, 0, 0}}}, // DDDSUUDDDD
        test_params{3, tdd_ul_dl_config_common{subcarrier_spacing::kHz30, {6, 3, 5, 2, 0}, tdd_ul_dl_pattern{4, 4, 0, 0, 0}}}, // DDDSUUDDDD
        test_params{2, tdd_ul_dl_config_common{subcarrier_spacing::kHz30, {6, 3, 5, 2, 0}, tdd_ul_dl_pattern{4, 4, 0, 0, 0}}}, // DDDSUUDDDD
        test_params{4, tdd_ul_dl_config_common{subcarrier_spacing::kHz30, {4, 2, 9, 1, 0}}}, // DDSU
        test_params{3, tdd_ul_dl_config_common{subcarrier_spacing::kHz30, {4, 2, 9, 1, 0}}}, // DDSU
        test_params{2, tdd_ul_dl_config_common{subcarrier_spacing::kHz30, {4, 2, 9, 1, 0}}}, // DDSU
        test_params{5, tdd_ul_dl_config_common{subcarrier_spacing::kHz30, {5, 3, 9, 1, 0}}}, // DDDSU
        test_params{4, tdd_ul_dl_config_common{subcarrier_spacing::kHz30, {5, 3, 9, 1, 0}}}, // DDDSU
        test_params{3, tdd_ul_dl_config_common{subcarrier_spacing::kHz30, {5, 3, 9, 1, 0}}}, // DDDSU
        test_params{2, tdd_ul_dl_config_common{subcarrier_spacing::kHz30, {5, 3, 9, 1, 0}}}, // DDDSU
        test_params{5, tdd_ul_dl_config_common{subcarrier_spacing::kHz30, {10, 4, 5, 5, 0}}}, // DDDDSUUUUU
        test_params{4, tdd_ul_dl_config_common{subcarrier_spacing::kHz30, {10, 4, 5, 5, 0}}}, // DDDDSUUUUU
        test_params{3, tdd_ul_dl_config_common{subcarrier_spacing::kHz30, {10, 4, 5, 5, 0}}}, // DDDDSUUUUU
        test_params{5, tdd_ul_dl_config_common{subcarrier_spacing::kHz30,{6, 3, 8, 2, 4}, tdd_ul_dl_pattern{4, 1, 8, 2, 4}}}, // DDDSUUDSUU
        test_params{4, tdd_ul_dl_config_common{subcarrier_spacing::kHz30,{6, 3, 8, 2, 4}, tdd_ul_dl_pattern{4, 1, 8, 2, 4}}}, // DDDSUUDSUU
        test_params{3, tdd_ul_dl_config_common{subcarrier_spacing::kHz30,{6, 3, 8, 2, 4}, tdd_ul_dl_pattern{4, 1, 8, 2, 4}}}, // DDDSUUDSUU
        test_params{2, tdd_ul_dl_config_common{subcarrier_spacing::kHz30,{6, 3, 8, 2, 4}, tdd_ul_dl_pattern{4, 1, 8, 2, 4}}}, // DDDSUUDSUU
        test_params{5, tdd_ul_dl_config_common{subcarrier_spacing::kHz30,{5, 3, 8, 1, 4}, tdd_ul_dl_pattern{5, 1, 8, 3, 4}}}, // DDDSUDSUUU
        test_params{4, tdd_ul_dl_config_common{subcarrier_spacing::kHz30,{5, 3, 8, 1, 4}, tdd_ul_dl_pattern{5, 1, 8, 3, 4}}}, // DDDSUDSUUU
        test_params{3, tdd_ul_dl_config_common{subcarrier_spacing::kHz30,{5, 3, 8, 1, 4}, tdd_ul_dl_pattern{5, 1, 8, 3, 4}}}, // DDDSUDSUUU
        test_params{2, tdd_ul_dl_config_common{subcarrier_spacing::kHz30,{5, 3, 8, 1, 4}, tdd_ul_dl_pattern{5, 1, 8, 3, 4}}}, // DDDSUDSUUU
        test_params{4, tdd_ul_dl_config_common{subcarrier_spacing::kHz30,{5, 3, 8, 1, 4}, tdd_ul_dl_pattern{5, 1, 0, 3, 4}}}, // DDDSUDXUUU
        test_params{3, tdd_ul_dl_config_common{subcarrier_spacing::kHz30,{5, 3, 8, 1, 4}, tdd_ul_dl_pattern{5, 1, 0, 3, 4}}}, // DDDSUDXUUU
        test_params{5, tdd_ul_dl_config_common{subcarrier_spacing::kHz30,{4, 1, 8, 2, 4}, tdd_ul_dl_pattern{6, 3, 8, 2, 4}}}, // DSUUDDDSUU
        test_params{4, tdd_ul_dl_config_common{subcarrier_spacing::kHz30,{4, 1, 8, 2, 4}, tdd_ul_dl_pattern{6, 3, 8, 2, 4}}}, // DSUUDDDSUU
        test_params{3, tdd_ul_dl_config_common{subcarrier_spacing::kHz30,{4, 1, 8, 2, 4}, tdd_ul_dl_pattern{6, 3, 8, 2, 4}}}, // DSUUDDDSUU
        test_params{2, tdd_ul_dl_config_common{subcarrier_spacing::kHz30,{4, 1, 8, 2, 4}, tdd_ul_dl_pattern{6, 3, 8, 2, 4}}}, // DSUUDDDSUU
        test_params{4, tdd_ul_dl_config_common{subcarrier_spacing::kHz30,{15, 13, 8, 1, 4}, tdd_ul_dl_pattern{5, 1, 0, 3, 4}}}, // 13DSUDSUUU
        // TDD - UL Heavy
        test_params{2, tdd_ul_dl_config_common{subcarrier_spacing::kHz30, {10, 3, 5, 6, 0}}}, // DDDSUUUUUU
        test_params{2, tdd_ul_dl_config_common{subcarrier_spacing::kHz30, {10, 2, 10, 7, 0}}}, // DDSUUUUUUU
        test_params{2, tdd_ul_dl_config_common{subcarrier_spacing::kHz30, {5, 1, 10, 3, 0}}}, // DSUUU
        test_params{2, tdd_ul_dl_config_common{subcarrier_spacing::kHz30, {5, 1, 10, 3, 0}, tdd_ul_dl_pattern{5, 1, 10, 3, 0}}}, // DSUUUDSUUU
        test_params{2, tdd_ul_dl_config_common{subcarrier_spacing::kHz30, {6, 2, 10, 3, 0}, tdd_ul_dl_pattern{4, 1, 0, 3, 0}}}, // DDSUUUDUUU
        test_params{2, tdd_ul_dl_config_common{subcarrier_spacing::kHz30, {4, 1, 10, 2, 0}, tdd_ul_dl_pattern{6, 1, 10, 4, 0}}} // DSUUDSUUUU

        // clang-format on
        ));
