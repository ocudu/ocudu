// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

namespace ocudu {

struct executor_metrics;

/// Notifier interface used by the executor metrics backend to report metrics.
class executor_metrics_notifier
{
public:
  virtual ~executor_metrics_notifier() = default;

  /// Notifies new executor metrics.
  virtual void on_new_metrics(const executor_metrics& metrics) = 0;
};

} // namespace ocudu
