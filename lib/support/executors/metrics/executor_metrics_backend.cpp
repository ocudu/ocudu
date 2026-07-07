// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/support/executors/metrics/executor_metrics_backend.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/support/executors/metrics/executor_metrics_notifier.h"
#include "ocudu/support/ocudu_assert.h"

using namespace ocudu;

void executor_metrics_backend::start(std::chrono::milliseconds  period_,
                                     unique_timer               timer_,
                                     executor_metrics_notifier& notifier_)
{
  if (notifier) {
    return;
  }

  period   = period_;
  timer    = std::move(timer_);
  notifier = &notifier_;

  stop_control.reset();

  timer.set(period, [this]() { fetch_metrics(); });
  timer.run();

  ocudulog::fetch_basic_logger("METRICS").info("Started the executor metrics backend worker");
}

void executor_metrics_backend::stop()
{
  // Signal stop to asynchronous timer thread.
  stop_control.stop();
  // Stop the timer.
  timer.stop();

  notifier = nullptr;

  ocudulog::fetch_basic_logger("METRICS").info("Stopped the executor metrics backend worker");
}

executor_metrics_backend::~executor_metrics_backend()
{
  stop();
}

executor_metrics_channel& executor_metrics_backend::add_channel(const std::string& exec_name)
{
  ocudu_assert(!notifier, "Cannot add new metrics channel when the backend is running");

  auto  unique_channel = std::make_unique<executor_metrics_channel>(exec_name);
  auto& channel        = *unique_channel.get();
  {
    std::unique_lock lock(mutex);
    channels.push_back(std::move(unique_channel));
  }
  return channel;
}

void executor_metrics_backend::fetch_metrics()
{
  auto token = stop_control.get_token();

  // Do not rearm the timer and process metrics if stop was requested.
  if (OCUDU_UNLIKELY(token.is_stop_requested())) {
    return;
  }

  // Rearm the timer.
  timer.run();

  ocudu_assert(notifier, "Invalid notifier");

  // Notify new metrics.
  for (auto& channel : channels) {
    executor_metrics report = channel->read();
    notifier->on_new_metrics(report);
  }
}
