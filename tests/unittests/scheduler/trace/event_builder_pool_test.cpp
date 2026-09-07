// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "fbs/cell_event_generated.h"
#include "lib/scheduler/trace/event_builder_pool.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocudu::schedtrace;
namespace fbs = ocudu::schedtrace::fbs;

/// Builds a busy slot event: several PUSCHs/PUCCHs with allocations, to exercise vectors and unions.
static void build_busy_slot_event(flatbuffers::FlatBufferBuilder& fbb)
{
  std::array<flatbuffers::Offset<fbs::Pusch>, 16> puschs;
  for (unsigned i = 0; i != puschs.size(); ++i) {
    auto vrb  = fbs::CreateVrbAlloc(fbb, i, 20);
    puschs[i] = fbs::CreatePusch(fbb, 0x4601 + i, fbs::RbAlloc::VrbAlloc, vrb.Union(), 0, 14, 0, 1, i % 8, 1024, i);
  }
  std::array<flatbuffers::Offset<fbs::Pucch>, 64> pucchs;
  for (unsigned i = 0; i != pucchs.size(); ++i) {
    pucchs[i] = fbs::CreatePucch(fbb, 0x4601 + i, i, fbs::PucchTxParams::NONE, 0, 2, 1, 0);
  }

  auto pusch_vec = fbb.CreateVector(puschs.data(), puschs.size());
  auto pucch_vec = fbb.CreateVector(pucchs.data(), pucchs.size());

  fbs::SlotDecisionBuilder dec{fbb};
  dec.add_puschs(pusch_vec);
  dec.add_pucchs(pucch_vec);
  auto decision = dec.Finish();

  auto slot_ev = fbs::CreateCellSlotEvent(fbb, 1234, 200, 0, 0, decision);
  auto cell_ev = fbs::CreateCellEvent(fbb, 0, fbs::CellEventValue::CellSlotEvent, slot_ev.Union());
  fbs::FinishSizePrefixedCellEventBuffer(fbb, cell_ev);
}

TEST(event_builder_pool_test, steady_state_reuse_is_malloc_free)
{
  event_builder_pool pool(4);

  for (unsigned i = 0; i != 100; ++i) {
    event_builder* b = pool.allocate();
    ASSERT_NE(b, nullptr);
    build_busy_slot_event(b->fbb());
    ASSERT_FALSE(b->finished().empty());
    pool.deallocate(*b);
  }

  // Each builder performed exactly one allocation: the eager one at pool construction.
  event_builder* b = pool.allocate();
  EXPECT_EQ(b->nof_allocations(), 1);
  EXPECT_EQ(b->buffer_size(), event_builder_pool::default_buffer_size);
}

TEST(event_builder_pool_test, oversized_event_grows_buffer_once_then_reuses_it)
{
  // Deliberately undersized buffers.
  event_builder_pool pool(1, 128);

  event_builder* b = pool.allocate();
  ASSERT_NE(b, nullptr);
  ASSERT_EQ(b->nof_allocations(), 1);

  build_busy_slot_event(b->fbb());
  const size_t grown_allocs = b->nof_allocations();
  const size_t grown_size   = b->buffer_size();
  EXPECT_GT(grown_allocs, 1);
  EXPECT_GT(grown_size, 128);

  // The grown buffer is retained: rebuilding the same event allocates no more.
  pool.deallocate(*b);
  for (unsigned i = 0; i != 10; ++i) {
    b = pool.allocate();
    ASSERT_NE(b, nullptr);
    build_busy_slot_event(b->fbb());
    pool.deallocate(*b);
  }
  b = pool.allocate();
  EXPECT_EQ(b->nof_allocations(), grown_allocs);
  EXPECT_EQ(b->buffer_size(), grown_size);
}

TEST(event_builder_pool_test, allocate_fails_when_pool_is_exhausted)
{
  event_builder_pool pool(2);

  // qsize + 2 builders are available.
  ASSERT_EQ(pool.capacity(), 4);
  for (unsigned i = 0; i != pool.capacity(); ++i) {
    EXPECT_NE(pool.allocate(), nullptr);
  }
  EXPECT_EQ(pool.allocate(), nullptr);
}
