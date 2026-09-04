// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief Unit tests for configured_grant_type1_rrm. Verifies correct CG resource allocation, PRACH/PUCCH collision
/// avoidance, multi-UE orthogonality, capacity exhaustion and resource reclamation.

#include "tests/ocudu_test_requirements.h"
#include "tests/test_doubles/scheduler/cell_config_builder_profiles.h"
#include "tests/test_doubles/utils/test_rng.h"
#include "ocudu/adt/format.h"
#include "ocudu/du/du_cell_config_helpers.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/ran/prach/prach_time_mapping.h"
#include "ocudu/ran/tdd/tdd_ul_dl_config.h"
#include "ocudu/scheduler/config/pucch_guardbands.h"
#include "ocudu/scheduler/config/ran_cell_config_helper.h"
#include "ocudu/scheduler/config/serving_cell_config_factory.h"
#include "ocudu/scheduler/rrm/configured_grant_type1_rrm.h"
#include "ocudu/scheduler/support/rb_helper.h"
#include "fmt/ostream.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace odu;

namespace {

// ---- Test parameters ----

struct cg_test_params {
  // If set, it's TDD; the struct fields configure the TDD pattern.
  std::optional<unsigned> nof_ul_slots;
};

std::ostream& operator<<(std::ostream& out, const cg_test_params& p)
{
  if (p.nof_ul_slots.has_value()) {
    out << fmt::format("TDD_ul_slots_{}", p.nof_ul_slots.value());
  } else {
    out << "FDD";
  }
  return out;
}

} // namespace

template <>
struct fmt::formatter<cg_test_params> : ostream_formatter {};

// ---- Helpers ----

// Builds cell_config_builder_params for FDD or TDD depending on the test parameter.
static cell_config_builder_params make_cell_cfg_params(const cg_test_params& params)
{
  const bool                 is_tdd = params.nof_ul_slots.has_value();
  cell_config_builder_params cell_params =
      cell_config_builder_profiles::create(is_tdd ? duplex_mode::TDD : duplex_mode::FDD);
  if (is_tdd) {
    auto&              tdd_cfg    = cell_params.tdd_ul_dl_cfg_common.emplace();
    const unsigned     nof_ul_sl  = params.nof_ul_slots.value();
    constexpr unsigned nof_ul_sym = 0U;
    const unsigned     nof_dl_sl  = 10U - nof_ul_sl - 1U;
    constexpr unsigned nof_dl_sym = 10U;
    tdd_cfg.pattern1              = {nof_dl_sl + 1U + nof_ul_sl, nof_dl_sl, nof_dl_sym, nof_ul_sl, nof_ul_sym};
    tdd_cfg.ref_scs               = subcarrier_spacing::kHz30;
  }
  return cell_params;
}

// Creates a du_cell_config with CG enabled using the given builder params.
static du_cell_config make_cg_du_cell_config(const cell_config_builder_params& cell_params,
                                             const cg_builder_params&          cg_params)
{
  du_cell_config du_cfg      = config_helpers::make_default_du_cell_config(cell_params);
  du_cfg.ran.init_bwp.cg_cfg = cg_params;
  return du_cfg;
}

// Helper struct that uniquely identifies a CG resource allocation for collision detection.
struct ue_cg_alloc_params {
  unsigned     offset;
  vrb_interval vrbs;

  bool operator==(const ue_cg_alloc_params& rhs) const { return offset == rhs.offset and vrbs == rhs.vrbs; }
};

// Extracts the CG allocation identifier (offset, VRBs) from a UE's cell config.
ue_cg_alloc_params get_cg_alloc(const ue_cell_config& ue_cell_cfg)
{
  const auto& ue_cg = ue_cell_cfg.bwps[0].ul.cg;
  ocudu_assert(ue_cg.has_value(), "Configured Grant config not set");
  return {ue_cg.value().cg_offset, ue_cg.value().vrbs};
}

// ---- Test fixture ----

class configured_grant_type1_rrm_test : public ::testing::TestWithParam<cg_test_params>
{
protected:
  // NOTE: the default cg_builder_params are expected to be self-consistent, i.e. the number of PRBs derived by the
  // resource manager from grant_size_or_bitrate and MCS must not exceed max_nof_cell_cg_rbs, otherwise no CG
  // allocation is possible.
  explicit configured_grant_type1_rrm_test(const cg_builder_params& cg_params_ = {}) :
    cg_params(cg_params_),
    cell_params(make_cell_cfg_params(GetParam())),
    cell_cfg_list({make_cg_du_cell_config(cell_params, cg_params)})
  {
    cg_res_mng.add_cell(to_du_cell_index(0), cell_cfg_list.front().ran);
  }

  // Creates a ue_cell_config for a new UE PCell, then calls build_ue_cg_config.
  // Return the ue_cell_config if allocation succeeded, nullopt otherwise.
  std::optional<ue_cell_config> add_ue(du_ue_index_t ue_idx)
  {
    ue_cell_config ue_cell_cfg = config_helpers::make_default_ue_cell_config(cell_cfg_list.front().ran);

    // Reset CG config so build_ue_cg_config fills it fresh.
    ue_cell_cfg.serv_cell_cfg.ul_config->init_ul_bwp.cg_cfg.reset();

    if (not cg_res_mng.build_ue_cg_config(ue_cell_cfg)) {
      return std::nullopt;
    }

    ues.emplace(ue_idx, ue_cell_cfg);
    return ue_cell_cfg;
  }

  // Deallocates CG resources for the given UE and removes it from the pool.
  void rem_ue(du_ue_index_t ue_idx)
  {
    ASSERT_TRUE(ues.contains(ue_idx));
    cg_res_mng.reset_ue_cg_config(ues[ue_idx]);
    ues.erase(ue_idx);
  }

  // Returns the CG period in slots.
  unsigned cg_period_slots() const { return static_cast<unsigned>(cg_params.periodicity.value()); }

  // Returns the PRACH periodicity, in slots.
  unsigned prach_period_slots() const
  {
    const auto ul_scs = cell_cfg_list.front().ran.ul_cfg_common.init_ul_bwp.generic_params.scs;
    return get_nof_slots_per_subframe(ul_scs) * static_cast<unsigned>(NOF_SUBFRAMES_PER_FRAME) *
           make_prach_mapper().sfn_period();
  }

  // Returns the PRACH slot helper for collision checks.
  prach_helper::preamble_slot_mapping make_prach_mapper() const
  {
    const auto& ran = cell_cfg_list.front().ran;
    return {ran.dl_carrier.band,
            ran.ul_cfg_common.init_ul_bwp.generic_params.scs,
            ran.ul_cfg_common.init_ul_bwp.rach_cfg_common.value().rach_cfg_generic.prach_config_index};
  }

  // Returns the PUCCH guardband CRBs bitmap (BWP-relative).
  crb_bitmap get_pucch_crbs() const { return compute_pucch_crbs(cell_cfg_list.front().ran); }

  // Checks whether the given slot offset (within the CG period) falls on a PRACH occasion.
  bool is_prach_slot(unsigned slot_offset) const
  {
    const auto td_mapper = make_prach_mapper();
    const auto ul_scs    = cell_cfg_list.front().ran.ul_cfg_common.init_ul_bwp.generic_params.scs;
    return td_mapper.has_prach_occasion(slot_point(ul_scs, slot_offset));
  }

  // Checks whether CG VRBs overlap with PUCCH CRBs.
  bool cg_overlaps_pucch(const ue_cg_alloc_params& alloc) const
  {
    const auto&    ran           = cell_cfg_list.front().ran;
    const unsigned bwp_crb_start = ran.ul_cfg_common.init_ul_bwp.generic_params.crbs.start();
    const auto     cg_crbs       = rb_helper::vrb_to_crb_ul_non_interleaved(alloc.vrbs, bwp_crb_start);
    const auto     pucch_crbs    = get_pucch_crbs();
    // Check overlap: any CRB in [cg_crbs.start - bwp_start, cg_crbs.stop - bwp_start) set in PUCCH bitmap.
    for (unsigned crb = cg_crbs.start() - bwp_crb_start; crb < cg_crbs.stop() - bwp_crb_start; ++crb) {
      if (pucch_crbs.test(crb)) {
        return true;
      }
    }
    return false;
  }

  // Checks whether two CG allocations collide (same offset with overlapping VRBs).
  static bool cg_resources_collide(const ue_cg_alloc_params& ue_1, const ue_cg_alloc_params& ue_2, unsigned period)
  {
    // Two CG allocations collide if they appear in the same slot (i.e. offsets are congruent modulo the period)
    // AND their VRB intervals overlap.
    if (ue_1.offset % period != ue_2.offset % period) {
      return false;
    }
    // Check VRB overlap.
    return ue_1.vrbs.overlaps(ue_2.vrbs);
  }

  cg_builder_params                             cg_params;
  cell_config_builder_params                    cell_params;
  std::vector<du_cell_config>                   cell_cfg_list;
  configured_grant_type1_rrm                    cg_res_mng;
  slotted_array<ue_cell_config, MAX_NOF_DU_UES> ues;
};

// ---- Tests ----

/// Test: a single UE gets all CG parameters correctly populated.
TEST_P(configured_grant_type1_rrm_test, single_ue_cg_config_is_fully_populated)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-BW-16-3");

  auto ue = add_ue(to_du_ue_index(0));
  ASSERT_TRUE(ue.has_value()) << "CG allocation failed for a single UE";

  // Verify the serving cell CG config is set.
  ASSERT_TRUE(ue->serv_cell_cfg.ul_config.has_value());
  ASSERT_TRUE(ue->serv_cell_cfg.ul_config->init_ul_bwp.cg_cfg.has_value());

  const auto& cg_cfg = ue->serv_cell_cfg.ul_config->init_ul_bwp.cg_cfg.value();

  // Verify cg_builder_params are reflected in the CG configuration.
  EXPECT_EQ(cg_cfg.periodicity, cg_params.periodicity.value());
  EXPECT_EQ(cg_cfg.nof_harq_processes, cg_params.nof_harq_processes);
  // Verify rrc_configured_ul_grant is populated.
  ASSERT_TRUE(cg_cfg.rrc_configured_ul_grant_cfg.has_value());
  const auto& grant = cg_cfg.rrc_configured_ul_grant_cfg.value();
  EXPECT_EQ(grant.mcs, cg_params.mcs);
  // The number of VRBs is derived by the resource manager from grant_size_or_bitrate and MCS; verify it is set.
  const auto& freq_res = std::get<ra_frequency_type1_configuration>(grant.freq_domain_res);
  EXPECT_GT(freq_res.length_vrb, 0U);
  EXPECT_LE(freq_res.length_vrb, cg_params.max_nof_cell_cg_rbs);

  // Verify the BWP-level CG config is set and consistent with the RRC configured UL grant.
  ASSERT_FALSE(ue->bwps.empty());
  const auto& bwp_cg = ue->bwps[0].ul.cg;
  ASSERT_TRUE(bwp_cg.has_value());
  EXPECT_EQ(bwp_cg.value().vrbs.start(), freq_res.start_vrb);
  EXPECT_EQ(bwp_cg.value().vrbs.length(), freq_res.length_vrb);
  EXPECT_EQ(bwp_cg.value().cg_offset, grant.time_domain_offset);
}

/// Test: CG offset does not fall on a PRACH slot.
TEST_P(configured_grant_type1_rrm_test, cg_offset_does_not_collide_with_prach)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-BW-16-3");

  auto ue = add_ue(to_du_ue_index(0));
  ASSERT_TRUE(ue.has_value());

  const auto [offset, vrbs] = get_cg_alloc(*ue);

  // Verify the CG offset does not collide with any PRACH occasion across all periodic repetitions.
  const auto     td_mapper = make_prach_mapper();
  const auto     ul_scs    = cell_cfg_list.front().ran.ul_cfg_common.init_ul_bwp.generic_params.scs;
  const unsigned prach_period =
      get_nof_slots_per_subframe(ul_scs) * static_cast<unsigned>(NOF_SUBFRAMES_PER_FRAME) * td_mapper.sfn_period();
  const unsigned lcm_period = std::lcm(prach_period, cg_period_slots());

  for (unsigned n = offset; n < lcm_period; n += cg_period_slots()) {
    EXPECT_FALSE(is_prach_slot(n)) << "CG slot offset " << n << " collides with a PRACH occasion";
  }
}

/// Test: every occurrence of the CG offset, and not only the first one, falls on a full-UL slot.
TEST_P(configured_grant_type1_rrm_test, cg_offset_falls_on_a_full_ul_slot)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-BW-16-3");

  const auto& ran = cell_cfg_list.front().ran;
  if (ran.tdd_cfg.has_value()) {
    const auto ue = add_ue(to_du_ue_index(0));
    ASSERT_TRUE(ue.has_value());

    const auto [offset, vrbs] = get_cg_alloc(*ue);

    // The CG, PRACH and TDD patterns realign after the LCM of their periods, so the occurrences of the offset within
    // that window are all the distinct slots it can land on.
    const unsigned lcm_period =
        std::lcm(std::lcm(prach_period_slots(), cg_period_slots()), nof_slots_per_tdd_period(ran.tdd_cfg.value()));
    for (unsigned n = offset; n < lcm_period; n += cg_period_slots()) {
      EXPECT_TRUE(is_tdd_full_ul_slot(ran.tdd_cfg.value(), n))
          << "CG offset " << offset << " recurs at slot " << n << ", which is not a full-UL slot";
    }
  }
}

/// Test: CG VRBs do not overlap with PUCCH guardband CRBs.
TEST_P(configured_grant_type1_rrm_test, cg_rbs_do_not_collide_with_pucch)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-BW-16-3");

  auto ue = add_ue(to_du_ue_index(0));
  ASSERT_TRUE(ue.has_value());

  EXPECT_FALSE(cg_overlaps_pucch(get_cg_alloc(*ue))) << "CG VRBs overlap with PUCCH guardband CRBs";
}

/// Test: multiple UEs get orthogonal CG resources (no collision in offset+VRBs).
TEST_P(configured_grant_type1_rrm_test, multiple_ues_get_orthogonal_cg_resources)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-BW-16-3");

  std::vector<ue_cg_alloc_params> used_allocs;

  for (unsigned i = 0; i != MAX_NOF_DU_UES; ++i) {
    auto ue = add_ue(to_du_ue_index(i));
    if (not ue.has_value()) {
      // Resources exhausted — this is expected and tested separately.
      break;
    }

    const auto alloc = get_cg_alloc(*ue);

    // Verify no collision with any previously allocated UE.
    for (const auto& prev : used_allocs) {
      ASSERT_FALSE(cg_resources_collide(alloc, prev, cg_period_slots()))
          << "CG collision: UE " << i << " (offset=" << alloc.offset << ", vrbs=[" << alloc.vrbs.start() << ","
          << alloc.vrbs.stop() << ")) collides with previous (offset=" << prev.offset << ", vrbs=[" << prev.vrbs.start()
          << "," << prev.vrbs.stop() << "))";
    }

    // Each new allocation must also avoid PRACH and PUCCH.
    EXPECT_FALSE(cg_overlaps_pucch(alloc)) << "UE " << i << " CG VRBs overlap with PUCCH";

    used_allocs.push_back(alloc);
  }

  EXPECT_GT(used_allocs.size(), 1U) << "Expected at least 2 UEs to fit with CG resources";
}

/// Test: allocation fails when resources are exhausted; all previous UEs remain valid.
TEST_P(configured_grant_type1_rrm_test, allocation_fails_when_resources_exhausted)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-BW-16-3");

  unsigned nof_allocated = 0;

  for (unsigned i = 0; i != MAX_NOF_DU_UES; ++i) {
    auto ue = add_ue(to_du_ue_index(i));
    if (not ue.has_value()) {
      break;
    }
    ++nof_allocated;
  }

  // At least one UE should have been allocated.
  ASSERT_GT(nof_allocated, 0U);

  // The next allocation must fail.
  auto extra_ue = add_ue(to_du_ue_index(nof_allocated));
  EXPECT_FALSE(extra_ue.has_value()) << "Allocation should fail after resource exhaustion";
}

/// Test: allocating twice for the same UE is a no-op. A second allocation would overwrite the offset and VRBs stored
/// in the UE config, leaving the RBs of the first one reserved in the grid with nothing left pointing at them.
TEST_P(configured_grant_type1_rrm_test, allocating_twice_for_the_same_ue_is_a_no_op)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-BW-16-3");

  auto ue = add_ue(to_du_ue_index(0));
  ASSERT_TRUE(ue.has_value());
  const ue_cg_alloc_params first_alloc = get_cg_alloc(*ue);

  // Fill the rest of the cell, so that a second allocation would have to find room that is no longer there.
  for (unsigned i = 1; i != MAX_NOF_DU_UES; ++i) {
    if (not add_ue(to_du_ue_index(i)).has_value()) {
      break;
    }
  }

  // The UE already holds its CG resources, so the call must report success and change nothing, even though there is
  // no room left in the cell for a new allocation.
  ue_cell_config& stored_ue = ues[to_du_ue_index(0)];
  EXPECT_TRUE(cg_res_mng.build_ue_cg_config(stored_ue))
      << "re-allocating for an already configured UE must succeed, even on a full cell";
  EXPECT_EQ(get_cg_alloc(stored_ue), first_alloc) << "the CG allocation of the UE must be left untouched";
}

/// Test: after removing a UE, a new UE can be allocated with the freed resources.
TEST_P(configured_grant_type1_rrm_test, dealloc_and_realloc_succeeds)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-BW-16-3");

  // Fill up all resources.
  unsigned nof_allocated = 0;
  for (unsigned i = 0; i != MAX_NOF_DU_UES; ++i) {
    auto ue = add_ue(to_du_ue_index(i));
    if (not ue.has_value()) {
      break;
    }
    ++nof_allocated;
  }
  ASSERT_GT(nof_allocated, 1U) << "Need at least 2 UEs to test dealloc/realloc";

  // Verify allocation is now rejected.
  ASSERT_FALSE(add_ue(to_du_ue_index(nof_allocated)).has_value());

  // Remove a random UE.
  const auto          rem_idx   = test_rng::uniform_int<unsigned>(0, nof_allocated - 1);
  const du_ue_index_t ue_to_rem = to_du_ue_index(rem_idx);
  rem_ue(ue_to_rem);

  // A new UE should now be allocatable.
  auto new_ue = add_ue(to_du_ue_index(nof_allocated));
  ASSERT_TRUE(new_ue.has_value()) << "Expected allocation to succeed after deallocation";

  // The new allocation must not collide with remaining UEs.
  const auto new_alloc = get_cg_alloc(*new_ue);
  for (unsigned i = 0; i != nof_allocated; ++i) {
    const du_ue_index_t idx = to_du_ue_index(i);
    if (not ues.contains(idx)) {
      continue;
    }
    const auto existing_alloc = get_cg_alloc(ues[idx]);
    EXPECT_FALSE(cg_resources_collide(new_alloc, existing_alloc, cg_period_slots()))
        << "New UE collides with existing UE after realloc";
  }
}

// ---- Non-parameterized tests ----

/// A CG period that is not a multiple of the TDD period walks the offset through the whole TDD pattern. When no offset
/// stays on a full-UL slot at every one of its occurrences, the allocation must be refused rather than granted on a
/// slot that turns out to be DL.
TEST(configured_grant_type1_rrm_tdd_test, allocation_is_refused_when_no_offset_stays_on_ul_slots)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-BW-16-3");

  // TDD 7DL + 1S + 2UL (10-slot period) against a 16-slot CG period: gcd(16, 10) = 2, so each offset visits 5
  // different TDD slot indices, of which at most 2 are UL.
  const du_cell_config du_cfg =
      make_cg_du_cell_config(make_cell_cfg_params(cg_test_params{.nof_ul_slots = 2}),
                             cg_builder_params{.periodicity = cg_configuration::periodicity_t::sl16});

  configured_grant_type1_rrm res_mng;
  res_mng.add_cell(to_du_cell_index(0), du_cfg.ran);

  ue_cell_config ue_cell_cfg = config_helpers::make_default_ue_cell_config(du_cfg.ran);
  ue_cell_cfg.serv_cell_cfg.ul_config->init_ul_bwp.cg_cfg.reset();

  EXPECT_FALSE(res_mng.build_ue_cg_config(ue_cell_cfg))
      << "a CG offset was allocated even though every one of its occurrences cannot be an UL slot";
}

// ---- Parameterization ----

INSTANTIATE_TEST_SUITE_P(configured_grant_type1_rrm,
                         configured_grant_type1_rrm_test,
                         ::testing::Values(
                             // FDD.
                             cg_test_params{},
                             // TDD DL-heavy: 7 DL + 1 partial (10S DL, 0S UL) + 2 UL.
                             cg_test_params{.nof_ul_slots = 2},
                             // TDD UL-heavy: 3 DL + 1 partial (10S DL, 0S UL) + 6 UL.
                             cg_test_params{.nof_ul_slots = 6}));
