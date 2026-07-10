// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/nrppa/meas_context/nrppa_meas_context.h"
#include "tests/test_doubles/utils/test_rng.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocucp;

// Test class. Exposes next_ran_meas_id so wraparound scenarios can be set up without looping over the full ID range.
class nrppa_meas_context_list_test : public nrppa_meas_context_list
{
public:
  nrppa_meas_context_list_test(ocudulog::basic_logger& logger_) : nrppa_meas_context_list(logger_) {}

  void set_next_ran_meas_id(ran_meas_id_t id) { next_ran_meas_id = id; }
};

/// Fixture class for NRPPA measurement context.
class nrppa_meas_context_test : public ::testing::Test
{
protected:
  nrppa_meas_context_test()
  {
    nrppa_logger.set_level(ocudulog::basic_levels::debug);
    ocudulog::init();
  }

  ~nrppa_meas_context_test()
  {
    // Flush logger after each test.
    ocudulog::flush();
  }

  static lmf_meas_id_t generate_random_lmf_meas_id()
  {
    return uint_to_lmf_meas_id(test_rng::uniform_int<uint64_t>(lmf_meas_id_to_uint(lmf_meas_id_t::min),
                                                               lmf_meas_id_to_uint(lmf_meas_id_t::max) - 1));
  }

  ocudulog::basic_logger&      nrppa_logger = ocudulog::fetch_basic_logger("nrppa");
  cu_cp_amf_index_t            amf_index    = cu_cp_amf_index_t::min;
  std::vector<trp_id_t>        trp_list     = {trp_id_t::min};
  nrppa_meas_context_list_test meas_ctxt_list{nrppa_logger};
};

TEST_F(nrppa_meas_context_test, when_measurement_added_then_it_exists)
{
  lmf_meas_id_t lmf_meas_id = generate_random_lmf_meas_id();
  ran_meas_id_t ran_meas_id = meas_ctxt_list.allocate_ran_meas_id().value();

  meas_ctxt_list.add_measurement(amf_index, ran_meas_id, lmf_meas_id, trp_list);

  ASSERT_TRUE(meas_ctxt_list.contains(lmf_meas_id));
  ASSERT_EQ(meas_ctxt_list[lmf_meas_id].ran_meas_id, ran_meas_id);
  ASSERT_EQ(meas_ctxt_list.size(), 1);
}

TEST_F(nrppa_meas_context_test, when_measurement_not_added_then_it_doesnt_exist)
{
  lmf_meas_id_t lmf_meas_id = generate_random_lmf_meas_id();

  ASSERT_FALSE(meas_ctxt_list.contains(lmf_meas_id));
}

TEST_F(nrppa_meas_context_test, when_measurement_exists_then_removal_succeeds)
{
  lmf_meas_id_t lmf_meas_id = generate_random_lmf_meas_id();
  ran_meas_id_t ran_meas_id = meas_ctxt_list.allocate_ran_meas_id().value();

  meas_ctxt_list.add_measurement(amf_index, ran_meas_id, lmf_meas_id, trp_list);

  meas_ctxt_list.remove_measurement_context(lmf_meas_id);

  ASSERT_FALSE(meas_ctxt_list.contains(lmf_meas_id));
  ASSERT_EQ(meas_ctxt_list.size(), 0);
}

TEST_F(nrppa_meas_context_test, when_unknown_measurement_is_removed_then_it_is_ignored)
{
  lmf_meas_id_t lmf_meas_id = generate_random_lmf_meas_id();

  meas_ctxt_list.remove_measurement_context(lmf_meas_id);

  ASSERT_EQ(meas_ctxt_list.size(), 0);
}

TEST_F(nrppa_meas_context_test, when_measurement_is_added_then_next_ran_meas_id_is_increased)
{
  ran_meas_id_t first_id = meas_ctxt_list.allocate_ran_meas_id().value();
  ASSERT_EQ((unsigned)first_id, (unsigned)ran_meas_id_t::min);

  meas_ctxt_list.add_measurement(amf_index, first_id, generate_random_lmf_meas_id(), trp_list);

  ASSERT_EQ((unsigned)meas_ctxt_list.allocate_ran_meas_id().value(), (unsigned)ran_meas_id_t::min + 1);
}

TEST_F(nrppa_meas_context_test, when_ran_meas_id_counter_wraps_then_allocation_skips_ids_still_in_use)
{
  // Simulate the counter having already cycled through the whole range back to min, with min still in use.
  meas_ctxt_list.add_measurement(amf_index, ran_meas_id_t::min, generate_random_lmf_meas_id(), trp_list);
  meas_ctxt_list.set_next_ran_meas_id(ran_meas_id_t::min);

  ASSERT_EQ((unsigned)meas_ctxt_list.allocate_ran_meas_id().value(), (unsigned)ran_meas_id_t::min + 1);
}

TEST_F(nrppa_meas_context_test, when_all_ran_meas_ids_are_in_use_then_allocation_fails)
{
  for (auto i = (unsigned)ran_meas_id_t::min; i <= (unsigned)ran_meas_id_t::max; ++i) {
    meas_ctxt_list.add_measurement(amf_index, uint_to_ran_meas_id(i), uint_to_lmf_meas_id(i), trp_list);
  }

  ASSERT_FALSE(meas_ctxt_list.allocate_ran_meas_id().has_value());
}
