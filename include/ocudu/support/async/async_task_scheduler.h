// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/support/async/async_task.h"

namespace ocudu {

/// Generic interface for scheduling async tasks.
class async_task_scheduler
{
public:
  virtual ~async_task_scheduler() = default;

  /// Schedule async task.
  virtual bool schedule(async_task<void> task) = 0;
};

} // namespace ocudu
