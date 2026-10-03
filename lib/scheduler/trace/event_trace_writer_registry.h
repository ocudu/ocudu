// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "cell_event_channel.h"
#include "ocudu/ran/du_cell_index.h"
#include "ocudu/support/executors/task_executor.h"
#include "ocudu/support/timers.h"
#include <array>
#include <chrono>
#include <functional>
#include <memory>

namespace ocudu {

class cell_configuration;

namespace schedtrace {

class cell_event_tracer;

/// \brief Interface to flush events to a sink.
class event_trace_writer
{
public:
  virtual ~event_trace_writer() = default;

  /// \brief Flushes events to the sink. Called periodically by the consumer.
  /// \return Returns false if the trace writer has been ordered to stop.
  virtual bool on_flush_triggered(cell_event_channel& ev_queue) = 0;
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

  /// \brief Creates and registers a cell event tracer.
  /// \remark The previous cell event tracer of the same cell, if any, must have been destroyed beforehand.
  std::unique_ptr<cell_event_tracer> create_cell_tracer(const ocudu::cell_configuration& cell_cfg);

  /// \brief Flushes the pending events of all cells and stops their periodic flush.
  /// \remark All cell event tracers must have been destroyed beforehand.
  void stop();

private:
  /// Resources associated with a cell index, reused across the cell event tracers of the same cell.
  struct cell_context {
    /// Strand where the cell resources are accessed, except for its own creation.
    std::unique_ptr<task_executor> strand;
    /// Timer that triggers periodically to flush the events.
    unique_timer flush_timer;
    /// Handler of the events of the cell.
    std::unique_ptr<event_trace_writer> trace_writer;
    /// Queue of the events of the current cell event tracer, if any.
    std::unique_ptr<cell_event_channel> ev_queue;
  };

  void install_queue(du_cell_index_t cell_idx, std::unique_ptr<cell_event_channel> ev_queue);

  void handle_flush(cell_context& cell);

  void close_cell(cell_context& cell);

  timer_manager&            timers;
  task_executor&            task_executor_ref;
  std::chrono::milliseconds flush_period;
  /// Factory used to generate the trace writer of each cell.
  trace_writer_factory_type trace_writer_factory;

  std::array<cell_context, MAX_NOF_DU_CELLS> cells;
};

} // namespace schedtrace
} // namespace ocudu
