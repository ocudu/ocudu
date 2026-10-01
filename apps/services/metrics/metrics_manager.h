// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "metrics_channel.h"
#include "periodic_metrics_report_controller.h"
#include "ocudu/adt/span.h"
#include "ocudu/ocudulog/logger.h"
#include "ocudu/support/executors/task_executor.h"
#include "ocudu/support/ocudu_assert.h"
#include "ocudu/support/synchronization/stop_event.h"
#include <chrono>
#include <vector>

namespace ocudu {
namespace app_services {

/// \brief Metrics manager application service.
///
/// The manager takes ownership of the metrics channels created by the application/unit builders
/// and connects them to the runtime services they need (metrics executor, logger and stop event),
/// and drives their producers through the periodic report controller. Metric values never reach
/// the manager: they stay inside the typed channels. The manager therefore needs no knowledge of
/// the metric types that exist, and a new metric type can be added without touching the framework.
///
/// \note \ref stop (and the destructor) block until every in-flight deferred report has run to
/// completion. Consequently, \ref stop must never be called from a context that the metrics
/// executor cannot make progress without, in particular from inside a \c handle_metric callback
/// running on the metrics executor itself: the stop token of the currently running report can
/// only be released once that callback returns, which would deadlock.
class metrics_manager
{
public:
  metrics_manager(ocudulog::basic_logger&    logger_,
                  task_executor&             executor_,
                  metrics_channel_collection channels_,
                  timer_manager&             timers,
                  std::chrono::milliseconds  report_period) :
    logger(logger_),
    executor(executor_),
    periodic_controller(collect_producers(channels_), timers, executor_, report_period)
  {
    for (auto& channel : channels_) {
      ocudu_assert(channel != nullptr, "Invalid metrics channel");
      channel->connect(ctx);
    }
    channels = std::move(channels_);
  }

  /// \brief Stops the periodic controller, drains every in-flight report, disconnects every
  /// channel and destroys the channels together with the manager.
  ///
  /// Calling \ref stop here (instead of relying on the member destruction order) makes the
  /// drain explicit: by the time the member destructors run, no deferred report can be in
  /// flight anymore. Producers must complete all their pushes before the manager is destroyed:
  /// the manager owns the channels, so a channel reference held by a producer dangles after
  /// destruction.
  ~metrics_manager()
  {
    stop();

    for (auto& channel : channels) {
      channel->disconnect();
    }
  }

  /// Starts the periodic metrics report controller.
  void start()
  {
    stop_manager.reset();

    periodic_controller.start();
  }

  /// \brief Stops the periodic metrics report controller.
  ///
  /// Blocks until every in-flight deferred metrics report has run to completion. Must not be
  /// called from inside a consumer callback running on the metrics executor (see the class-level
  /// note for the reasoning).
  void stop()
  {
    periodic_controller.stop();

    stop_manager.stop();
  }

private:
  /// Collects the producers of every channel for the periodic report controller.
  static std::vector<metrics_producer*> collect_producers(const metrics_channel_collection& channels)
  {
    std::vector<metrics_producer*> producers;
    for (auto& channel : channels) {
      ocudu_assert(channel != nullptr, "Invalid metrics channel");
      for (auto* producer : channel->get_producers()) {
        producers.push_back(producer);
      }
    }
    return producers;
  }

  /// \brief Member ordering note.
  ///
  /// \c stop_manager (together with \c ctx, which references it) is deliberately declared
  /// **last** as a second line of defence: even without the \ref stop call in the destructor,
  /// its destruction would still drain every in-flight deferred report before \c channels, and
  /// with them the consumers, are destroyed. With \ref stop in the destructor this ordering is
  /// a redundant safety net rather than a load-bearing guarantee.
  ocudulog::basic_logger&            logger;
  task_executor&                     executor;
  metrics_channel_collection         channels;
  periodic_metrics_report_controller periodic_controller;
  stop_event_source                  stop_manager;
  metrics_runtime_context            ctx{executor, logger, stop_manager};
};

} // namespace app_services
} // namespace ocudu
