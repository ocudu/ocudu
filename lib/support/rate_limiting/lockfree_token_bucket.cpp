// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/support/rate_limiting/lockfree_token_bucket.h"
#include "ocudu/ocudulog/ocudulog.h"
#include <cstdint>

using namespace ocudu;

lockfree_token_bucket::lockfree_token_bucket(uint64_t rate_, uint64_t capacity_, timer_manager& timer_mng_) :
  time_per_token(std::chrono::nanoseconds(std::chrono::seconds(1)) / rate_),
  time_to_fill_from_empty(time_per_token * capacity_),
  timer_mng(timer_mng_),
  logger(ocudulog::fetch_basic_logger("ALL"))
{
  if (not empty_time.is_lock_free()) {
    logger.warning("Rate limiter is not lock free");
  }
}

bool lockfree_token_bucket::consume(uint32_t tokens)
{
  tick_t                         now         = get_now();
  const std::chrono::nanoseconds time_needed = time_per_token * tokens;
  tick_t                         min_time    = now - time_to_fill_from_empty.count(); // clamp to capacity.

  tick_t old_time = empty_time.load(std::memory_order_relaxed);

  for (;;) {
    tick_t new_time = old_time;
    if (min_time > new_time) {
      new_time = min_time;
    }
    new_time += time_needed.count();
    if (new_time > now) {
      return false;
    }
    if (empty_time.compare_exchange_weak(old_time, new_time, std::memory_order_relaxed, std::memory_order_relaxed)) {
      return true;
    }
  }
  return false;
}
