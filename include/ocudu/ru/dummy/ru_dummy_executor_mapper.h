// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/support/executors/task_executor.h"
#include <memory>
#include <vector>

namespace ocudu {

class task_executor;

/// Dummy RU executor mapper interface.
class ru_dummy_executor_mapper
{
public:
  /// Default destructor.
  virtual ~ru_dummy_executor_mapper() = default;

  /// Retrieves the dummy RU executor reference.
  virtual task_executor& common_executor() = 0;
};

/// Creates an RU dummyu implementation executor mapper from a single task executor.
std::unique_ptr<ru_dummy_executor_mapper> create_ru_dummy_executor_mapper(task_executor& executor);

} // namespace ocudu
