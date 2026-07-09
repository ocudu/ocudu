// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

/// This file implements a lockfree token buffer. Based on https://github.com/rigtorp/TokenBucket.
/// The algorithm was adapt to use the time from the timer manager, so that it can be driven
/// by system time or the radio time.

#pragma once

#include "ocudu/ocudulog/logger.h"
#include "ocudu/support/timers.h"
#include <cstdint>

namespace ocudu {

/// \brief Rate limiter class that implements a rate limiter
/// based on the token bucket algorithm.
///
class lockfree_token_bucket
{
public:
  lockfree_token_bucket(uint64_t rate, uint64_t capacity, timer_manager& timer_mng_);

  /// Consume tokens from the bucket.
  /// \return False if there were not enough available tokens, true otherwise.
  bool consume(uint32_t tokens);

  void stop();

private:
  /// Type to represent the virtual empty time. It must have sufficient resolution
  /// so that consuming a PDU worth of tokens does not get quantized. In this implementation
  /// it is represented in nanoseconds.
  using tick_t = uint64_t;

  tick_t get_now()
  {
    // Return in nanoseconds the current tick time.
    return static_cast<tick_t>(timer_mng.now()) * 1000000ULL;
  }

  /// Virtual time of emptiness. Represents how long ago in the past
  /// the bucket would have been empty so that we have the current amount of tokens
  /// given the rate.
  std::atomic<tick_t>      empty_time              = {};
  std::chrono::nanoseconds time_per_token          = {};
  std::chrono::nanoseconds time_to_fill_from_empty = {};

  timer_manager& timer_mng;

  ocudulog::basic_logger& logger;
};
} // namespace ocudu
