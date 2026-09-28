// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief Unit tests for the Configured Grant resource helpers of ran_cell_config_helper.

#include "tests/ocudu_test_requirements.h"
#include "tests/test_doubles/scheduler/cell_config_builder_profiles.h"
#include "ocudu/du/du_cell_config_helpers.h"
#include "ocudu/ran/prach/prach_time_mapping.h"
#include "ocudu/ran/tdd/tdd_ul_dl_config.h"
#include "ocudu/scheduler/config/cg_builder_params.h"
#include "ocudu/scheduler/config/ran_cell_config_helper.h"
#include "ocudu/scheduler/config/time_domain_resource_helper.h"
#include <gtest/gtest.h>
#include <numeric>

using namespace ocudu;
using namespace odu;

namespace {

// ---- Helpers ----

// Builds a cell with CG enabled. \c nof_ul_slots, when set, makes it TDD with that many full-UL slots in a 10-slot
// pattern; otherwise the cell is FDD.
ran_cell_config make_cg_cell_cfg(std::optional<unsigned> nof_ul_slots, cg_configuration::periodicity_t cg_period)
{
  const bool                 is_tdd = nof_ul_slots.has_value();
  cell_config_builder_params cell_params =
      cell_config_builder_profiles::create(is_tdd ? duplex_mode::TDD : duplex_mode::FDD);
  if (is_tdd) {
    auto&              tdd_cfg    = cell_params.tdd_ul_dl_cfg_common.emplace();
    const unsigned     nof_ul_sl  = nof_ul_slots.value();
    constexpr unsigned nof_ul_sym = 0U;
    const unsigned     nof_dl_sl  = 10U - nof_ul_sl - 1U;
    constexpr unsigned nof_dl_sym = 10U;
    tdd_cfg.pattern1              = {nof_dl_sl + 1U + nof_ul_sl, nof_dl_sl, nof_dl_sym, nof_ul_sl, nof_ul_sym};
    tdd_cfg.ref_scs               = subcarrier_spacing::kHz30;
  }

  ran_cell_config cell_cfg = config_helpers::make_default_du_cell_config(cell_params).ran;
  cell_cfg.init_bwp.cg_cfg = cg_builder_params{.periodicity = cg_period, .type = cg_builder_params::cg_type::type2};
  return cell_cfg;
}

subcarrier_spacing get_ul_scs(const ran_cell_config& cell_cfg)
{
  return cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.scs;
}

prach_helper::preamble_slot_mapping make_prach_mapper(const ran_cell_config& cell_cfg)
{
  return {cell_cfg.dl_carrier.band,
          get_ul_scs(cell_cfg),
          cell_cfg.ul_cfg_common.init_ul_bwp.rach_cfg_common.value().rach_cfg_generic.prach_config_index};
}

unsigned prach_period_slots(const ran_cell_config& cell_cfg)
{
  return get_nof_slots_per_subframe(get_ul_scs(cell_cfg)) * static_cast<unsigned>(NOF_SUBFRAMES_PER_FRAME) *
         make_prach_mapper(cell_cfg).sfn_period();
}

unsigned cg_period_slots(const ran_cell_config& cell_cfg)
{
  return static_cast<unsigned>(cell_cfg.init_bwp.cg_cfg.value().periodicity.value());
}

// Period after which the CG, TDD and PRACH patterns all realign, i.e. the window holding every distinct slot that a CG
// offset can land on.
unsigned fold_period_slots(const ran_cell_config& cell_cfg)
{
  const unsigned tdd_period = cell_cfg.tdd_cfg.has_value() ? nof_slots_per_tdd_period(cell_cfg.tdd_cfg.value()) : 1U;
  return std::lcm(std::lcm(prach_period_slots(cell_cfg), cg_period_slots(cell_cfg)), tdd_period);
}

bool is_ul_slot(const ran_cell_config& cell_cfg, unsigned slot)
{
  return not cell_cfg.tdd_cfg.has_value() or is_tdd_full_ul_slot(cell_cfg.tdd_cfg.value(), slot);
}

bool is_prach_slot(const ran_cell_config& cell_cfg, unsigned slot)
{
  return make_prach_mapper(cell_cfg).has_prach_occasion(
      slot_point(get_ul_scs(cell_cfg), slot % prach_period_slots(cell_cfg)));
}

// ---- Test parameters ----

struct cg_offsets_test_params {
  // If set, it's TDD and the value is the number of full-UL slots in the TDD pattern; otherwise FDD.
  std::optional<unsigned> nof_ul_slots;
  // CG period. The interesting cases are the ones that are not a multiple of the TDD period.
  cg_configuration::periodicity_t cg_period;
};

std::ostream& operator<<(std::ostream& out, const cg_offsets_test_params& p)
{
  if (p.nof_ul_slots.has_value()) {
    out << "TDD_ul_slots_" << p.nof_ul_slots.value();
  } else {
    out << "FDD";
  }
  return out << "_sl" << static_cast<unsigned>(p.cg_period);
}

// ---- Test fixture ----

class cg_usable_slot_offsets_test : public ::testing::TestWithParam<cg_offsets_test_params>
{
protected:
  cg_usable_slot_offsets_test() :
    cell_cfg(make_cg_cell_cfg(GetParam().nof_ul_slots, GetParam().cg_period)),
    offsets(config_helpers::compute_cg_usable_slot_offsets(cell_cfg))
  {
  }

  const ran_cell_config       cell_cfg;
  const std::vector<unsigned> offsets;
};

// ---- Tests ----

/// Every occurrence of a returned offset, not only the first one, must land on a full-UL slot free of PRACH. A CG
/// period that is not a multiple of the TDD period (e.g. sl16 against a 10-slot pattern) walks the offset through the
/// TDD pattern, so an offset starting on an UL slot can still hit a DL slot on a later repetition.
TEST_P(cg_usable_slot_offsets_test, every_occurrence_of_a_returned_offset_is_usable)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-BW-16-3");

  for (unsigned offset : offsets) {
    for (unsigned n = offset; n < fold_period_slots(cell_cfg); n += cg_period_slots(cell_cfg)) {
      EXPECT_TRUE(is_ul_slot(cell_cfg, n))
          << "CG offset " << offset << " recurs at slot " << n << ", which is not a full-UL slot";
      EXPECT_FALSE(is_prach_slot(cell_cfg, n))
          << "CG offset " << offset << " recurs at slot " << n << ", which carries a PRACH occasion";
    }
  }
}

/// The helper must not discard an offset whose every occurrence is usable, otherwise CG capacity is silently lost.
TEST_P(cg_usable_slot_offsets_test, no_usable_offset_is_left_out)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-BW-16-3");

  for (unsigned offset = 0; offset != cg_period_slots(cell_cfg); ++offset) {
    bool all_occurrences_usable = true;
    for (unsigned n = offset; n < fold_period_slots(cell_cfg); n += cg_period_slots(cell_cfg)) {
      if (not is_ul_slot(cell_cfg, n) or is_prach_slot(cell_cfg, n)) {
        all_occurrences_usable = false;
        break;
      }
    }
    if (all_occurrences_usable) {
      EXPECT_NE(std::find(offsets.begin(), offsets.end(), offset), offsets.end())
          << "CG offset " << offset << " is usable at every occurrence but was not returned";
    }
  }
}

/// The offsets are a subset of the CG period, strictly increasing and without duplicates.
TEST_P(cg_usable_slot_offsets_test, offsets_are_sorted_and_within_the_cg_period)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-BW-16-3");

  EXPECT_TRUE(std::is_sorted(offsets.begin(), offsets.end(), std::less_equal<>{}))
      << "the offsets must be strictly increasing";
  for (unsigned offset : offsets) {
    EXPECT_LT(offset, cg_period_slots(cell_cfg));
  }
}

// ---- Parameterization ----

INSTANTIATE_TEST_SUITE_P(
    cg_usable_slot_offsets,
    cg_usable_slot_offsets_test,
    ::testing::Values(
        // CG period that is a multiple of the 10-slot TDD period: every occurrence keeps the same TDD phase.
        cg_offsets_test_params{.nof_ul_slots = 2, .cg_period = cg_configuration::periodicity_t::sl40},
        cg_offsets_test_params{.nof_ul_slots = 6, .cg_period = cg_configuration::periodicity_t::sl40},
        cg_offsets_test_params{.nof_ul_slots = 6, .cg_period = cg_configuration::periodicity_t::sl20},
        // CG period that is NOT a multiple of the 10-slot TDD period: the offset walks the TDD pattern.
        cg_offsets_test_params{.nof_ul_slots = 2, .cg_period = cg_configuration::periodicity_t::sl16},
        cg_offsets_test_params{.nof_ul_slots = 6, .cg_period = cg_configuration::periodicity_t::sl16},
        cg_offsets_test_params{.nof_ul_slots = 6, .cg_period = cg_configuration::periodicity_t::sl64},
        // FDD, for the PRACH-only checks.
        cg_offsets_test_params{.cg_period = cg_configuration::periodicity_t::sl16},
        cg_offsets_test_params{.cg_period = cg_configuration::periodicity_t::sl40}),
    [](const ::testing::TestParamInfo<cg_offsets_test_params>& param_info) {
      std::ostringstream os;
      os << param_info.param;
      return os.str();
    });

// ---- Non-parameterized tests ----

/// Regression test: an offset whose first occurrence is a full-UL slot but whose later occurrences are not must be
/// rejected. Checking only the first occurrence, which is what the helper used to do, would hand the scheduler a CG
/// resource on a DL slot, as configured_grant_type2_sched_impl performs no UL check of its own.
TEST(cg_usable_slot_offsets_regression_test, offset_that_walks_onto_a_dl_slot_is_rejected)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-BW-16-3");

  // CG period of 16 slots against a 10-slot TDD pattern: gcd(16, 10) = 2, so each offset visits 5 different TDD slot
  // indices instead of always landing on the same one.
  const ran_cell_config cell_cfg = make_cg_cell_cfg(2, cg_configuration::periodicity_t::sl16);

  // Collect the offsets that pass a first-occurrence-only check but fail on a later occurrence: these are exactly the
  // ones the old implementation returned.
  std::vector<unsigned> offsets_walking_onto_dl;
  for (unsigned offset = 0; offset != cg_period_slots(cell_cfg); ++offset) {
    if (not is_ul_slot(cell_cfg, offset) or is_prach_slot(cell_cfg, offset)) {
      continue;
    }
    for (unsigned n = offset + cg_period_slots(cell_cfg); n < fold_period_slots(cell_cfg);
         n += cg_period_slots(cell_cfg)) {
      if (not is_ul_slot(cell_cfg, n)) {
        offsets_walking_onto_dl.push_back(offset);
        break;
      }
    }
  }
  ASSERT_FALSE(offsets_walking_onto_dl.empty())
      << "this cell configuration does not exercise the multi-occurrence TDD check";

  const std::vector<unsigned> offsets = config_helpers::compute_cg_usable_slot_offsets(cell_cfg);
  for (unsigned offset : offsets_walking_onto_dl) {
    EXPECT_EQ(std::find(offsets.begin(), offsets.end(), offset), offsets.end())
        << "CG offset " << offset << " starts on an UL slot but recurs on a DL slot, and must not be returned";
  }
}

/// The multi-occurrence check must not be so strict that it starves a well-matched cell: a CG period that is a
/// multiple of the TDD period must still yield one offset per full-UL slot of the pattern, minus the PRACH ones.
TEST(cg_usable_slot_offsets_regression_test, a_cg_period_aligned_with_the_tdd_pattern_keeps_its_offsets)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-BW-16-3");

  const ran_cell_config cell_cfg = make_cg_cell_cfg(6, cg_configuration::periodicity_t::sl40);

  const std::vector<unsigned> offsets = config_helpers::compute_cg_usable_slot_offsets(cell_cfg);
  EXPECT_FALSE(offsets.empty()) << "no usable CG slot offset was found for a TDD cell with 6 UL slots";
  for (unsigned offset : offsets) {
    EXPECT_TRUE(is_ul_slot(cell_cfg, offset));
  }
}

/// An FDD cell has no TDD constraint, so only the PRACH occasions remove offsets.
TEST(cg_usable_slot_offsets_regression_test, fdd_cell_only_drops_the_prach_offsets)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-BW-16-3");

  const ran_cell_config cell_cfg = make_cg_cell_cfg(std::nullopt, cg_configuration::periodicity_t::sl40);

  const std::vector<unsigned> offsets = config_helpers::compute_cg_usable_slot_offsets(cell_cfg);
  ASSERT_FALSE(offsets.empty());
  EXPECT_LT(offsets.size(), cg_period_slots(cell_cfg)) << "the PRACH occasions must remove at least one offset";
}

// ---- find_cg_pusch_td_res_idx ----

// Enables SRS on the cell and rebuilds its common PUSCH TDRA list around it, the way the DU config translator does.
void enable_srs(ran_cell_config& cell_cfg, unsigned max_nof_symbols, srs_nof_symbols nof_symbols)
{
  cell_cfg.init_bwp.srs_cfg.srs_type_enabled = srs_type::periodic;
  cell_cfg.init_bwp.srs_cfg.max_nof_symbols  = max_nof_symbols;
  cell_cfg.init_bwp.srs_cfg.nof_symbols      = nof_symbols;
  cell_cfg.ul_cfg_common.init_ul_bwp.pusch_cfg_common->pusch_td_alloc_list =
      time_domain_resource_helper::generate_dedicated_pusch_td_res_list(
          cell_cfg.tdd_cfg,
          cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.cp,
          cell_cfg.ul_cfg_common.init_ul_bwp.pusch_cfg_common->pusch_td_alloc_list.front().k2,
          static_cast<uint8_t>(max_nof_symbols),
          static_cast<uint8_t>(nof_symbols));
}

/// With no SRS the whole slot is free, so the very first PUSCH TD resource qualifies.
TEST(find_cg_pusch_td_res_idx_test, without_srs_the_first_resource_is_selected)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-BW-16-3");

  const ran_cell_config cell_cfg = make_cg_cell_cfg(std::nullopt, cg_configuration::periodicity_t::sl40);
  ASSERT_EQ(cell_cfg.init_bwp.srs_cfg.srs_type_enabled, srs_type::disabled);

  EXPECT_EQ(config_helpers::find_cg_pusch_td_res_idx(cell_cfg), 0U);
}

/// An SRS length that is a multiple of the per-resource length leaves a PUSCH TD resource ending right at its edge.
TEST(find_cg_pusch_td_res_idx_test, resource_clear_of_the_srs_is_selected)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-BW-16-3");

  constexpr unsigned max_nof_srs_symbols = 2;
  ran_cell_config    cell_cfg            = make_cg_cell_cfg(std::nullopt, cg_configuration::periodicity_t::sl40);
  enable_srs(cell_cfg, max_nof_srs_symbols, srs_nof_symbols::n1);

  const std::optional<unsigned> idx = config_helpers::find_cg_pusch_td_res_idx(cell_cfg);
  ASSERT_TRUE(idx.has_value());
  const auto& td_res = cell_cfg.ul_cfg_common.init_ul_bwp.pusch_cfg_common->pusch_td_alloc_list;
  EXPECT_LE(td_res[idx.value()].symbols.stop(), NOF_OFDM_SYM_PER_SLOT_NORMAL_CP - max_nof_srs_symbols)
      << "the selected PUSCH TD resource overlaps the SRS symbols";
}

/// A CG PUSCH is sized against the cell's whole SRS budget, so every SRS configuration an FDD cell accepts must leave
/// it a PUSCH TD resource to pick. The resource list is generated in steps of one SRS resource length, which used to
/// stop short of the SRS region whenever the budget was not a multiple of that length, leaving the CG PUSCH nothing
/// clear of the SRS.
TEST(find_cg_pusch_td_res_idx_test, every_fdd_srs_configuration_leaves_a_resource_clear_of_the_srs)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-BW-16-3");

  // Values the DU accepts: an SRS resource spans 1, 2 or 4 symbols, and the per-slot budget is 1 to 6 symbols and at
  // least one resource long.
  for (srs_nof_symbols nof_symbols : {srs_nof_symbols::n1, srs_nof_symbols::n2, srs_nof_symbols::n4}) {
    for (unsigned max_nof_symbols = nof_symbols; max_nof_symbols <= 6; ++max_nof_symbols) {
      ran_cell_config cell_cfg = make_cg_cell_cfg(std::nullopt, cg_configuration::periodicity_t::sl40);
      enable_srs(cell_cfg, max_nof_symbols, nof_symbols);

      const std::optional<unsigned> idx = config_helpers::find_cg_pusch_td_res_idx(cell_cfg);
      ASSERT_TRUE(idx.has_value()) << "no PUSCH TD resource avoids the SRS with a budget of " << max_nof_symbols
                                   << " symbols in resources of " << static_cast<unsigned>(nof_symbols);
      const auto& td_res = cell_cfg.ul_cfg_common.init_ul_bwp.pusch_cfg_common->pusch_td_alloc_list;
      EXPECT_LE(td_res[idx.value()].symbols.stop(), NOF_OFDM_SYM_PER_SLOT_NORMAL_CP - max_nof_symbols)
          << "the selected PUSCH TD resource overlaps the SRS symbols";
    }
  }
}

} // namespace
