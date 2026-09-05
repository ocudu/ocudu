// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "f1ap_cu_test_helpers.h"
#include "lib/f1ap/cu_cp/ue_context/f1ap_cu_ue_context.h"
#include "tests/test_doubles/utils/test_rng.h"
#include "ocudu/adt/format.h"
#include "ocudu/f1ap/f1ap_ue_id_types.h"
#include "ocudu/support/executors/manual_task_worker.h"
#include <cstdint>
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocucp;

// test class
class f1ap_ue_context_list_test : public f1ap_ue_context_list
{
public:
  f1ap_ue_context_list_test(timer_factory timers_, ocudulog::basic_logger& logger_) :
    f1ap_ue_context_list(timers_, logger_)
  {
  }

  gnb_cu_ue_f1ap_id_t allocate_cu_ue_f1ap_id() { return next_cu_ue_f1ap_id; }

  void set_next_cu_ue_f1ap_id(gnb_cu_ue_f1ap_id_t next_id) { next_cu_ue_f1ap_id = next_id; }
};

/// Fixture class for F1AP UE context
class f1ap_cu_ue_context_test : public ::testing::Test
{
protected:
  f1ap_cu_ue_context_test()
  {
    f1ap_logger.set_level(ocudulog::basic_levels::debug);
    ocudulog::init();
  }
  ~f1ap_cu_ue_context_test()
  {
    // flush logger after each test
    ocudulog::flush();
  }

  cu_cp_ue_index_t generate_random_ue_index()
  {
    return uint_to_ue_index(test_rng::uniform_int<uint64_t>(to_underlying(cu_cp_ue_index_t::min),
                                                            to_underlying(cu_cp_ue_index_t::max) - 1));
  }

  ocudulog::basic_logger&   f1ap_logger = ocudulog::fetch_basic_logger("CU-CP-F1");
  timer_manager             timer_mng;
  manual_task_worker        ctrl_worker{128};
  timer_factory             timers{timer_mng, ctrl_worker};
  f1ap_ue_context_list_test ue_ctxt_list{timers, f1ap_logger};
};

TEST_F(f1ap_cu_ue_context_test, when_ue_added_then_ue_exists)
{
  cu_cp_ue_index_t    ue_index      = generate_random_ue_index();
  gnb_cu_ue_f1ap_id_t cu_ue_f1ap_id = generate_random_gnb_cu_ue_f1ap_id();

  ue_ctxt_list.add_ue(ue_index, cu_ue_f1ap_id);

  ASSERT_TRUE(ue_ctxt_list.contains(cu_ue_f1ap_id));
  ASSERT_TRUE(ue_ctxt_list.contains(ue_index));

  ASSERT_EQ(ue_ctxt_list[cu_ue_f1ap_id].ue_ids.cu_ue_f1ap_id, cu_ue_f1ap_id);
  ASSERT_EQ(ue_ctxt_list[cu_ue_f1ap_id].ue_ids.ue_index, ue_index);
  ASSERT_EQ(ue_ctxt_list[ue_index].ue_ids.cu_ue_f1ap_id, cu_ue_f1ap_id);
  ASSERT_EQ(ue_ctxt_list[ue_index].ue_ids.ue_index, ue_index);
}

TEST_F(f1ap_cu_ue_context_test, when_ue_not_added_then_ue_doesnt_exist)
{
  cu_cp_ue_index_t    ue_index      = generate_random_ue_index();
  gnb_cu_ue_f1ap_id_t cu_ue_f1ap_id = generate_random_gnb_cu_ue_f1ap_id();

  ASSERT_FALSE(ue_ctxt_list.contains(cu_ue_f1ap_id));
  ASSERT_FALSE(ue_ctxt_list.contains(ue_index));
}

TEST_F(f1ap_cu_ue_context_test, when_ue_exists_then_removal_succeeds)
{
  cu_cp_ue_index_t    ue_index      = generate_random_ue_index();
  gnb_cu_ue_f1ap_id_t cu_ue_f1ap_id = generate_random_gnb_cu_ue_f1ap_id();

  ue_ctxt_list.add_ue(ue_index, cu_ue_f1ap_id);

  // test removal
  ue_ctxt_list.remove_ue(ue_index);

  ASSERT_FALSE(ue_ctxt_list.contains(cu_ue_f1ap_id));
  ASSERT_FALSE(ue_ctxt_list.contains(ue_index));
}

TEST_F(f1ap_cu_ue_context_test, when_ue_is_added_then_next_ue_id_is_increased)
{
  cu_cp_ue_index_t    ue_index      = generate_random_ue_index();
  gnb_cu_ue_f1ap_id_t cu_ue_f1ap_id = ue_ctxt_list.allocate_gnb_cu_ue_f1ap_id();

  ASSERT_EQ((unsigned)cu_ue_f1ap_id, (unsigned)gnb_cu_ue_f1ap_id_t::min);

  ue_ctxt_list.add_ue(ue_index, cu_ue_f1ap_id);

  // remove ue
  ue_ctxt_list.remove_ue(ue_index);

  ASSERT_FALSE(ue_ctxt_list.contains(cu_ue_f1ap_id));
  ASSERT_FALSE(ue_ctxt_list.contains(ue_index));

  ASSERT_EQ((unsigned)ue_ctxt_list.allocate_gnb_cu_ue_f1ap_id(), (unsigned)gnb_cu_ue_f1ap_id_t::min + 1);
}
