// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/mac/rnti_manager.h"
#include <gtest/gtest.h>

using namespace ocudu;

TEST(rnti_manager_test, when_allocate_rnti_called_multiple_times_then_rntis_are_unique)
{
  rnti_manager rnti_db;

  unsigned         max_count = 100;
  std::set<rnti_t> prev_rntis;
  for (unsigned count = 0; count != max_count; ++count) {
    rnti_t rnti = rnti_db.allocate();
    ASSERT_EQ(prev_rntis.count(rnti), 0);
    prev_rntis.insert(rnti);
  }

  ASSERT_EQ(rnti_db.nof_ues(), 0) << "No UE should have been added";
}

TEST(rnti_manager_test, when_ue_added_then_allocate_rnti_does_not_repeat_rnti)
{
  rnti_manager rnti_db;

  rnti_t rnti1 = rnti_db.allocate();
  ASSERT_TRUE(rnti_db.add_ue(rnti1, to_du_ue_index(0)));

  rnti_t rnti2 = rnti_db.allocate();
  ASSERT_NE(rnti1, rnti2);

  ASSERT_EQ(rnti_db.nof_ues(), 1);
}

TEST(rnti_manager_test, when_cs_rnti_added_then_nof_ues_is_not_incremented)
{
  rnti_manager rnti_db;

  // Add a regular C-RNTI.
  rnti_t crnti = rnti_db.allocate();
  ASSERT_TRUE(rnti_db.add_ue(crnti, to_du_ue_index(0)));
  ASSERT_EQ(rnti_db.nof_ues(), 1);

  // Add a CS-RNTI for the same UE. The counter should not increase.
  rnti_t cs_rnti = rnti_db.allocate(true);
  ASSERT_NE(cs_rnti, rnti_t::INVALID_RNTI);
  ASSERT_TRUE(rnti_db.add_ue(cs_rnti, to_du_ue_index(0), true));
  ASSERT_EQ(rnti_db.nof_ues(), 1) << "CS-RNTI should not increase the C-RNTI count";

  // Remove the CS-RNTI. The counter should remain unchanged.
  rnti_db.rem_ue(cs_rnti, true);
  ASSERT_EQ(rnti_db.nof_ues(), 1) << "Removing CS-RNTI should not decrease the C-RNTI count";
  ASSERT_FALSE(rnti_db.has_rnti(cs_rnti));

  // Remove the C-RNTI. The counter should decrease.
  rnti_db.rem_ue(crnti);
  ASSERT_EQ(rnti_db.nof_ues(), 0);
}
