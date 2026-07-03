// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

///
/// This file
///

#pragma once

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
  /// Virtual time of emptiness. Represents how long ago in the past
  /// the bucket would have been empty so that we have the current amount of tokens
  /// given the rate.
  std::atomic<signed>      empty_time              = {};
  std::chrono::nanoseconds time_per_token          = {};
  std::chrono::nanoseconds time_to_fill_from_empty = {};

  timer_manager& timer_mng;
};
} // namespace ocudu
