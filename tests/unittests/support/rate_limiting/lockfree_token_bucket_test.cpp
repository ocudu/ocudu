// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/support/executors/manual_task_worker.h"
#include "ocudu/support/rate_limiting/lockfree_token_bucket.h"
#include <gtest/gtest.h>

using namespace ocudu;

static void tick_all(timer_manager& timers, uint32_t nof_ticks)
{
  for (uint32_t i = 0; i < nof_ticks; i++) {
    timers.tick();
  }
}

/// \brief Test token bucket consume and re-fill.
TEST(lockfree_token_bucket_test, consume_and_refill_test)
{
  timer_manager timers;

  /// Configure lockfree token bucket with 20M tokens per second,
  /// and 24000 tokens of capacity.
  uint32_t rate     = 20000000;
  uint32_t capacity = 24000;

  lockfree_token_bucket bucket{rate, capacity, timers};

  // Consume all tokens. Next token consumption will fail.
  ASSERT_TRUE(bucket.consume(24000));
  ASSERT_FALSE(bucket.consume(1));

  // Tick once. This will refresh 20k tokens.
  tick_all(timers, 1);
  ASSERT_TRUE(bucket.consume(20000));
  ASSERT_FALSE(bucket.consume(1));

  // Fully refresh the bucket.
  // This will be limited by the capacity, so 24k tokens.
  tick_all(timers, 2);
  ASSERT_TRUE(bucket.consume(10000));
  ASSERT_TRUE(bucket.consume(10000));
  ASSERT_TRUE(bucket.consume(4000));
  ASSERT_FALSE(bucket.consume(1));
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
