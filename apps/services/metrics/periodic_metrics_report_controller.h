// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "include/ocudu/support/ocudu_assert.h"
#include "include/ocudu/support/timers.h"
#include "metrics_producer.h"
#include "ocudu/support/executors/execute_until_success.h"
#include "ocudu/support/synchronization/sync_event.h"

namespace ocudu {
namespace app_services {

/// This controller class uses unique timer and triggers new metrics generation by the registered producers based on a
/// configured period.
class periodic_metrics_report_controller
{
public:
  /// Constructor receives timer object, report period and application metric configs.
  periodic_metrics_report_controller(std::vector<metrics_producer*> producers_,
                                     timer_manager&                 timers_,
                                     task_executor&                 executor_,
                                     std::chrono::milliseconds      report_period_) :
    executor(executor_),
    timers(timers_),
    timer(timers.create_unique_timer(executor)),
    report_period(report_period_),
    producers(std::move(producers_))
  {
    ocudu_assert(timer.is_valid(), "Invalid timer passed to metrics controller");
    timer.set(report_period, [this]() { report_metrics(); });
  }

  /// Starts the metrics report timer.
  void start()
  {
    if (!report_period.count()) {
      return;
    }
    stop_manager.reset();

    sync_event wait_all;
    defer_until_success(executor, timers, [this, token = wait_all.get_token()]() mutable { timer.run(); });
    // Block waiting for the controller to start.
    wait_all.wait();
  }

  /// Stops the metrics report timer.
  void stop()
  {
    if (!report_period.count()) {
      return;
    }

    // Signal stop to asynchronous timer thread.
    stop_manager.stop();
    // Stop the timer.
    timer.stop();
  }

private:
  /// Trigger metrics report in all registered producers.
  void report_metrics()
  {
    auto token = stop_manager.get_token();
    // Do not rearm the timer and process metrics if stop was requested.
    if (OCUDU_UNLIKELY(token.is_stop_requested())) {
      return;
    }

    // Rearm the timer.
    timer.run();

    // Command the producers to report their accumulated metrics.
    for (auto* producer : producers) {
      producer->on_new_report_period();
    }
  }

  task_executor& executor;
  timer_manager& timers;

  /// Timer object armed for configured report period.
  unique_timer timer;
  /// Metrics report period.
  std::chrono::milliseconds report_period{0};
  /// Manager used for stopping this controller.
  stop_event_source stop_manager;
  /// List of metrics producers managed by this controller.
  std::vector<metrics_producer*> producers;
};

} // namespace app_services
} // namespace ocudu
