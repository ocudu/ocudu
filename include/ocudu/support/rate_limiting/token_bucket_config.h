// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/support/timers.h"
#include <chrono>

namespace ocudu {

struct token_bucket_config {
  uint32_t                  refill_token;
  std::chrono::milliseconds refill_period;
  uint32_t                  max_tokens;
  timer_factory             timer_f;
};

inline token_bucket_config generate_token_bucket_config(uint64_t                  avg_rate_bps,
                                                        uint64_t                  max_rate_bps,
                                                        std::chrono::milliseconds refill_period,
                                                        timer_factory             timer_f)
{
  token_bucket_config cfg = {};
  cfg.refill_period       = refill_period;
  cfg.refill_token        = avg_rate_bps * refill_period.count() / 8 * 1e-3;
  cfg.max_tokens          = max_rate_bps * refill_period.count() / 8 * 1e-3;
  cfg.timer_f             = timer_f;
  return cfg;
}

} // namespace ocudu
