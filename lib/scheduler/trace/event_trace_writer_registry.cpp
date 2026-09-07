// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "event_trace_writer_registry.h"
#include "../config/cell_configuration.h"
#include "ocudu/adt/mpmc_queue.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/ran/bwp/bwp_configuration.h"
#include "ocudu/support/executors/strand_executor.h"

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

cell_event_trace_consumer::cell_event_trace_consumer(event_trace_writer_registry&        parent_,
                                                     du_cell_index_t                     cell_idx,
                                                     subcarrier_spacing                  max_scs,
                                                     std::chrono::milliseconds           sleep_period,
                                                     timer_manager&                      timers,
                                                     task_executor&                      executor,
                                                     std::unique_ptr<event_trace_writer> writer) :
  parent(parent_),
  cell_index(cell_idx),
  cell_trace_executor(make_task_strand_ptr<concurrent_queue_policy::lockfree_mpmc>(executor, strand_queue_size)),
  flush_timer(timers.create_unique_timer(*cell_trace_executor)),
  ev_queue(cell_idx, compute_required_queue_size(max_scs, sleep_period), ocudulog::fetch_basic_logger("SCHED")),
  trace_writer(std::move(writer))
{
  // Set up periodic flush timer.
  flush_timer.set(sleep_period, [this]() {
    // On timer expiry, flush events and return slots to the free queue.
    if (not trace_writer->on_flush_triggered(ev_queue)) {
      // Timer is not rearmed because an event was received to stop tracing.
      parent.handle_cell_destruction(cell_index);
      // Note: Careful not to touch any this member at this stage, as consumer might already have been destroyed.
      return;
    }

    // Rearm flush timer.
    flush_timer.run();
  });
}

std::unique_ptr<cell_event_tracer> cell_event_trace_consumer::create_producer(const cell_configuration& cell_cfg)
{
  ocudu_assert(not flush_timer.is_running(), "Flush timer is already running for cell {}", cell_index);

  // Start the timer.
  flush_timer.run();

  // Create and return the producer.
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
  ocudu_assert(
      channels[cell_cfg.cell_index] == nullptr, "Cell event tracer for cell {} already exists", cell_cfg.cell_index);

  // Creates a cell trace consumer.
  const subcarrier_spacing max_scs = std::max(cell_cfg.init_bwp.dl.cfg().scs, cell_cfg.init_bwp.ul.cfg().scs);

  auto  channel                 = std::make_unique<cell_event_trace_consumer>(*this,
                                                             cell_cfg.cell_index,
                                                             max_scs,
                                                             flush_period,
                                                             timers,
                                                             task_executor_ref,
                                                             trace_writer_factory(cell_cfg.cell_index));
  auto& channel_ref             = *channel;
  channels[cell_cfg.cell_index] = std::move(channel);

  // Request consumer to provide a notifier.
  return channel_ref.create_producer(cell_cfg);
}

void event_trace_writer_registry::handle_cell_destruction(du_cell_index_t cell_idx)
{
  // Called from within the cell tracer task executor.
  channels[cell_idx] = nullptr;
}
