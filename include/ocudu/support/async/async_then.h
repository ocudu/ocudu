// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/support/async/coroutine.h"

namespace ocudu {

template <typename Task>
[[nodiscard]] Task async_then(Task&& first, Task&& second)
{
  return launch_async(
      [first = std::forward<Task>(first), second = std::forward<Task>(second)](coro_context<Task>& ctx) mutable {
        CORO_BEGIN(ctx);
        CORO_AWAIT(first);
        CORO_AWAIT(second);
        CORO_RETURN();
      });
}

} // namespace ocudu
