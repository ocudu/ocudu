// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/nrppa/ue_context/nrppa_ue_context.h"
#include "test_helpers.h"
#include "tests/test_doubles/utils/test_rng.h"
#include "ocudu/support/executors/manual_task_worker.h"
#include <cstdint>
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocucp;

// Test class.
class nrppa_ue_context_list_test : public nrppa_ue_context_list
{
public:
  nrppa_ue_context_list_test(ocudulog::basic_logger& logger_) : nrppa_ue_context_list(logger_) {}
};

/// Fixture class for NRPPA UE context.
class nrppa_ue_context_test : public ::testing::Test
{
protected:
  nrppa_ue_context_test()
  {
    nrppa_logger.set_level(ocudulog::basic_levels::debug);
    ocudulog::init();
  }

  ~nrppa_ue_context_test()
  {
    // Flush logger after each test.
    ocudulog::flush();
  }

  static cu_cp_ue_index_t generate_random_ue_index()
  {
    return uint_to_ue_index(test_rng::uniform_int<uint64_t>(cu_cp_ue_index_to_uint(cu_cp_ue_index_t::min),
                                                            cu_cp_ue_index_to_uint(cu_cp_ue_index_t::max) - 1));
  }

  static ran_ue_meas_id_t generate_random_ran_ue_meas_id()
  {
    return uint_to_ran_ue_meas_id(test_rng::uniform_int<uint64_t>(ran_ue_meas_id_to_uint(ran_ue_meas_id_t::min),
                                                                  ran_ue_meas_id_to_uint(ran_ue_meas_id_t::max) - 1));
  }

  static lmf_ue_meas_id_t generate_random_lmf_ue_meas_id()
  {
    return uint_to_lmf_ue_meas_id(test_rng::uniform_int<uint64_t>(lmf_ue_meas_id_to_uint(lmf_ue_meas_id_t::min),
                                                                  lmf_ue_meas_id_to_uint(lmf_ue_meas_id_t::max) - 1));
  }

  ocudulog::basic_logger&       nrppa_logger = ocudulog::fetch_basic_logger("nrppa");
  timer_manager                 timer_mng;
  manual_task_worker            ctrl_worker{128};
  timer_factory                 timers{timer_mng, ctrl_worker};
  dummy_nrppa_cu_cp_ue_notifier ue_notifier;
  nrppa_ue_context_list_test    ue_ctxt_list{nrppa_logger};
};

TEST_F(nrppa_ue_context_test, when_ue_added_then_ue_exists)
{
  cu_cp_ue_index_t ue_index       = generate_random_ue_index();
  ran_ue_meas_id_t ran_ue_meas_id = generate_random_ran_ue_meas_id();
  lmf_ue_meas_id_t lmf_ue_meas_id = generate_random_lmf_ue_meas_id();

  ue_ctxt_list.add_ue(ue_index, ran_ue_meas_id, lmf_ue_meas_id, ue_notifier, timer_mng, ctrl_worker);

  ASSERT_TRUE(ue_ctxt_list.contains(ue_index));

  ASSERT_EQ(ue_ctxt_list[ue_index].ue_ids.ran_ue_meas_id, ran_ue_meas_id);
  ASSERT_EQ(ue_ctxt_list[ue_index].ue_ids.ue_index, ue_index);
}

TEST_F(nrppa_ue_context_test, when_ue_not_added_then_ue_doesnt_exist)
{
  cu_cp_ue_index_t ue_index = generate_random_ue_index();

  ASSERT_FALSE(ue_ctxt_list.contains(ue_index));
}

TEST_F(nrppa_ue_context_test, when_ue_exists_then_removal_succeeds)
{
  cu_cp_ue_index_t ue_index       = generate_random_ue_index();
  ran_ue_meas_id_t ran_ue_meas_id = generate_random_ran_ue_meas_id();
  lmf_ue_meas_id_t lmf_ue_meas_id = generate_random_lmf_ue_meas_id();

  ue_ctxt_list.add_ue(ue_index, ran_ue_meas_id, lmf_ue_meas_id, ue_notifier, timer_mng, ctrl_worker);

  // Test removal.
  ue_ctxt_list.remove_ue_context(ue_index);

  ASSERT_FALSE(ue_ctxt_list.contains(ue_index));
}

TEST_F(nrppa_ue_context_test, when_ue_is_added_then_next_ue_id_is_increased)
{
  cu_cp_ue_index_t ue_index       = generate_random_ue_index();
  ran_ue_meas_id_t ran_ue_meas_id = ue_ctxt_list.allocate_ran_ue_meas_id().value();
  lmf_ue_meas_id_t lmf_ue_meas_id = generate_random_lmf_ue_meas_id();

  ASSERT_EQ((unsigned)ran_ue_meas_id, (unsigned)ran_ue_meas_id_t::min);

  ue_ctxt_list.add_ue(ue_index, ran_ue_meas_id, lmf_ue_meas_id, ue_notifier, timer_mng, ctrl_worker);

  // Remove UE.
  ue_ctxt_list.remove_ue_context(ue_index);

  ASSERT_FALSE(ue_ctxt_list.contains(ue_index));

  ASSERT_EQ((unsigned)ue_ctxt_list.allocate_ran_ue_meas_id().value(), (unsigned)ran_ue_meas_id_t::min + 1);
}

TEST_F(nrppa_ue_context_test, when_ue_index_is_updated_then_context_is_moved_to_new_index)
{
  cu_cp_ue_index_t old_ue_index   = generate_random_ue_index();
  cu_cp_ue_index_t new_ue_index   = generate_random_ue_index();
  ran_ue_meas_id_t ran_ue_meas_id = generate_random_ran_ue_meas_id();
  lmf_ue_meas_id_t lmf_ue_meas_id = generate_random_lmf_ue_meas_id();
  while (new_ue_index == old_ue_index) {
    new_ue_index = generate_random_ue_index();
  }

  ue_ctxt_list.add_ue(old_ue_index, ran_ue_meas_id, lmf_ue_meas_id, ue_notifier, timer_mng, ctrl_worker);

  dummy_nrppa_cu_cp_ue_notifier new_ue_notifier;
  ue_ctxt_list.update_ue_index(new_ue_index, old_ue_index, new_ue_notifier, timer_mng, ctrl_worker);

  ASSERT_FALSE(ue_ctxt_list.contains(old_ue_index));
  ASSERT_TRUE(ue_ctxt_list.contains(new_ue_index));
  ASSERT_EQ(ue_ctxt_list[new_ue_index].ue_ids.ran_ue_meas_id, ran_ue_meas_id);
  ASSERT_EQ(ue_ctxt_list[new_ue_index].ue_ids.lmf_ue_meas_id, lmf_ue_meas_id);
  ASSERT_EQ(ue_ctxt_list[new_ue_index].get_cu_cp_ue(), &new_ue_notifier);
}

TEST_F(nrppa_ue_context_test, when_all_ran_ue_meas_ids_are_in_use_then_allocation_fails)
{
  for (auto i = (unsigned)ran_ue_meas_id_t::min; i <= (unsigned)ran_ue_meas_id_t::max; ++i) {
    ran_ue_meas_id_t ran_ue_meas_id = ue_ctxt_list.allocate_ran_ue_meas_id().value();
    ue_ctxt_list.add_ue(
        uint_to_ue_index(i), ran_ue_meas_id, generate_random_lmf_ue_meas_id(), ue_notifier, timer_mng, ctrl_worker);
  }

  ASSERT_FALSE(ue_ctxt_list.allocate_ran_ue_meas_id().has_value());
}

TEST_F(nrppa_ue_context_test, when_ran_ue_meas_id_counter_wraps_then_allocation_reuses_freed_id)
{
  std::vector<cu_cp_ue_index_t> ue_indexes;
  for (auto i = (unsigned)ran_ue_meas_id_t::min; i <= (unsigned)ran_ue_meas_id_t::max; ++i) {
    cu_cp_ue_index_t ue_index       = uint_to_ue_index(i);
    ran_ue_meas_id_t ran_ue_meas_id = ue_ctxt_list.allocate_ran_ue_meas_id().value();
    ue_ctxt_list.add_ue(
        ue_index, ran_ue_meas_id, generate_random_lmf_ue_meas_id(), ue_notifier, timer_mng, ctrl_worker);
    ue_indexes.push_back(ue_index);
  }

  // Free the RAN UE MEAS ID in the middle of the range; the counter has already wrapped back to min.
  ran_ue_meas_id_t freed_id = ue_ctxt_list[ue_indexes[4]].ue_ids.ran_ue_meas_id;
  ue_ctxt_list.remove_ue_context(ue_indexes[4]);

  ASSERT_EQ(ue_ctxt_list.allocate_ran_ue_meas_id().value(), freed_id);
}
