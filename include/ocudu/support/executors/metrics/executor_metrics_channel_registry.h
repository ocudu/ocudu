// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include <string>

namespace ocudu {

class executor_metrics_channel;

/// Interface for adding a new executor metrics channel.
class executor_metrics_channel_registry
{
public:
  virtual ~executor_metrics_channel_registry() = default;

  /// Adds new metrics channel that a calling executor can use to send its metrics.
  virtual executor_metrics_channel& add_channel(const std::string&) = 0;
};

} // namespace ocudu
