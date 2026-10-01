// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "metrics_consumer.h"
#include "metrics_notifier.h"
#include "metrics_producer.h"
#include "ocudu/adt/span.h"
#include "ocudu/ocudulog/logger.h"
#include "ocudu/support/executors/task_executor.h"
#include "ocudu/support/ocudu_assert.h"
#include "ocudu/support/synchronization/stop_event.h"
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

namespace ocudu {
namespace app_services {

/// \brief Runtime services a channel needs to deliver its reports.
///
/// Provided by the metrics manager when it adopts the channel. Owned by the manager; a channel
/// only refers to it while connected.
struct metrics_runtime_context {
  /// Executor the report fan-out tasks are deferred to (must be able to make progress while
  /// producers push their reports).
  task_executor& executor;
  /// Logger used to report metrics that could not be deferred.
  ocudulog::basic_logger& logger;
  /// Stop event used to reject new reports and to drain the in-flight ones on shutdown.
  stop_event_source& stop;
};

/// \brief Type-erased handle of a metrics channel.
///
/// Metric values never cross this interface: it exists only so that the metrics manager can
/// uniformly connect channels of different metric types to its runtime context and command their
/// producers through the periodic report controller. The channels themselves stay owned by the
/// application's channel collection.
class metrics_channel_base
{
public:
  virtual ~metrics_channel_base() = default;

  /// \brief Connects the channel to the runtime services of the adopting metrics manager.
  ///
  /// Reports pushed while the channel is not connected are dropped. This mirrors the semantics of
  /// a unit whose producers are built before the application creates the metrics manager: no
  /// report can be delivered until the manager adopts the channel.
  virtual void connect(metrics_runtime_context& ctx) = 0;

  /// \brief Disconnects the channel from its metrics manager.
  ///
  /// Called by the manager when it is destroyed, before the channels themselves are destroyed:
  /// from that point on, the channel no longer refers to the manager's runtime context.
  virtual void disconnect() = 0;

  /// Returns the producers registered in this channel, to be commanded by the periodic report
  /// controller.
  virtual span<metrics_producer*> get_producers() = 0;

  /// Returns the number of consumers registered in this channel.
  virtual unsigned nof_consumers() const = 0;
};

/// \brief Typed channel connecting the producers of one metric type with its consumers.
///
/// A channel is created per concrete metric type by the application/unit builders (through \ref
/// make_channel), long before the metrics manager exists. Producers push reports through the
/// typed \ref metrics_notifier<MetricType> interface; the channel defers a single task to the
/// metrics executor that fans the report out to every registered consumer, which receives it
/// through the typed \ref metrics_consumer<MetricType> interface. The metric type therefore never
/// crosses a type-erased boundary: no cast, tag or type marker is involved anywhere on the
/// delivery path.
template <typename MetricType>
class metrics_channel final : public metrics_notifier<MetricType>, public metrics_channel_base
{
  static_assert(std::is_move_constructible_v<MetricType>,
                "MetricType must be move-constructible: the channel takes ownership of the report "
                "by moving it into the deferred task. A type with const members or a deleted move "
                "constructor would be silently copied (or fail to compile here) instead.");

public:
  /// \brief Registers a consumer that will receive every report pushed to this channel.
  void add_consumer(std::unique_ptr<metrics_consumer<MetricType>> consumer)
  {
    ocudu_assert(consumer != nullptr, "Invalid metrics consumer");
    consumers_snapshot.push_back(consumer.get());
    consumers.push_back(std::move(consumer));
  }

  /// \brief Registers a producer that the periodic report controller commands.
  void add_producer(std::unique_ptr<metrics_producer> producer)
  {
    ocudu_assert(producer != nullptr, "Invalid metrics producer");
    producers_snapshot.push_back(producer.get());
    producers.push_back(std::move(producer));
  }

  // See interface for documentation.
  void on_new_metric(MetricType metric) override
  {
    // Not connected to a metrics manager: the report cannot be delivered and is dropped.
    if (ctx == nullptr) {
      return;
    }
    stop_event_token token = ctx->stop.get_token();
    if (OCUDU_UNLIKELY(token.is_stop_requested())) {
      return;
    }
    if (consumers_snapshot.empty()) {
      return;
    }
    // The stop token is captured on purpose: its destructor decrements the manager's observer
    // count, so metrics_manager::stop() blocks until every in-flight deferred report, including
    // this one, has run to completion. Without it, the task could touch consumers after the
    // manager started tearing them down. For the same reason the task captures \c this instead of
    // a copy of the consumer snapshot: the channel (and with it \c consumers_snapshot) is
    // destroyed by the metrics manager only after every in-flight task has been drained, so the
    // pointers the task reads stay valid for as long as the task can still run. Consumers are
    // registered at build time, before the manager connects the channel, so the snapshot is never
    // mutated while a task may be in flight.
    //
    // Performance note: the deferred task may heap-allocate once per report, because its small
    // buffer (unique_task, 64 bytes) must hold the metric, the channel pointer and the stop
    // token; only metric types above ~48 bytes pay it. Reports arrive at most once per report
    // period per producer, so this is a handful of small allocations per second application-wide.
    // A report pool was deliberately not introduced for this: it would trade that negligible
    // allocation rate for a bounded capacity that silently drops reports, per-type capacity
    // tuning and extra lifetime coupling between the pool, the deferred tasks and the manager.
    if (!ctx->executor.defer([this, metric = std::move(metric), stop_token = std::move(token)]() {
          for (auto* consumer : consumers_snapshot) {
            consumer->handle_metric(metric);
          }
        })) {
      ctx->logger.error("Failed to defer a metrics report for its consumers");
    }
  }

  // See interface for documentation.
  void connect(metrics_runtime_context& ctx_) override { ctx = &ctx_; }

  // See interface for documentation.
  void disconnect() override { ctx = nullptr; }

  // See interface for documentation.
  span<metrics_producer*> get_producers() override { return producers_snapshot; }

  // See interface for documentation.
  unsigned nof_consumers() const override { return consumers_snapshot.size(); }

private:
  metrics_runtime_context*                                   ctx = nullptr;
  std::vector<std::unique_ptr<metrics_consumer<MetricType>>> consumers;
  // Snapshot of \c consumers that the deferred fan-out task reads through the channel pointer.
  // The pointers stay valid as long as the channel (and therefore \c consumers) is alive; every
  // in-flight task is drained before the metrics manager destroys the channel. Consumers are
  // registered at build time, before the manager connects the channel, so this vector is never
  // mutated while a deferred task may be reading it.
  std::vector<metrics_consumer<MetricType>*>     consumers_snapshot;
  std::vector<std::unique_ptr<metrics_producer>> producers;
  std::vector<metrics_producer*>                 producers_snapshot;
};

/// \brief Collection of metrics channels created by the application and unit builders.
///
/// Builders extend the collection through \ref make_channel, long before the metrics manager
/// exists; the application then moves the collection into the metrics manager, which owns the
/// channels for the rest of the application lifetime. The channels are heap-allocated, so moving
/// the collection never invalidates the references the producers and unit builders hold.
using metrics_channel_collection = std::vector<std::unique_ptr<metrics_channel_base>>;

/// \brief Creates a channel for the metric type \c MetricType in the given collection.
///
/// The channel is created typed and stored type-erased for ownership only, so no metric value
/// ever crosses a type-erased boundary. The returned reference remains valid for as long as the
/// channel exists, i.e. until the collection is destroyed (inside the metrics manager).
template <typename MetricType>
metrics_channel<MetricType>& make_channel(metrics_channel_collection& channel_list)
{
  auto  channel = std::make_unique<metrics_channel<MetricType>>();
  auto& ref     = *channel;
  channel_list.push_back(std::move(channel));
  return ref;
}

} // namespace app_services
} // namespace ocudu
