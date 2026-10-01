// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include <vector>

namespace ocudulog {

class sink;

/// Contains the registered sinks used by the loggers.
class sink_repository
{
public:
  virtual ~sink_repository() = default;

  /// Returns the contents of the sink repository.
  virtual std::vector<sink*> contents() = 0;
};

} // namespace ocudulog
