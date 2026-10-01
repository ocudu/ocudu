// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/adt/unique_function.h"

namespace ocudu {

class task_executor
{
public:
  virtual ~task_executor() = default;

  /// Dispatches a task to be executed. According to this policy, executor may invoke task inplace if it deemed safe
  /// to do so.
  [[nodiscard]] virtual bool execute(unique_task task) = 0;

  /// Dispatches a task to be later executed. The task always gets enqueued for later invocation.
  [[nodiscard]] virtual bool defer(unique_task task) = 0;
};

} // namespace ocudu
