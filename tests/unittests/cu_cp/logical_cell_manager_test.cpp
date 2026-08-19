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
    declared_cells({cu_cp_logical_cell_config{declared_nci, /* admin_locked = */ false, /* barred = */ false}}),
    mng(declared_cells)
  {
  }

  std::vector<cu_cp_logical_cell_config> declared_cells;
  logical_cell_manager                   mng;
};

TEST_F(logical_cell_manager_test, when_nci_is_unknown_then_transitions_report_failure_and_registry_is_unchanged)
{
  EXPECT_FALSE(mng.set_admin_locked(unknown_nci, true).has_value());
  EXPECT_FALSE(mng.set_barred(unknown_nci, true).has_value());
  EXPECT_EQ(mng.find_cell(unknown_nci), nullptr) << "a failed transition must not create a cell";
}

TEST_F(logical_cell_manager_test, when_intent_is_set_then_previous_intent_is_returned_and_state_updated)
{
  std::optional<bool> prev = mng.set_admin_locked(declared_nci, true);
  ASSERT_TRUE(prev.has_value());
  EXPECT_FALSE(*prev);
  ASSERT_NE(mng.find_cell(declared_nci), nullptr);
  EXPECT_TRUE(mng.find_cell(declared_nci)->admin_locked);

  prev = mng.set_barred(declared_nci, true);
  ASSERT_TRUE(prev.has_value());
  EXPECT_FALSE(*prev);
  EXPECT_TRUE(mng.find_cell(declared_nci)->barred);

  // Restoring the returned previous intent round-trips the state.
  prev = mng.set_admin_locked(declared_nci, false);
  ASSERT_TRUE(prev.has_value());
  EXPECT_TRUE(*prev);
  EXPECT_FALSE(mng.find_cell(declared_nci)->admin_locked);
}

TEST_F(logical_cell_manager_test, when_du_is_derealized_then_intent_is_kept)
{
  const cu_cp_du_index_t du_index = uint_to_cu_cp_du_index(0);
  mng.realize_cell(declared_nci, du_index);
  ASSERT_TRUE(mng.set_admin_locked(declared_nci, true).has_value());

  mng.derealize_du_cells(du_index);

  ASSERT_NE(mng.find_cell(declared_nci), nullptr);
  EXPECT_FALSE(mng.find_cell(declared_nci)->realized);
  EXPECT_TRUE(mng.find_cell(declared_nci)->admin_locked) << "operator intent must survive DU removal";
}
