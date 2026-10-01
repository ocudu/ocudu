// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/support/executors/task_executor.h"

namespace ocudu {

class inline_task_executor final : public task_executor
{
public:
  [[nodiscard]] bool execute(unique_task task) override
  {
    task();
    return true;
  }

  [[nodiscard]] bool defer(unique_task task) override
  {
    task();
    return true;
  }
};

} // namespace ocudu
