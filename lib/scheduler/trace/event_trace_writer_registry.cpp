// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "event_trace_writer_registry.h"
#include "../config/cell_configuration.h"
#include "../logging/cell_event_tracer.h"
#include "ocudu/adt/mpmc_queue.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/ran/subcarrier_spacing.h"
#include "ocudu/support/executors/execute_until_success.h"
#include "ocudu/support/executors/strand_executor.h"
#include "ocudu/support/synchronization/sync_event.h"
#include <algorithm>

using namespace ocudu::schedtrace;

/// \brief Function used to compute the required cell-specific tracer queue size.
/// \param max_scs Max SCS of the cell.
/// \param flush_period Period at which events are dequeued.
/// \return Computed queue size
static unsigned compute_required_queue_size(ocudu::subcarrier_spacing max_scs, std::chrono::milliseconds flush_period)
{
  // We apply a multiplicative and an adictive coefficient to the queue size to account for potential jitters in
  // the wake up of the consumer.
  const float    mult_coeff = 2;
  const unsigned add_coeff  = 64;
  return flush_period.count() * ocudu::get_nof_slots_per_subframe(max_scs) * mult_coeff + add_coeff;
}

/// Strand queue size.
/// \note The queue size is tiny because the cell event tracer stores all the pending events in an inner queue and
/// only dispatches a timer for flushing.
static constexpr unsigned strand_queue_size = 16;

event_trace_writer_registry::event_trace_writer_registry(std::chrono::milliseconds        flush_period_,
                                                         timer_manager&                   timers_,
                                                         task_executor&                   pool_executor,
                                                         const trace_writer_factory_type& factory) :
  timers(timers_), task_executor_ref(pool_executor), flush_period(flush_period_), trace_writer_factory(factory)
{
}

std::unique_ptr<cell_event_tracer>
event_trace_writer_registry::create_cell_tracer(const ocudu::cell_configuration& cell_cfg)
{
  cell_context& cell = cells[cell_cfg.cell_index];
  if (cell.strand == nullptr) {
    cell.strand = make_task_strand_ptr<concurrent_queue_policy::lockfree_mpmc>(task_executor_ref, strand_queue_size);
  }

  // Create the event queue of the new cell event tracer.
  const subcarrier_spacing max_scs  = std::max(cell_cfg.init_bwp.dl.cfg().scs, cell_cfg.init_bwp.ul.cfg().scs);
  auto                     ev_queue = std::make_unique<cell_event_channel>(
      cell_cfg.cell_index, compute_required_queue_size(max_scs, flush_period), ocudulog::fetch_basic_logger("SCHED"));
  auto tracer = std::make_unique<cell_event_tracer>(cell_cfg, *ev_queue);

  // The queue is installed in the cell strand, as the events of the previous tracer may still be pending.
  // Note: The queue ownership is passed as a raw pointer, because the tasks dispatched via defer_until_success must be
  // copyable. The queue is not leaked, as the install task always runs before the registry stop completes.
  defer_until_success(*cell.strand, timers, [this, cell_idx = cell_cfg.cell_index, queue = ev_queue.release()]() {
    install_queue(cell_idx, std::unique_ptr<cell_event_channel>(queue));
  });

  return tracer;
}

void event_trace_writer_registry::install_queue(du_cell_index_t cell_idx, std::unique_ptr<cell_event_channel> ev_queue)
{
  cell_context& cell = cells[cell_idx];

  if (cell.trace_writer == nullptr) {
    cell.trace_writer = trace_writer_factory(cell_idx);
    cell.flush_timer  = timers.create_unique_timer(*cell.strand);
    cell.flush_timer.set(flush_period, [this, &cell]() { handle_flush(cell); });
  }

  if (cell.ev_queue != nullptr) {
    // Flush the remaining events of the previous tracer, which was destroyed before its stop event got flushed.
    bool stop_received = not cell.trace_writer->on_flush_triggered(*cell.ev_queue);
    ocudu_assert(
        stop_received, "Cell event tracer for cell {} created while the previous one is still active", cell_idx);
  }

  cell.ev_queue = std::move(ev_queue);
  cell.flush_timer.run();
}

void event_trace_writer_registry::handle_flush(cell_context& cell)
{
  // On timer expiry, flush events and return slots to the free queue.
  if (not cell.trace_writer->on_flush_triggered(*cell.ev_queue)) {
    // Timer is not rearmed because an event was received to stop tracing.
    cell.ev_queue.reset();
    return;
  }

  // Rearm flush timer.
  cell.flush_timer.run();
}

void event_trace_writer_registry::close_cell(cell_context& cell)
{
  if (cell.ev_queue == nullptr) {
    // The events of the last tracer were already flushed.
    return;
  }
  cell.flush_timer.stop();
  // Flush the pending events, including the stop event, without waiting for the next timer tick.
  cell.trace_writer->on_flush_triggered(*cell.ev_queue);
  cell.ev_queue.reset();
}

void event_trace_writer_registry::stop()
{
  sync_event cells_closed;
  for (cell_context& cell : cells) {
    if (cell.strand == nullptr) {
      continue;
    }
    // The token is released once the task is destroyed, which happens after it has run.
    defer_until_success(*cell.strand, timers, [this, &cell, token = cells_closed.get_token()]() { close_cell(cell); });
  }

  // Wait for all the cells to be closed.
  cells_closed.wait();
}
