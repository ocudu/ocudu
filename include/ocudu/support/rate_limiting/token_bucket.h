// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/support/rate_limiting/token_bucket_config.h"
#include "ocudu/support/timers.h"
#include <cstdint>

namespace ocudu {

/// \brief Rate limiter class that implements a rate limiter
/// based on the token bucket algorithm.
///
class token_bucket
{
public:
  token_bucket(token_bucket_config cfg);

  /// Consume tokens from the bucket.
  /// \return False if there were not enough available tokens, true otherwise.
  bool consume(uint32_t tokens);

  void stop();

private:
  /// Refill tokens to the bucket.
  /// It will be called internally by the refill timer.
  void refill(uint32_t tokens);

  uint32_t max_tokens;
  uint32_t tokens_in_bucket;

  unique_timer refill_timer;

  bool stopped = false;
};
} // namespace ocudu
