// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "metrics/split6_flexible_o_du_low_metrics_notifier.h"
#include "ocudu/ru/ru.h"
#include "ocudu/support/timers.h"

namespace ocudu {
class task_executor;

namespace odu {
class o_du_low_metrics_collector;
}

/// Split 6 O-DU low metrics collector implementation.
class split6_o_du_low_metrics_collector_impl
{
public:
  split6_o_du_low_metrics_collector_impl() = default;
  split6_o_du_low_metrics_collector_impl(odu::o_du_low_metrics_collector*           odu_low_,
                                         ru_metrics_collector*                      ru_,
                                         split6_flexible_o_du_low_metrics_notifier* notifier_,
                                         unique_timer                               timer_,
                                         std::chrono::milliseconds                  report_period_);

  split6_o_du_low_metrics_collector_impl& operator=(split6_o_du_low_metrics_collector_impl&& other) noexcept;

  /// Delete unused constructors.
  split6_o_du_low_metrics_collector_impl(const split6_o_du_low_metrics_collector_impl&)            = delete;
  split6_o_du_low_metrics_collector_impl(split6_o_du_low_metrics_collector_impl&& other)           = delete;
  split6_o_du_low_metrics_collector_impl& operator=(const split6_o_du_low_metrics_collector_impl&) = delete;

  ~split6_o_du_low_metrics_collector_impl();

private:
  void collect_metrics();

private:
  unique_timer                               timer;
  std::chrono::milliseconds                  report_period{0};
  std::atomic<bool>                          stopped{true};
  split6_flexible_o_du_low_metrics_notifier* notifier = nullptr;
  odu::o_du_low_metrics_collector*           odu_low  = nullptr;
  ru_metrics_collector*                      ru       = nullptr;
};

} // namespace ocudu
