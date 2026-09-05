// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/ngap/ue_context/ngap_ue_context.h"
#include "tests/test_doubles/utils/test_rng.h"
#include "ocudu/adt/format.h"
#include "ocudu/support/executors/manual_task_worker.h"
#include <cstdint>
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocucp;

// Test class.
class ngap_ue_context_list_test : public ngap_ue_context_list
{
public:
  ngap_ue_context_list_test(timer_factory& timers_, ocudulog::basic_logger& logger_) :
    ngap_ue_context_list(timers_, logger_)
  {
  }
};

/// Fixture class for NGAP UE context
class ngap_ue_context_test : public ::testing::Test
{
protected:
  ngap_ue_context_test()
  {
    ngap_logger.set_level(ocudulog::basic_levels::debug);
    ocudulog::init();
  }
  ~ngap_ue_context_test()
  {
    // flush logger after each test
    ocudulog::flush();
  }

  cu_cp_ue_index_t generate_random_ue_index()
  {
    return uint_to_ue_index(test_rng::uniform_int<uint64_t>(to_underlying(cu_cp_ue_index_t::min),
                                                            to_underlying(cu_cp_ue_index_t::max) - 1));
  }

  ran_ue_id_t generate_random_ran_ue_id()
  {
    return uint_to_ran_ue_id(
        test_rng::uniform_int<uint64_t>(to_underlying(ran_ue_id_t::min), to_underlying(ran_ue_id_t::max) - 1));
  }

  ocudulog::basic_logger&      ngap_logger = ocudulog::fetch_basic_logger("NGAP");
  timer_manager                timer_mng;
  manual_task_worker           ctrl_worker{128};
  timer_factory                timers{timer_mng, ctrl_worker};
  dummy_ngap_cu_cp_ue_notifier ue_notifier;
  ngap_ue_context_list_test    ue_ctxt_list{timers, ngap_logger};
};

TEST_F(ngap_ue_context_test, when_ue_added_then_ue_exists)
{
  cu_cp_ue_index_t ue_index  = generate_random_ue_index();
  ran_ue_id_t      ran_ue_id = generate_random_ran_ue_id();

  ue_ctxt_list.add_ue(ue_index, ran_ue_id, ue_notifier);

  ASSERT_TRUE(ue_ctxt_list.contains(ran_ue_id));
  ASSERT_TRUE(ue_ctxt_list.contains(ue_index));

  ASSERT_EQ(ue_ctxt_list[ran_ue_id].ue_ids.ran_ue_id, ran_ue_id);
  ASSERT_EQ(ue_ctxt_list[ran_ue_id].ue_ids.ue_index, ue_index);
  ASSERT_EQ(ue_ctxt_list[ue_index].ue_ids.ran_ue_id, ran_ue_id);
  ASSERT_EQ(ue_ctxt_list[ue_index].ue_ids.ue_index, ue_index);
}

TEST_F(ngap_ue_context_test, when_ue_not_added_then_ue_doesnt_exist)
{
  cu_cp_ue_index_t ue_index  = generate_random_ue_index();
  ran_ue_id_t      ran_ue_id = generate_random_ran_ue_id();

  ASSERT_FALSE(ue_ctxt_list.contains(ran_ue_id));
  ASSERT_FALSE(ue_ctxt_list.contains(ue_index));
}

TEST_F(ngap_ue_context_test, when_ue_exists_then_removal_succeeds)
{
  cu_cp_ue_index_t ue_index  = generate_random_ue_index();
  ran_ue_id_t      ran_ue_id = generate_random_ran_ue_id();

  ue_ctxt_list.add_ue(ue_index, ran_ue_id, ue_notifier);

  // test removal
  ue_ctxt_list.remove_ue_context(ue_index);

  ASSERT_FALSE(ue_ctxt_list.contains(ran_ue_id));
  ASSERT_FALSE(ue_ctxt_list.contains(ue_index));
}

TEST_F(ngap_ue_context_test, when_ue_is_added_then_next_ue_id_is_increased)
{
  cu_cp_ue_index_t ue_index  = generate_random_ue_index();
  ran_ue_id_t      ran_ue_id = ue_ctxt_list.allocate_ran_ue_id();

  ASSERT_EQ((unsigned)ran_ue_id, (unsigned)ran_ue_id_t::min);

  ue_ctxt_list.add_ue(ue_index, ran_ue_id, ue_notifier);

  // remove ue
  ue_ctxt_list.remove_ue_context(ue_index);

  ASSERT_FALSE(ue_ctxt_list.contains(ran_ue_id));
  ASSERT_FALSE(ue_ctxt_list.contains(ue_index));

  ASSERT_EQ((unsigned)ue_ctxt_list.allocate_ran_ue_id(), (unsigned)ran_ue_id_t::min + 1);
}
