// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/support/rate_limiting/lockfree_token_bucket.h"
#include "ocudu/support/compiler.h"
#include "ocudu/support/ocudu_assert.h"
#include <algorithm>
#include <cstdint>

using namespace ocudu;

lockfree_token_bucket::lockfree_token_bucket(uint64_t rate_, uint64_t capacity_, timer_manager& timer_mng_) :
  time_per_token(std::chrono::nanoseconds(std::chrono::seconds(1)) / rate_),
  time_to_fill_from_empty(time_per_token * capacity_),
  timer_mng(timer_mng_)
{
}

bool lockfree_token_bucket::consume(uint32_t tokens)
{
  auto                           now         = std::chrono::nanoseconds(timer_mng.now() * 1000000);
  const std::chrono::nanoseconds time_needed = time_per_token * tokens;
  const std::chrono::nanoseconds min_time =
      std::chrono::nanoseconds(now) - time_to_fill_from_empty; // clamp to capacity.

  signed old_time = empty_time.load(std::memory_order_relaxed);

  for (;;) {
    signed new_time = old_time;
    new_time        = std::max((signed)min_time.count(), new_time);
    new_time += time_needed.count();
    if (new_time > now.count()) {
      return false;
    }
    if (empty_time.compare_exchange_weak(old_time, new_time, std::memory_order_relaxed, std::memory_order_relaxed)) {
      return true;
    }
  }
  return false;
}
