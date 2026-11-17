// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "task_executor_test_doubles.h"
#include "ocudu/adt/byte_buffer_recycler.h"
#include "ocudu/adt/detail/byte_buffer_segment_pool.h"
#include <gtest/gtest.h>

using namespace ocudu;

namespace {

void assert_all_segments_freed()
{
  auto&    pool    = detail::get_default_byte_buffer_segment_pool();
  unsigned in_pool = pool.get_local_cache_size() + pool.get_central_cache_approx_size();
  ASSERT_EQ(in_pool, pool.nof_memory_blocks()) << "Leaked byte_buffer segments";
}

} // namespace

class byte_buffer_recycler_test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    ocudulog::init();
    logger.set_level(ocudulog::basic_levels::debug);
  }

  void TearDown() override
  {
    assert_all_segments_freed();
    ocudulog::flush();
  }

  static byte_buffer make_buf() { return byte_buffer::create({0xab, 0xcd, 0xef}).value(); }

  ocudulog::basic_logger&                 logger = ocudulog::fetch_basic_logger("TEST", false);
  manual_task_worker_always_enqueue_tasks worker{16};
};

// flush() enqueues exactly one task on the background executor; recycle() alone enqueues nothing.
TEST_F(byte_buffer_recycler_test, flush_dispatches_task_to_executor)
{
  byte_buffer_recycler recycler{worker, 4, logger};

  recycler.recycle(make_buf());
  EXPECT_FALSE(worker.has_pending_tasks());

  recycler.flush();
  EXPECT_TRUE(worker.has_pending_tasks());

  worker.run_pending_tasks();
  EXPECT_FALSE(worker.has_pending_tasks());
}

// Buffers are freed by the executor task, not by the recycler itself or its destructor.
TEST_F(byte_buffer_recycler_test, executor_frees_buffers)
{
  byte_buffer_recycler recycler{worker, 4, logger};

  recycler.recycle(make_buf());
  recycler.recycle(make_buf());
  recycler.flush();

  ASSERT_TRUE(worker.has_pending_tasks());
  worker.run_pending_tasks();

  // All segments returned to pool while the recycler is still alive.
  assert_all_segments_freed();
}

// Buffers recycled after a flush() but before the executor runs are picked up by a subsequent flush().
TEST_F(byte_buffer_recycler_test, recycle_between_flushes)
{
  byte_buffer_recycler recycler{worker, 4, logger};

  recycler.recycle(make_buf());
  recycler.flush();
  // Enqueue another buffer while the first drain task is still pending.
  recycler.recycle(make_buf());
  recycler.flush();

  worker.run_pending_tasks();
  assert_all_segments_freed();
}

// Multiple independent recycle–flush–run cycles each leave no segments allocated.
TEST_F(byte_buffer_recycler_test, multiple_recycle_flush_cycles)
{
  byte_buffer_recycler recycler{worker, 4, logger};

  for (int i = 0; i < 3; ++i) {
    recycler.recycle(make_buf());
    recycler.flush();
    worker.run_pending_tasks();
    assert_all_segments_freed();
  }
}

// When the queue is full the overflowing buffer is destroyed immediately on the calling thread.
TEST_F(byte_buffer_recycler_test, queue_full_destroys_buffer_immediately)
{
  constexpr size_t     capacity = 2;
  byte_buffer_recycler recycler{worker, capacity, logger};

  recycler.recycle(make_buf());
  recycler.recycle(make_buf()); // fills queue
  recycler.recycle(make_buf()); // overflow — destroyed immediately, not queued

  recycler.flush();
  worker.run_pending_tasks();
  // All three freed: one on the spot, two by the executor.
  assert_all_segments_freed();
}

// The destructor drains remaining buffers synchronously without requiring a flush().
TEST_F(byte_buffer_recycler_test, destructor_frees_pending_buffers)
{
  {
    byte_buffer_recycler recycler{worker, 4, logger};
    recycler.recycle(make_buf());
    recycler.recycle(make_buf());
    // Intentionally no flush() — destructor must drain synchronously.
  }
  assert_all_segments_freed();
}

// flush() on a stopped executor logs an error but does not crash; the destructor still frees any
// remaining buffers.
TEST_F(byte_buffer_recycler_test, flush_with_stopped_executor_does_not_crash)
{
  byte_buffer_recycler recycler{worker, 4, logger};
  recycler.recycle(make_buf());
  worker.stop();
  recycler.flush();
  // Destructor drains items the rejected task left behind.
}
