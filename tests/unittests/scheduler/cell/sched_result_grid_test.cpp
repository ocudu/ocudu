// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/scheduler/result/sched_result_grid.h"
#include <gtest/gtest.h>

using namespace ocudu;

namespace {

/// Number of slots of the grid under test.
constexpr unsigned nof_test_slots = 4;
/// Slot the single-slot tests operate on.
constexpr unsigned test_slot_idx = 1;

class sched_result_grid_test : public ::testing::Test
{
protected:
  sched_result_grid_test() : grid(sched_result_grid_config{nof_test_slots}) {}

  /// Fills the UE grant list of a slot up to the given number of grants.
  void fill_ue_grants(unsigned slot_idx, unsigned nof_grants)
  {
    auto ue_grants = grid.get_builder(slot_idx).dl().ue_grants();
    for (unsigned i = 0; i != nof_grants; ++i) {
      dl_msg_alloc* grant = ue_grants.emplace_back();
      ASSERT_NE(grant, nullptr);
      grant->pdsch_cfg.rnti = to_rnti(0x4601 + i);
    }
  }

  sched_result_grid grid;
};

} // namespace

// The builder and the storage of a list are always derived from the same type, so that a list moving to a different
// storage cannot leave its builder behind.
static_assert(
    std::is_same_v<pdu_list_builder<dl_msg_alloc>::storage_type, decltype(sched_result_context::dl_slot::ue_grants)>,
    "UE grant builder and storage disagree");
static_assert(std::is_same_v<pdu_list_builder<srs_info>::storage_type, decltype(sched_result_context::ul_slot::srss)>,
              "SRS builder and storage disagree");
static_assert(std::is_same_v<pdu_list_builder<sib_information>::storage_type,
                             decltype(sched_result_context::broadcast_slot::sibs)>,
              "SIB builder and storage disagree");
static_assert(std::is_same_v<pdu_list_builder<rar_information>::storage_type,
                             decltype(sched_result_context::dl_slot::rar_grants)>,
              "RAR builder and storage disagree");

// The RAR list is pooled and the UE grant list is not, so they resolve to different builders and views through the
// same aliases.
static_assert(!std::is_same_v<pdu_list_builder<rar_information>, pdu_list_builder<dl_msg_alloc>>,
              "Pooled and inline lists must not share a builder");
static_assert(!std::is_same_v<pdu_span<rar_information>, pdu_span<dl_msg_alloc>>,
              "Pooled and inline lists must not share a view");
static_assert(std::is_same_v<pdu_span<dl_msg_alloc>, span<const dl_msg_alloc>>,
              "Inline list resolved to the wrong view");
static_assert(std::is_same_v<pdu_span<rar_information>, sched_pdu_detail::pooled_list_view<rar_information>>,
              "Pooled list resolved to the wrong view");

TEST_F(sched_result_grid_test, when_grid_is_created_then_it_holds_the_configured_number_of_slots)
{
  ASSERT_EQ(grid.nof_slots(), nof_test_slots);
}

TEST_F(sched_result_grid_test, when_grid_is_created_then_slots_are_empty)
{
  for (unsigned i = 0; i != nof_test_slots; ++i) {
    const sched_result_grid& cgrid = grid;
    sched_slot_result_reader slot  = cgrid.get_reader(i);
    ASSERT_TRUE(slot.valid());
    ASSERT_FALSE(slot.success());
    ASSERT_TRUE(slot.dl().ue_grants().empty());
    ASSERT_TRUE(slot.dl().rar_grants().empty());
    ASSERT_TRUE(slot.ul().puschs().empty());
    ASSERT_EQ(slot.dl().nof_dl_symbols(), 0);
    ASSERT_EQ(slot.ul().nof_ul_symbols(), 0);
  }
}

TEST_F(sched_result_grid_test, when_default_constructed_then_reader_is_invalid)
{
  ASSERT_FALSE(sched_slot_result_reader{}.valid());
  ASSERT_FALSE(sched_slot_dl_result_reader{}.valid());
  ASSERT_FALSE(sched_slot_ul_result_reader{}.valid());
}

TEST_F(sched_result_grid_test, when_pdu_is_emplaced_then_reader_sees_it)
{
  const rnti_t test_rnti = to_rnti(0x4601);

  dl_msg_alloc* grant = grid.get_builder(test_slot_idx).dl().ue_grants().emplace_back();
  ASSERT_NE(grant, nullptr);
  grant->pdsch_cfg.rnti = test_rnti;

  const sched_result_grid& cgrid  = grid;
  pdu_span<dl_msg_alloc>   grants = cgrid.get_reader(test_slot_idx).dl().ue_grants();
  ASSERT_EQ(grants.size(), 1);
  ASSERT_EQ(grants[0].pdsch_cfg.rnti, test_rnti);
  ASSERT_EQ(grants.front().pdsch_cfg.rnti, test_rnti);
  ASSERT_EQ(grants.back().pdsch_cfg.rnti, test_rnti);
}

TEST_F(sched_result_grid_test, when_pdu_is_emplaced_in_one_slot_then_other_slots_are_unaffected)
{
  ASSERT_NE(grid.get_builder(test_slot_idx).dl().ue_grants().emplace_back(), nullptr);

  const sched_result_grid& cgrid = grid;
  for (unsigned i = 0; i != nof_test_slots; ++i) {
    const size_t expected_size = (i == test_slot_idx) ? 1 : 0;
    ASSERT_EQ(cgrid.get_reader(i).dl().ue_grants().size(), expected_size);
  }
}

TEST_F(sched_result_grid_test, when_list_is_full_then_emplace_back_returns_nullptr)
{
  auto ue_grants = grid.get_builder(test_slot_idx).dl().ue_grants();
  ASSERT_NO_FATAL_FAILURE(fill_ue_grants(test_slot_idx, MAX_UE_PDUS_PER_SLOT));

  ASSERT_TRUE(ue_grants.full());
  ASSERT_EQ(ue_grants.size(), MAX_UE_PDUS_PER_SLOT);
  ASSERT_EQ(ue_grants.emplace_back(), nullptr);
  ASSERT_EQ(ue_grants.size(), MAX_UE_PDUS_PER_SLOT);
}

TEST_F(sched_result_grid_test, when_pdu_is_popped_then_list_shrinks)
{
  static constexpr unsigned nof_grants = 3;

  auto ue_grants = grid.get_builder(test_slot_idx).dl().ue_grants();
  ASSERT_NO_FATAL_FAILURE(fill_ue_grants(test_slot_idx, nof_grants));
  ASSERT_EQ(ue_grants.size(), nof_grants);

  ue_grants.pop_back();
  ASSERT_EQ(ue_grants.size(), nof_grants - 1);
  ASSERT_FALSE(ue_grants.full());
  ASSERT_EQ(ue_grants.back().pdsch_cfg.rnti, to_rnti(0x4601 + nof_grants - 2));
}

TEST_F(sched_result_grid_test, when_full_list_is_popped_then_emplace_back_succeeds_again)
{
  auto ue_grants = grid.get_builder(test_slot_idx).dl().ue_grants();
  ASSERT_NO_FATAL_FAILURE(fill_ue_grants(test_slot_idx, MAX_UE_PDUS_PER_SLOT));
  ASSERT_TRUE(ue_grants.full());

  ue_grants.pop_back();
  ASSERT_FALSE(ue_grants.full());
  ASSERT_NE(ue_grants.emplace_back(), nullptr);
  ASSERT_TRUE(ue_grants.full());
}

TEST_F(sched_result_grid_test, builder_iteration_visits_every_pdu_in_order)
{
  static constexpr unsigned nof_grants = 3;
  ASSERT_NO_FATAL_FAILURE(fill_ue_grants(test_slot_idx, nof_grants));

  unsigned count = 0;
  for (dl_msg_alloc& grant : grid.get_builder(test_slot_idx).dl().ue_grants()) {
    ASSERT_EQ(grant.pdsch_cfg.rnti, to_rnti(0x4601 + count));
    ++count;
  }
  ASSERT_EQ(count, nof_grants);
}

TEST_F(sched_result_grid_test, view_of_builder_matches_reader)
{
  static constexpr unsigned nof_grants = 2;
  ASSERT_NO_FATAL_FAILURE(fill_ue_grants(test_slot_idx, nof_grants));

  pdu_span<dl_msg_alloc>   from_builder = grid.get_builder(test_slot_idx).dl().ue_grants().view();
  const sched_result_grid& cgrid        = grid;
  pdu_span<dl_msg_alloc>   from_reader  = cgrid.get_reader(test_slot_idx).dl().ue_grants();

  ASSERT_EQ(from_builder.size(), from_reader.size());
  ASSERT_EQ(&from_builder.front(), &from_reader.front());
}

TEST_F(sched_result_grid_test, last_returns_the_tail_of_the_list)
{
  static constexpr unsigned nof_grants = 4;
  static constexpr unsigned tail_size  = 2;
  ASSERT_NO_FATAL_FAILURE(fill_ue_grants(test_slot_idx, nof_grants));

  const sched_result_grid& cgrid = grid;
  pdu_span<dl_msg_alloc>   tail  = cgrid.get_reader(test_slot_idx).dl().ue_grants().last(tail_size);

  ASSERT_EQ(tail.size(), tail_size);
  ASSERT_EQ(tail.front().pdsch_cfg.rnti, to_rnti(0x4601 + nof_grants - tail_size));
  ASSERT_EQ(tail.back().pdsch_cfg.rnti, to_rnti(0x4601 + nof_grants - 1));
}

TEST_F(sched_result_grid_test, default_constructed_view_is_empty)
{
  pdu_span<dl_msg_alloc> inline_view;
  ASSERT_TRUE(inline_view.empty());
  ASSERT_EQ(inline_view.size(), 0);
  ASSERT_EQ(inline_view.begin(), inline_view.end());

  pdu_span<rar_information> pooled_view;
  ASSERT_TRUE(pooled_view.empty());
  ASSERT_EQ(pooled_view.size(), 0);
  ASSERT_EQ(pooled_view.begin(), pooled_view.end());
}

TEST_F(sched_result_grid_test, scalars_written_through_builder_are_seen_by_reader)
{
  auto builder = grid.get_builder(test_slot_idx);
  builder.set_success(true);
  ++builder.failed_attempts().dl_pdcch;
  ++builder.failed_attempts().uci;

  const sched_result_grid& cgrid = grid;
  sched_slot_result_reader slot  = cgrid.get_reader(test_slot_idx);
  ASSERT_TRUE(slot.success());
  ASSERT_EQ(slot.failed_attempts().dl_pdcch, 1);
  ASSERT_EQ(slot.failed_attempts().uci, 1);
  ASSERT_EQ(slot.failed_attempts().ul_pdcch, 0);
}

TEST_F(sched_result_grid_test, when_slot_is_reset_then_its_pdus_and_scalars_are_discarded)
{
  static constexpr unsigned nof_dl_symbols = 14;
  static constexpr unsigned nof_ul_symbols = 0;

  auto builder = grid.get_builder(test_slot_idx);
  builder.set_success(true);
  ++builder.failed_attempts().dl_pdcch;
  ASSERT_NO_FATAL_FAILURE(fill_ue_grants(test_slot_idx, 2));
  ASSERT_NE(builder.dl().rar_grants().emplace_back(), nullptr);
  ASSERT_NE(builder.ul().puschs().emplace_back(), nullptr);
  builder.ul().pucchs().emplace();
  ASSERT_EQ(builder.ul().pucchs().size(), 1);

  grid.reset_slot(test_slot_idx, nof_dl_symbols, nof_ul_symbols);

  const sched_result_grid& cgrid = grid;
  sched_slot_result_reader slot  = cgrid.get_reader(test_slot_idx);
  ASSERT_FALSE(slot.success());
  ASSERT_EQ(slot.failed_attempts().dl_pdcch, 0);
  ASSERT_TRUE(slot.dl().ue_grants().empty());
  ASSERT_TRUE(slot.dl().rar_grants().empty());
  ASSERT_TRUE(slot.ul().puschs().empty());
  ASSERT_TRUE(slot.ul().pucchs().empty());
  ASSERT_EQ(slot.dl().nof_dl_symbols(), nof_dl_symbols);
  ASSERT_EQ(slot.ul().nof_ul_symbols(), nof_ul_symbols);
}

TEST_F(sched_result_grid_test, when_slot_is_reset_then_other_slots_keep_their_pdus)
{
  static constexpr unsigned other_slot_idx = test_slot_idx + 1;

  ASSERT_NO_FATAL_FAILURE(fill_ue_grants(test_slot_idx, 1));
  ASSERT_NO_FATAL_FAILURE(fill_ue_grants(other_slot_idx, 1));

  grid.reset_slot(test_slot_idx, 0, 0);

  const sched_result_grid& cgrid = grid;
  ASSERT_TRUE(cgrid.get_reader(test_slot_idx).dl().ue_grants().empty());
  ASSERT_EQ(cgrid.get_reader(other_slot_idx).dl().ue_grants().size(), 1);
}

TEST_F(sched_result_grid_test, when_slot_is_reset_then_list_capacity_is_preserved)
{
  ASSERT_NO_FATAL_FAILURE(fill_ue_grants(test_slot_idx, MAX_UE_PDUS_PER_SLOT));
  ASSERT_TRUE(grid.get_builder(test_slot_idx).dl().ue_grants().full());

  grid.reset_slot(test_slot_idx, 0, 0);

  // A reset that dropped the capacity would refuse the very first grant, or allocate to satisfy it.
  auto ue_grants = grid.get_builder(test_slot_idx).dl().ue_grants();
  ASSERT_FALSE(ue_grants.full());
  ASSERT_NO_FATAL_FAILURE(fill_ue_grants(test_slot_idx, MAX_UE_PDUS_PER_SLOT));
  ASSERT_TRUE(ue_grants.full());
  ASSERT_EQ(ue_grants.size(), MAX_UE_PDUS_PER_SLOT);
}

TEST_F(sched_result_grid_test, every_dl_list_is_reachable_and_independent)
{
  auto dl = grid.get_builder(test_slot_idx).dl();
  ASSERT_NE(dl.dl_pdcchs().emplace_back(), nullptr);
  ASSERT_NE(dl.ul_pdcchs().emplace_back(), nullptr);
  ASSERT_NE(dl.ssb_info().emplace_back(), nullptr);
  ASSERT_NE(dl.sibs().emplace_back(), nullptr);
  ASSERT_NE(dl.rar_grants().emplace_back(), nullptr);
  ASSERT_NE(dl.paging_grants().emplace_back(), nullptr);
  ASSERT_NE(dl.ue_grants().emplace_back(), nullptr);
  ASSERT_NE(dl.csi_rs().emplace_back(), nullptr);
  ASSERT_NE(dl.prs().emplace_back(), nullptr);

  const sched_result_grid&    cgrid = grid;
  sched_slot_dl_result_reader slot  = cgrid.get_reader(test_slot_idx).dl();
  ASSERT_EQ(slot.dl_pdcchs().size(), 1);
  ASSERT_EQ(slot.ul_pdcchs().size(), 1);
  ASSERT_EQ(slot.ssb_info().size(), 1);
  ASSERT_EQ(slot.sibs().size(), 1);
  ASSERT_EQ(slot.rar_grants().size(), 1);
  ASSERT_EQ(slot.paging_grants().size(), 1);
  ASSERT_EQ(slot.ue_grants().size(), 1);
  ASSERT_EQ(slot.csi_rs().size(), 1);
  ASSERT_EQ(slot.prs().size(), 1);
}

TEST_F(sched_result_grid_test, every_ul_list_is_reachable_and_independent)
{
  auto ul = grid.get_builder(test_slot_idx).ul();
  ASSERT_NE(ul.puschs().emplace_back(), nullptr);
  ASSERT_NE(ul.prachs().emplace_back(), nullptr);
  ASSERT_NE(ul.srss().emplace_back(), nullptr);
  ul.pucchs().emplace();

  const sched_result_grid&    cgrid = grid;
  sched_slot_ul_result_reader slot  = cgrid.get_reader(test_slot_idx).ul();
  ASSERT_EQ(slot.puschs().size(), 1);
  ASSERT_EQ(slot.prachs().size(), 1);
  ASSERT_EQ(slot.srss().size(), 1);
  ASSERT_EQ(slot.pucchs().size(), 1);
}

TEST_F(sched_result_grid_test, prach_list_holds_a_single_occasion)
{
  auto prachs = grid.get_builder(test_slot_idx).ul().prachs();
  ASSERT_NE(prachs.emplace_back(), nullptr);
  ASSERT_TRUE(prachs.full());
  ASSERT_EQ(prachs.emplace_back(), nullptr);
}

TEST_F(sched_result_grid_test, builder_reader_reflects_pdus_built_so_far)
{
  auto dl = grid.get_builder(test_slot_idx).dl();
  ASSERT_TRUE(dl.reader().ue_grants().empty());
  ASSERT_NE(dl.ue_grants().emplace_back(), nullptr);
  ASSERT_EQ(dl.reader().ue_grants().size(), 1);
}

// The RAR list is the one pooled list, so it is where the pool behaviour is exercised.
TEST_F(sched_result_grid_test, pooled_list_holds_the_pdus_written_through_it)
{
  static constexpr unsigned nof_rars = 3;

  auto rar_grants = grid.get_builder(test_slot_idx).dl().rar_grants();
  for (unsigned i = 0; i != nof_rars; ++i) {
    rar_information* rar = rar_grants.emplace_back();
    ASSERT_NE(rar, nullptr);
    rar->pdsch_cfg.rnti = to_rnti(0x0001 + i);
  }
  ASSERT_EQ(rar_grants.size(), nof_rars);

  unsigned count = 0;
  for (rar_information& rar : rar_grants) {
    ASSERT_EQ(rar.pdsch_cfg.rnti, to_rnti(0x0001 + count));
    ++count;
  }
  ASSERT_EQ(count, nof_rars);

  const sched_result_grid&  cgrid = grid;
  pdu_span<rar_information> rars  = cgrid.get_reader(test_slot_idx).dl().rar_grants();
  ASSERT_EQ(rars.size(), nof_rars);
  ASSERT_EQ(rars[0].pdsch_cfg.rnti, to_rnti(0x0001));
  ASSERT_EQ(rars.back().pdsch_cfg.rnti, to_rnti(0x0001 + nof_rars - 1));
  ASSERT_EQ(rars.last(1).front().pdsch_cfg.rnti, to_rnti(0x0001 + nof_rars - 1));
}

TEST_F(sched_result_grid_test, pooled_pdus_of_a_slot_are_not_seen_by_other_slots)
{
  ASSERT_NE(grid.get_builder(test_slot_idx).dl().rar_grants().emplace_back(), nullptr);

  const sched_result_grid& cgrid = grid;
  for (unsigned i = 0; i != nof_test_slots; ++i) {
    const size_t expected_size = (i == test_slot_idx) ? 1 : 0;
    ASSERT_EQ(cgrid.get_reader(i).dl().rar_grants().size(), expected_size);
  }
}

TEST_F(sched_result_grid_test, pooled_pdus_of_different_slots_do_not_alias)
{
  static constexpr unsigned other_slot_idx = test_slot_idx + 1;

  rar_information* first = grid.get_builder(test_slot_idx).dl().rar_grants().emplace_back();
  ASSERT_NE(first, nullptr);
  first->pdsch_cfg.rnti = to_rnti(0x0001);

  rar_information* second = grid.get_builder(other_slot_idx).dl().rar_grants().emplace_back();
  ASSERT_NE(second, nullptr);
  second->pdsch_cfg.rnti = to_rnti(0x0002);

  ASSERT_NE(first, second);
  ASSERT_EQ(first->pdsch_cfg.rnti, to_rnti(0x0001));
}

TEST_F(sched_result_grid_test, when_pooled_list_is_full_then_emplace_back_returns_nullptr)
{
  auto rar_grants = grid.get_builder(test_slot_idx).dl().rar_grants();
  for (unsigned i = 0; i != MAX_RAR_PDUS_PER_SLOT; ++i) {
    ASSERT_NE(rar_grants.emplace_back(), nullptr);
  }

  ASSERT_TRUE(rar_grants.full());
  ASSERT_EQ(rar_grants.emplace_back(), nullptr);
  ASSERT_EQ(rar_grants.size(), MAX_RAR_PDUS_PER_SLOT);
}

TEST_F(sched_result_grid_test, when_pool_is_exhausted_then_emplace_back_returns_nullptr)
{
  // A pool sized for a single slot is exhausted by that slot, so the next slot cannot allocate.
  sched_result_grid small_pool_grid(sched_result_grid_config{nof_test_slots, 1});

  auto first_slot = small_pool_grid.get_builder(0).dl().rar_grants();
  for (unsigned i = 0; i != MAX_RAR_PDUS_PER_SLOT; ++i) {
    ASSERT_NE(first_slot.emplace_back(), nullptr);
  }

  auto second_slot = small_pool_grid.get_builder(1).dl().rar_grants();
  ASSERT_TRUE(second_slot.empty());
  ASSERT_TRUE(second_slot.full());
  ASSERT_EQ(second_slot.emplace_back(), nullptr);
}

TEST_F(sched_result_grid_test, when_slot_is_reset_then_its_pooled_pdus_return_to_the_pool)
{
  sched_result_grid small_pool_grid(sched_result_grid_config{nof_test_slots, 1});

  auto first_slot = small_pool_grid.get_builder(0).dl().rar_grants();
  for (unsigned i = 0; i != MAX_RAR_PDUS_PER_SLOT; ++i) {
    ASSERT_NE(first_slot.emplace_back(), nullptr);
  }
  ASSERT_TRUE(small_pool_grid.get_builder(1).dl().rar_grants().full());

  small_pool_grid.reset_slot(0, 0, 0);

  ASSERT_TRUE(small_pool_grid.get_builder(0).dl().rar_grants().empty());
  ASSERT_FALSE(small_pool_grid.get_builder(1).dl().rar_grants().full());
  ASSERT_NE(small_pool_grid.get_builder(1).dl().rar_grants().emplace_back(), nullptr);
}

TEST_F(sched_result_grid_test, when_pooled_pdu_is_popped_then_it_returns_to_the_pool)
{
  sched_result_grid small_pool_grid(sched_result_grid_config{nof_test_slots, 1});

  auto first_slot = small_pool_grid.get_builder(0).dl().rar_grants();
  for (unsigned i = 0; i != MAX_RAR_PDUS_PER_SLOT; ++i) {
    ASSERT_NE(first_slot.emplace_back(), nullptr);
  }
  ASSERT_TRUE(small_pool_grid.get_builder(1).dl().rar_grants().full());

  first_slot.pop_back();
  ASSERT_EQ(first_slot.size(), MAX_RAR_PDUS_PER_SLOT - 1);
  ASSERT_NE(small_pool_grid.get_builder(1).dl().rar_grants().emplace_back(), nullptr);
}
