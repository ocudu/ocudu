// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/mac/mac_dl/cell_dl_harq_buffer_pool.h"
#include <gtest/gtest.h>

using namespace ocudu;

namespace {

/// Number of PRBs used across the tests.
constexpr unsigned test_nof_prbs = 4;

// Expected maximum MAC PDU length in bytes, per number of layers:
//   MAX_NRE_PER_RB (156) * test_nof_prbs (4) * nof_layers * MAX_MODULATION_ORDER (8) / 8 = 624 * nof_layers.
constexpr size_t max_pdu_len_1_layer  = 624;
constexpr size_t max_pdu_len_2_layers = 1248;
constexpr size_t max_pdu_len_4_layers = 2496;

/// Constructs a pool for the given max number of layers and returns the size (in bytes) of a preallocated HARQ buffer.
size_t harq_buffer_size(unsigned max_nof_layers)
{
  cell_dl_harq_buffer_pool pool(test_nof_prbs, max_nof_layers, MAX_NOF_HARQS, 1);
  const du_ue_index_t      ue_index = to_du_ue_index(0);
  pool.allocate_ue_buffers(ue_index, 1);
  expected<dl_harq_buffer_handle> buffer = pool.allocate_dl_harq_buffer(ue_index, to_harq_id(0));
  EXPECT_TRUE(buffer.has_value());
  return buffer.value().get_buffer().size();
}

} // namespace

TEST(cell_dl_harq_buffer_pool_test, allocation_fails_when_the_cell_runs_out_of_ue_buffer_lists)
{
  constexpr unsigned       nof_ue_contexts = 1;
  cell_dl_harq_buffer_pool pool(test_nof_prbs, 1, MAX_NOF_HARQS, nof_ue_contexts);

  ASSERT_TRUE(pool.allocate_ue_buffers(to_du_ue_index(0), 1));
  ASSERT_FALSE(pool.allocate_ue_buffers(to_du_ue_index(1), 1));

  // The list of the removed UE becomes available for the refused one.
  pool.deallocate_ue_buffers(to_du_ue_index(0));
  ASSERT_TRUE(pool.allocate_ue_buffers(to_du_ue_index(1), 1));
}

TEST(cell_dl_harq_buffer_pool_test, buffers_are_returned_to_the_cell_when_an_allocation_cannot_be_completed)
{
  // The cell holds fewer HARQ buffers than the UE requests, so the allocation cannot be completed.
  constexpr unsigned       nof_cell_harqs = 4;
  cell_dl_harq_buffer_pool pool(test_nof_prbs, 1, nof_cell_harqs, 2);

  ASSERT_FALSE(pool.allocate_ue_buffers(to_du_ue_index(0), nof_cell_harqs + 1));

  // The buffers taken by the failed allocation are available again.
  ASSERT_TRUE(pool.allocate_ue_buffers(to_du_ue_index(1), nof_cell_harqs));
  for (unsigned i = 0; i != nof_cell_harqs; ++i) {
    ASSERT_TRUE(pool.allocate_dl_harq_buffer(to_du_ue_index(1), to_harq_id(i)).has_value());
  }
}

TEST(cell_dl_harq_buffer_pool_test, buffers_can_be_allocated_for_any_valid_du_ue_index)
{
  // The pool is dimensioned for the UE contexts of the cell, but it is addressed by DU UE index, whose range is
  // DU-wide.
  constexpr unsigned       nof_ue_contexts = 2;
  cell_dl_harq_buffer_pool pool(test_nof_prbs, 1, MAX_NOF_HARQS, nof_ue_contexts);

  const du_ue_index_t ue_index = to_du_ue_index(MAX_NOF_DU_UES - 1);
  pool.allocate_ue_buffers(ue_index, 1);

  expected<dl_harq_buffer_handle> buffer = pool.allocate_dl_harq_buffer(ue_index, to_harq_id(0));
  ASSERT_TRUE(buffer.has_value());
  ASSERT_EQ(buffer.value().get_buffer().size(), max_pdu_len_1_layer);
}

TEST(cell_dl_harq_buffer_pool_test, ue_buffers_are_reused_after_deallocation)
{
  constexpr unsigned       nof_ue_contexts = 1;
  cell_dl_harq_buffer_pool pool(test_nof_prbs, 1, MAX_NOF_HARQS, nof_ue_contexts);

  const du_ue_index_t first_ue = to_du_ue_index(3000);
  pool.allocate_ue_buffers(first_ue, 1);
  ASSERT_TRUE(pool.allocate_dl_harq_buffer(first_ue, to_harq_id(0)).has_value());
  pool.deallocate_ue_buffers(first_ue);

  // The list of the removed UE is available for another UE.
  const du_ue_index_t second_ue = to_du_ue_index(4000);
  pool.allocate_ue_buffers(second_ue, 1);
  ASSERT_TRUE(pool.allocate_dl_harq_buffer(second_ue, to_harq_id(0)).has_value());
}

TEST(cell_dl_harq_buffer_pool_test, buffer_size_is_bounded_by_layers_not_ports)
{
  // The buffer is sized by the configured maximum number of DL layers (already bounded by the number of antenna ports
  // and the single-codeword layer limit when the cell config is resolved), not by the number of antenna ports.
  EXPECT_EQ(harq_buffer_size(1), max_pdu_len_1_layer);
  EXPECT_EQ(harq_buffer_size(2), max_pdu_len_2_layers);
  EXPECT_EQ(harq_buffer_size(4), max_pdu_len_4_layers);
}
