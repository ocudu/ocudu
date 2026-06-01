// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "../common/test_helpers.h"
#include "lib/e1ap/cu_cp/ue_context/e1ap_cu_cp_ue_context.h"
#include "tests/test_doubles/utils/test_rng.h"
#include "ocudu/ocudulog/logger.h"
#include "ocudu/ran/cu_cp_types.h"
#include "ocudu/support/executors/manual_task_worker.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocucp;

/// Fixture class for E1AP UE context
class e1ap_cu_cp_ue_context_test : public ::testing::Test
{
protected:
  e1ap_cu_cp_ue_context_test()
  {
    e1ap_logger.set_level(ocudulog::basic_levels::debug);
    ocudulog::init();
  }

  ~e1ap_cu_cp_ue_context_test()
  {
    // flush logger after each test
    ocudulog::flush();
  }

  cu_cp_ue_index_t generate_random_ue_index()
  {
    return uint_to_ue_index(test_rng::uniform_int<uint64_t>(cu_cp_ue_index_to_uint(cu_cp_ue_index_t::min),
                                                            cu_cp_ue_index_to_uint(cu_cp_ue_index_t::max) - 1));
  }

  timer_manager           timer_mng;
  ocudulog::basic_logger& e1ap_logger = ocudulog::fetch_basic_logger("CU-CP-E1");
  manual_task_worker      ctrl_worker{128};
  timer_factory           timers{timer_mng, ctrl_worker};
  unsigned                max_nof_supported_ues = 1024 * 4;
  e1ap_ue_context_list    ue_ctxt_list{timers, max_nof_supported_ues, e1ap_logger};
};

TEST_F(e1ap_cu_cp_ue_context_test, when_ue_added_then_ue_exists)
{
  cu_cp_ue_index_t       ue_index         = generate_random_ue_index();
  gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id = generate_random_gnb_cu_cp_ue_e1ap_id();

  ASSERT_NE(ue_ctxt_list.add_ue(ue_index, cu_cp_ue_e1ap_id), nullptr);

  ASSERT_TRUE(ue_ctxt_list.contains(cu_cp_ue_e1ap_id));
  ASSERT_TRUE(ue_ctxt_list.contains(ue_index));

  ASSERT_EQ(ue_ctxt_list[cu_cp_ue_e1ap_id].ue_ids.cu_cp_ue_e1ap_id, cu_cp_ue_e1ap_id);
  ASSERT_EQ(ue_ctxt_list[cu_cp_ue_e1ap_id].ue_ids.ue_index, ue_index);
  ASSERT_EQ(ue_ctxt_list[ue_index].ue_ids.cu_cp_ue_e1ap_id, cu_cp_ue_e1ap_id);
  ASSERT_EQ(ue_ctxt_list[ue_index].ue_ids.ue_index, ue_index);
}

TEST_F(e1ap_cu_cp_ue_context_test, when_ue_not_added_then_ue_doesnt_exist)
{
  cu_cp_ue_index_t       ue_index         = generate_random_ue_index();
  gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id = generate_random_gnb_cu_cp_ue_e1ap_id();

  ASSERT_FALSE(ue_ctxt_list.contains(cu_cp_ue_e1ap_id));
  ASSERT_FALSE(ue_ctxt_list.contains(ue_index));
  ASSERT_EQ(ue_ctxt_list.find_ue(cu_cp_ue_e1ap_id), nullptr);
}

TEST_F(e1ap_cu_cp_ue_context_test, when_ue_exists_then_ue_not_added)
{
  cu_cp_ue_index_t       ue_index         = generate_random_ue_index();
  gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id = generate_random_gnb_cu_cp_ue_e1ap_id();

  ASSERT_NE(ue_ctxt_list.add_ue(ue_index, cu_cp_ue_e1ap_id), nullptr);

  ASSERT_TRUE(ue_ctxt_list.contains(cu_cp_ue_e1ap_id));
  ASSERT_TRUE(ue_ctxt_list.contains(ue_index));

  ASSERT_EQ(ue_ctxt_list[cu_cp_ue_e1ap_id].ue_ids.cu_cp_ue_e1ap_id, cu_cp_ue_e1ap_id);
  ASSERT_EQ(ue_ctxt_list[cu_cp_ue_e1ap_id].ue_ids.ue_index, ue_index);
  ASSERT_EQ(ue_ctxt_list[ue_index].ue_ids.cu_cp_ue_e1ap_id, cu_cp_ue_e1ap_id);
  ASSERT_EQ(ue_ctxt_list[ue_index].ue_ids.ue_index, ue_index);

  ASSERT_EQ(ue_ctxt_list.size(), 1U);

  // Try to add UE with the same UE index again
  ASSERT_EQ(ue_ctxt_list.add_ue(ue_index, generate_random_gnb_cu_cp_ue_e1ap_id()), nullptr);
  ASSERT_EQ(ue_ctxt_list.size(), 1U);
}

TEST_F(e1ap_cu_cp_ue_context_test, when_unsupported_number_of_ues_added_then_ue_not_added)
{
  // Add maximum number of supported UEs
  e1ap_logger.set_level(ocudulog::basic_levels::error);
  for (unsigned it = 0; it < max_nof_supported_ues; ++it) {
    gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id = ue_ctxt_list.allocate_gnb_cu_cp_ue_e1ap_id();
    ASSERT_NE(cu_cp_ue_e1ap_id, gnb_cu_cp_ue_e1ap_id_t::invalid);
    cu_cp_ue_index_t ue_index = uint_to_ue_index(it);

    ASSERT_NE(ue_ctxt_list.add_ue(ue_index, cu_cp_ue_e1ap_id), nullptr);
    ASSERT_TRUE(ue_ctxt_list.contains(cu_cp_ue_e1ap_id));
    ASSERT_TRUE(ue_ctxt_list.contains(ue_index));
  }
  e1ap_logger.set_level(ocudulog::basic_levels::debug);

  // Try to get another cu_cp_ue_e1ap_id (should fail)
  ASSERT_EQ(ue_ctxt_list.allocate_gnb_cu_cp_ue_e1ap_id(), gnb_cu_cp_ue_e1ap_id_t::invalid);
}

TEST_F(e1ap_cu_cp_ue_context_test, when_ue_exists_then_removal_succeeds)
{
  cu_cp_ue_index_t       ue_index         = generate_random_ue_index();
  gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id = generate_random_gnb_cu_cp_ue_e1ap_id();

  ASSERT_NE(ue_ctxt_list.add_ue(ue_index, cu_cp_ue_e1ap_id), nullptr);

  ue_ctxt_list.remove_ue(ue_index);

  ASSERT_FALSE(ue_ctxt_list.contains(cu_cp_ue_e1ap_id));
  ASSERT_FALSE(ue_ctxt_list.contains(ue_index));
}

TEST_F(e1ap_cu_cp_ue_context_test, when_ue_index_is_old_ue_index_then_ue_index_not_updated)
{
  cu_cp_ue_index_t       old_ue_index     = generate_random_ue_index();
  gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id = generate_random_gnb_cu_cp_ue_e1ap_id();

  ASSERT_NE(ue_ctxt_list.add_ue(old_ue_index, cu_cp_ue_e1ap_id), nullptr);

  // new ue index to update
  cu_cp_ue_index_t ue_index = old_ue_index;

  // test ue index update
  ue_ctxt_list.update_ue_index(ue_index, old_ue_index);

  ASSERT_TRUE(ue_ctxt_list.contains(old_ue_index));
  ASSERT_TRUE(ue_ctxt_list.contains(cu_cp_ue_e1ap_id));
  ASSERT_EQ(&ue_ctxt_list[old_ue_index], &ue_ctxt_list[cu_cp_ue_e1ap_id]);
}

TEST_F(e1ap_cu_cp_ue_context_test, when_ue_exists_then_ue_index_update_succeeds)
{
  cu_cp_ue_index_t       old_ue_index     = generate_random_ue_index();
  gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id = generate_random_gnb_cu_cp_ue_e1ap_id();

  ASSERT_NE(ue_ctxt_list.add_ue(old_ue_index, cu_cp_ue_e1ap_id), nullptr);

  // new ue index to update
  cu_cp_ue_index_t ue_index = generate_random_ue_index();

  // make sure ue index is not old ue index
  while (ue_index == old_ue_index) {
    ue_index = generate_random_ue_index();
  }

  // test ue index update
  ue_ctxt_list.update_ue_index(ue_index, old_ue_index);

  ASSERT_TRUE(ue_ctxt_list.contains(ue_index));
  ASSERT_TRUE(ue_ctxt_list.contains(cu_cp_ue_e1ap_id));
  ASSERT_FALSE(ue_ctxt_list.contains(old_ue_index));
  ASSERT_EQ(&ue_ctxt_list[ue_index], &ue_ctxt_list[cu_cp_ue_e1ap_id]);
  ASSERT_EQ(ue_ctxt_list[ue_index].ue_ids.cu_cp_ue_e1ap_id, cu_cp_ue_e1ap_id);
  ASSERT_EQ(ue_ctxt_list[ue_index].ue_ids.ue_index, ue_index);
}
