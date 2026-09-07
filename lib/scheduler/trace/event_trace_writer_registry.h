// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../logging/cell_event_tracer.h"
#include "cell_event_channel.h"
#include "ocudu/scheduler/config/bwp_configuration.h"
#include "ocudu/support/executors/task_executor.h"
#include "ocudu/support/timers.h"

namespace ocudu::schedtrace {

/// \brief Interface to flush events to a sink.
class event_trace_writer
{
public:
  virtual ~event_trace_writer() = default;

  /// \brief Flushes events to the sink. Called periodically by the consumer.
  /// \return Returns false if the trace writer has been ordered to stop.
  virtual bool on_flush_triggered(cell_event_channel& ev_queue) = 0;
};

class event_trace_writer_registry;

/// This class provides a channel between backend and frontend for cell event tracing propagation and flushing of
/// events to a event_trace_writer instance.
class cell_event_trace_consumer
{
public:
  cell_event_trace_consumer(event_trace_writer_registry&        parent_,
                            du_cell_index_t                     cell_idx,
                            subcarrier_spacing                  max_scs,
                            std::chrono::milliseconds           sleep_period,
                            timer_manager&                      timers,
                            task_executor&                      pool_executor,
                            std::unique_ptr<event_trace_writer> writer);

  /// \brief Create a cell event tracer associated with this channel.
  /// \return cell event trace producer.
  std::unique_ptr<schedtrace::cell_event_tracer> create_producer(const ocudu::cell_configuration& cell_cfg);

private:
  event_trace_writer_registry& parent;
  du_cell_index_t              cell_index;
  /// Strand task executor used by this cell.
  std::unique_ptr<task_executor> cell_trace_executor;
  /// Timer that triggers periodically to flush the events.
  unique_timer flush_timer;
  /// Queue of pending events to be processed by the backend.
  cell_event_channel ev_queue;
  /// Handler of the events at the backend.
  std::unique_ptr<event_trace_writer> trace_writer;
};

/// Component that creates and registers the active cell event tracers.
class event_trace_writer_registry
{
public:
  using trace_writer_factory_type = std::function<std::unique_ptr<event_trace_writer>(du_cell_index_t)>;

  event_trace_writer_registry(std::chrono::milliseconds        flush_period_,
                              timer_manager&                   timers_,
                              task_executor&                   pool_executor,
                              const trace_writer_factory_type& factory);

  /// Creates and registers a cell event tracer.
  std::unique_ptr<cell_event_tracer> create_cell_tracer(const ocudu::cell_configuration& cell_cfg);

private:
  friend class cell_event_trace_consumer;

  void handle_cell_destruction(du_cell_index_t cell_idx);

  timer_manager&            timers;
  task_executor&            task_executor_ref;
  std::chrono::milliseconds flush_period;
  /// Factory used to generate tracer writers for each cell event trace consumer.
  trace_writer_factory_type trace_writer_factory;

  std::array<std::unique_ptr<cell_event_trace_consumer>, MAX_NOF_DU_CELLS> channels;
};

} // namespace ocudu::schedtrace
