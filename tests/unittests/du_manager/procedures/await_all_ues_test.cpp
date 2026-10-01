// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief Unit tests for the await_all_ues helper used by du_ue_reset_procedure/du_stop_procedure.

#include "du_manager_procedure_test_helpers.h"
#include "lib/du/du_high/du_manager/procedures/await_all_ues.h"
#include "ocudu/adt/format.h"
#include "ocudu/du/du_cell_config_helpers.h"
#include "ocudu/support/async/async_no_op_task.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace odu;

class await_all_ues_tester : public du_manager_proc_tester, public ::testing::Test
{
protected:
  await_all_ues_tester() :
    du_manager_proc_tester(std::vector<du_cell_config>{config_helpers::make_default_du_cell_config()})
  {
  }
};

// The UEs' shared task scheduler in ue_manager_dummy is idle right after creation, so the first task scheduled onto
// it runs synchronously (async_queue::try_push resumes an idle scheduler loop inline). This reproduces the condition
// under which await_all_ues used to lose track of not-yet-scheduled UEs.
TEST_F(await_all_ues_tester, when_first_ue_task_completes_synchronously_then_second_ue_task_still_runs)
{
  du_ue& ue1 = create_ue(to_du_ue_index(0));
  du_ue& ue2 = create_ue(to_du_ue_index(1));

  std::vector<du_ue_index_t> ues_run;
  std::vector<du_ue_index_t> ues_to_update{ue1.ue_index, ue2.ue_index};

  async_task<void>         t = await_all_ues(ue_mng, ues_to_update, [&ues_run](du_ue& u) {
    ues_run.push_back(u.ue_index);
    return launch_no_op_task();
  });
  lazy_task_launcher<void> launcher{t};

  ASSERT_TRUE(launcher.ready()) << "await_all_ues never completed. The second UE's task was likely never scheduled";
  ASSERT_EQ(ues_run.size(), 2U);
  EXPECT_NE(std::find(ues_run.begin(), ues_run.end(), ue1.ue_index), ues_run.end());
  EXPECT_NE(std::find(ues_run.begin(), ues_run.end(), ue2.ue_index), ues_run.end());
}

TEST_F(await_all_ues_tester, when_single_ue_task_completes_synchronously_then_await_all_ues_completes)
{
  du_ue& ue = create_ue(to_du_ue_index(0));

  unsigned                   nof_runs = 0;
  std::vector<du_ue_index_t> ues_to_update{ue.ue_index};

  async_task<void>         t = await_all_ues(ue_mng, ues_to_update, [&nof_runs](du_ue& /* u */) {
    nof_runs++;
    return launch_no_op_task();
  });
  lazy_task_launcher<void> launcher{t};

  ASSERT_TRUE(launcher.ready());
  ASSERT_EQ(nof_runs, 1U);
}
