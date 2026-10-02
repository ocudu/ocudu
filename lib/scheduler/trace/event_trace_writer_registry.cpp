// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "event_trace_writer_registry.h"
#include "../config/cell_configuration.h"
#include "ocudu/adt/mpmc_queue.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/ran/bwp/bwp_configuration.h"
#include "ocudu/support/executors/execute_until_success.h"
#include "ocudu/support/executors/strand_executor.h"
#include "ocudu/support/synchronization/sync_event.h"

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

cell_event_trace_consumer::cell_event_trace_consumer(du_cell_index_t                     cell_idx,
                                                     unsigned                            queue_size,
                                                     std::unique_ptr<event_trace_writer> writer) :
  ev_queue(cell_idx, queue_size, ocudulog::fetch_basic_logger("SCHED")), trace_writer(std::move(writer))
{
}

std::unique_ptr<cell_event_tracer> cell_event_trace_consumer::create_producer(const cell_configuration& cell_cfg)
{
  return std::make_unique<cell_event_tracer>(cell_cfg, ev_queue);
}

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
  cell_context& cell = channels[cell_cfg.cell_index];
  ocudu_assert(not cell.active.load(std::memory_order_acquire),
               "Cell event tracer for cell {} already exists",
               cell_cfg.cell_index);
  if (cell.strand == nullptr) {
    cell.strand = make_task_strand_ptr<concurrent_queue_policy::lockfree_mpmc>(task_executor_ref, strand_queue_size);
    cell.flush_timer = timers.create_unique_timer(*cell.strand);
    cell.flush_timer.set(flush_period, [this, &cell]() { handle_flush(cell); });
  }
  cell.active.store(true, std::memory_order_relaxed);

  // Creates a cell trace consumer.
  const subcarrier_spacing max_scs    = std::max(cell_cfg.init_bwp.dl.cfg().scs, cell_cfg.init_bwp.ul.cfg().scs);
  const unsigned           queue_size = compute_required_queue_size(max_scs, flush_period);
  cell.consumer                       = std::make_unique<cell_event_trace_consumer>(
      cell_cfg.cell_index, queue_size, trace_writer_factory(cell_cfg.cell_index));

  // Start the periodic flush.
  cell.flush_timer.run();

  // Request consumer to provide a notifier.
  return cell.consumer->create_producer(cell_cfg);
}

void event_trace_writer_registry::handle_flush(cell_context& cell)
{
  // On timer expiry, flush events and return slots to the free queue.
  if (not cell.consumer->flush()) {
    // Timer is not rearmed because an event was received to stop tracing.
    destroy_consumer(cell);
    return;
  }

  // Rearm flush timer.
  cell.flush_timer.run();
}

void event_trace_writer_registry::close_cell(cell_context& cell)
{
  if (cell.consumer == nullptr) {
    // The consumer is already destroyed.
    return;
  }
  cell.flush_timer.stop();
  // Flush the pending events, including the stop event, without waiting for the next timer tick.
  cell.consumer->flush();
  destroy_consumer(cell);
}

void event_trace_writer_registry::destroy_consumer(cell_context& cell)
{
  // Note: The consumer can be destroyed from within the flush timer callback, as it does not own the timer.
  cell.consumer.reset();
  cell.active.store(false, std::memory_order_release);
}

void event_trace_writer_registry::stop()
{
  sync_event consumers_closed;
  for (cell_context& cell : channels) {
    if (not cell.active.load(std::memory_order_acquire)) {
      continue;
    }
    // The consumer may be destroyed concurrently, so its presence is only checked within the strand. The token is
    // released once the task is destroyed, which happens after it has run.
    defer_until_success(
        *cell.strand, timers, [this, &cell, token = consumers_closed.get_token()]() { close_cell(cell); });
  }

  // Wait for all the consumers to be destroyed.
  consumers_closed.wait();
}
