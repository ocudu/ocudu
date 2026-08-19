// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/cu_cp/logical_cell_manager.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocucp;

static const nr_cell_identity declared_nci = nr_cell_identity::create(gnb_id_t{411, 22}, 0).value();
static const nr_cell_identity unknown_nci  = nr_cell_identity::create(gnb_id_t{411, 22}, 1).value();

class logical_cell_manager_test : public ::testing::Test
{
protected:
  logical_cell_manager_test() :
    declared_cells({cu_cp_logical_cell_config{declared_nci, cell_admin_state::unlocked, /* barred = */ false}}),
    mng(declared_cells)
  {
  }

  std::vector<cu_cp_logical_cell_config> declared_cells;
  logical_cell_manager                   mng;
};

TEST_F(logical_cell_manager_test, when_nci_is_unknown_then_transitions_report_failure_and_registry_is_unchanged)
{
  EXPECT_FALSE(mng.set_admin_state(unknown_nci, cell_admin_state::locked).has_value());
  EXPECT_FALSE(mng.set_barred(unknown_nci, true).has_value());
  EXPECT_FALSE(mng.set_operational_state(unknown_nci, cell_operational_state::enabled).has_value());
  EXPECT_EQ(mng.find_cell(unknown_nci), nullptr) << "a failed transition must not create a cell";
}

TEST_F(logical_cell_manager_test, when_admin_state_is_set_then_previous_state_is_returned_and_state_updated)
{
  // The graceful-stop sequence: unlocked -> shutting_down -> locked, each step returning the prior state.
  std::optional<cell_admin_state> prev = mng.set_admin_state(declared_nci, cell_admin_state::shutting_down);
  ASSERT_TRUE(prev.has_value());
  EXPECT_EQ(*prev, cell_admin_state::unlocked);
  ASSERT_NE(mng.find_cell(declared_nci), nullptr);
  EXPECT_EQ(mng.find_cell(declared_nci)->admin_state, cell_admin_state::shutting_down);

  prev = mng.set_admin_state(declared_nci, cell_admin_state::locked);
  ASSERT_TRUE(prev.has_value());
  EXPECT_EQ(*prev, cell_admin_state::shutting_down);
  EXPECT_EQ(mng.find_cell(declared_nci)->admin_state, cell_admin_state::locked);

  // Restoring the returned previous state round-trips (the failed-command revert path).
  prev = mng.set_admin_state(declared_nci, cell_admin_state::shutting_down);
  ASSERT_TRUE(prev.has_value());
  ASSERT_TRUE(mng.set_admin_state(declared_nci, *prev).has_value());
  EXPECT_EQ(mng.find_cell(declared_nci)->admin_state, cell_admin_state::locked);
}

TEST_F(logical_cell_manager_test, when_barred_intent_is_set_then_previous_intent_is_returned_and_state_updated)
{
  std::optional<bool> prev = mng.set_barred(declared_nci, true);
  ASSERT_TRUE(prev.has_value());
  EXPECT_FALSE(*prev);
  EXPECT_TRUE(mng.find_cell(declared_nci)->barred);

  prev = mng.set_barred(declared_nci, false);
  ASSERT_TRUE(prev.has_value());
  EXPECT_TRUE(*prev);
  EXPECT_FALSE(mng.find_cell(declared_nci)->barred);
}

TEST_F(logical_cell_manager_test, when_operational_state_is_set_then_previous_state_is_returned_and_state_updated)
{
  // Cells start operationally disabled: nothing has activated them yet.
  EXPECT_EQ(mng.find_cell(declared_nci)->operational_state, cell_operational_state::disabled);

  std::optional<cell_operational_state> prev = mng.set_operational_state(declared_nci, cell_operational_state::enabled);
  ASSERT_TRUE(prev.has_value());
  EXPECT_EQ(*prev, cell_operational_state::disabled);
  EXPECT_EQ(mng.find_cell(declared_nci)->operational_state, cell_operational_state::enabled);
}

TEST_F(logical_cell_manager_test, when_admin_state_changes_then_operational_state_is_untouched)
{
  // An unlocked cell whose activation failed keeps operational_state disabled: the two axes are independent,
  // which is what makes that cell distinguishable from an active one.
  ASSERT_TRUE(mng.set_operational_state(declared_nci, cell_operational_state::disabled).has_value());
  ASSERT_TRUE(mng.set_admin_state(declared_nci, cell_admin_state::unlocked).has_value());
  EXPECT_EQ(mng.find_cell(declared_nci)->admin_state, cell_admin_state::unlocked);
  EXPECT_EQ(mng.find_cell(declared_nci)->operational_state, cell_operational_state::disabled);
}

TEST_F(logical_cell_manager_test, when_du_is_derealized_then_intent_is_kept_and_cell_becomes_disabled)
{
  const cu_cp_du_index_t du_index = uint_to_cu_cp_du_index(0);
  mng.realize_cell(declared_nci, du_index);
  ASSERT_TRUE(mng.set_operational_state(declared_nci, cell_operational_state::enabled).has_value());
  ASSERT_TRUE(mng.set_admin_state(declared_nci, cell_admin_state::locked).has_value());
  ASSERT_TRUE(mng.set_barred(declared_nci, true).has_value());

  mng.derealize_du_cells(du_index);

  const logical_cell* cell = mng.find_cell(declared_nci);
  ASSERT_NE(cell, nullptr);
  EXPECT_FALSE(cell->realized);
  EXPECT_EQ(cell->operational_state, cell_operational_state::disabled) << "no DU serves a de-realized cell";
  EXPECT_EQ(cell->admin_state, cell_admin_state::locked) << "operator intent must survive DU removal";
  EXPECT_TRUE(cell->barred) << "operator intent must survive DU removal";
}

TEST_F(logical_cell_manager_test, when_cells_are_declared_then_undeclared_realized_cell_comes_up_locked)
{
  // The declared set acts as the activation whitelist: a reported cell outside it is realized locked.
  const cu_cp_du_index_t du_index = uint_to_cu_cp_du_index(0);
  const logical_cell&    cell     = mng.realize_cell(unknown_nci, du_index);
  EXPECT_EQ(cell.admin_state, cell_admin_state::locked);
  EXPECT_EQ(cell.operational_state, cell_operational_state::disabled);
}

TEST(logical_cell_manager_seed_test, when_cell_is_declared_locked_then_it_seeds_locked)
{
  std::vector<cu_cp_logical_cell_config> declared = {
      cu_cp_logical_cell_config{declared_nci, cell_admin_state::locked, /* barred = */ true}};
  logical_cell_manager mng(declared);

  const logical_cell* cell = mng.find_cell(declared_nci);
  ASSERT_NE(cell, nullptr);
  EXPECT_EQ(cell->admin_state, cell_admin_state::locked);
  EXPECT_TRUE(cell->barred);
  EXPECT_EQ(cell->operational_state, cell_operational_state::disabled);
}

TEST(logical_cell_manager_seed_test, when_no_cells_are_declared_then_dynamic_cell_comes_up_unlocked)
{
  logical_cell_manager mng({});
  const logical_cell&  cell = mng.realize_cell(unknown_nci, uint_to_cu_cp_du_index(0));
  EXPECT_EQ(cell.admin_state, cell_admin_state::unlocked);
}
