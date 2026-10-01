// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/support/async/async_task.h"
#include "ocudu/support/async/eager_async_task.h"
#include "ocudu/support/synchronization/sync_event.h"
#include <optional>

namespace ocudu {
namespace unittest {

template <typename R>
struct waitable_task_launcher : public eager_async_task<R> {
  explicit waitable_task_launcher(async_task<R>& t_) : t(t_)
  {
    *static_cast<eager_async_task<R>*>(this) =
        launch_async([this, token = event.get_token()](coro_context<eager_async_task<R>>& ctx) {
          CORO_BEGIN(ctx);
          CORO_AWAIT_VALUE(result, t);
          CORO_RETURN(result.value());
        });
  }

  void wait_for_coroutine_result() { event.wait(); }

  bool finished() const { return result.has_value(); }

  std::optional<R> result;

private:
  async_task<R>& t;
  sync_event     event;
};

} // namespace unittest
} // namespace ocudu
