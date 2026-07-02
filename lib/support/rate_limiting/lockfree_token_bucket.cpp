// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/support/rate_limiting/lockfree_token_bucket.h"
#include "ocudu/support/compiler.h"
#include "ocudu/support/ocudu_assert.h"
#include <algorithm>
#include <cstdint>

using namespace ocudu;

lockfree_token_bucket::lockfree_token_bucket(uint32_t rate_, uint32_t capacity_) :
  time_per_token(1 / rate_), time_to_fill_from_empty(time_per_token * capacity_)
{
}

bool lockfree_token_bucket::consume(uint32_t tokens)
{
  tick_point_t now         = timer_mng.now();
  unsigned     time_needed = time_per_token * tokens;
  unsigned     min_time    = now - time_to_fill_from_empty; // clamp to capacity.

  tick_point_t old_time = empty_time.load(std::memory_order_relaxed);

  for (;;) {
    tick_point_t new_time = old_time;
    new_time              = std::max(min_time, new_time);
    new_time += time_needed;
    if (new_time > now) {
      return false;
    }
    if (empty_time.compare_exchange_weak(old_time, new_time, std::memory_order_relaxed, std::memory_order_relaxed)) {
      return true;
    }
  }
  return false;
}
