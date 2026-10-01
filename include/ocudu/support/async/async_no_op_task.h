// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/support/async/async_task.h"

namespace ocudu {

/// Launches an async task that finishes immediately with the result set by the caller to this function.
template <typename Resp>
[[nodiscard]] async_task<std::decay_t<Resp>> launch_no_op_task(Resp&& r)
{
  return launch_async([r = std::forward<Resp>(r)](coro_context<async_task<std::decay_t<Resp>>>& ctx) {
    CORO_BEGIN(ctx);
    CORO_RETURN(r);
  });
}

/// Launches an async task that finishes immediately with the result set by the caller to this function.
[[nodiscard]] inline async_task<void> launch_no_op_task()
{
  return launch_async([](coro_context<async_task<void>>& ctx) {
    CORO_BEGIN(ctx);
    CORO_RETURN();
  });
}

} // namespace ocudu
