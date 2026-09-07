// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "fbs/cell_event_generated.h"
#include "lib/scheduler/trace/event_trace_writer_registry.h"
#include "trace_test_utils.h"
#include "ocudu/scheduler/config/bwp_configuration.h"
#include "ocudu/scheduler/result/sched_result.h"
#include "ocudu/support/executors/manual_task_worker.h"
#include "ocudu/support/timers.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace schedtrace;
using namespace ocudu::schedtrace::test_helper;

namespace {

class event_aggregator
{
public:
  std::array<std::vector<std::vector<uint8_t>>, MAX_NOF_DU_CELLS> cell_events;

  const fbs::CellEvent& event(du_cell_index_t cell_idx, size_t i) const
  {
    return *fbs::GetSizePrefixedCellEvent(cell_events[cell_idx][i].data());
  }
};

class mock_event_writer : public event_trace_writer
{
public:
  mock_event_writer(du_cell_index_t cell_idx, event_aggregator& aggr_) : cell_index(cell_idx), aggr(aggr_) {}

  bool on_flush_triggered(cell_event_channel& ev_queue) override
  {
    bool stop_received = false;
    while (ev_queue.consume([this, &stop_received](span<const uint8_t> ev) {
      write_event(ev);
      if (fbs::GetSizePrefixedCellEvent(ev.data())->value_type() == fbs::CellEventValue::CellStopEvent) {
        stop_received = true;
      }
    })) {
      if (stop_received) {
        // If STOP command is received, stop handling new events.
        break;
      }
    }
    // Returns false to signal that the trace writer has been ordered to stop.
    return not stop_received;
  }

private:
  void write_event(span<const uint8_t> ev) { aggr.cell_events[cell_index].emplace_back(ev.begin(), ev.end()); }

  const du_cell_index_t cell_index;
  event_aggregator&     aggr;
};

} // namespace

class event_trace_writer_registry_test : public ::testing::Test
{
protected:
  static constexpr std::chrono::milliseconds flush_period{10};

  event_aggregator            written_events;
  timer_manager               timers{8};
  manual_task_worker          worker{16};
  event_trace_writer_registry registry{flush_period, timers, worker, [this](du_cell_index_t cell_idx) {
                                         return std::make_unique<mock_event_writer>(cell_idx, written_events);
                                       }};
};

TEST_F(event_trace_writer_registry_test, when_no_cells_are_created_then_no_events_are_pushed)
{
  for (unsigned i = 0; i != flush_period.count(); ++i) {
    timers.tick();
  }
  worker.run_pending_tasks();

  ASSERT_TRUE(written_events.cell_events[0].empty());
}

TEST_F(event_trace_writer_registry_test, when_cell_is_created_and_slot_pushed_then_event_is_flushed_on_timer_tick)
{
  const auto& cell_cfg = make_test_cell_cfg(to_du_cell_index(0));
  auto        tracer   = registry.create_cell_tracer(cell_cfg);

  sched_result result;
  result.success = true;
  tracer->on_scheduler_result(slot_point{0, 5}, result, std::chrono::microseconds(100));

  // Tick until flush period elapses.
  for (unsigned i = 0; i != flush_period.count(); ++i) {
    timers.tick();
  }
  worker.run_pending_tasks();

  ASSERT_GE(written_events.cell_events[0].size(), 2U);
  const fbs::CellEvent& start_ev = written_events.event(to_du_cell_index(0), 0);
  ASSERT_EQ(start_ev.value_type(), fbs::CellEventValue::CellStartEvent);
  check_bwp_cfg(start_ev.value_as_CellStartEvent()->init_ul_bwp(), cell_cfg.init_bwp.ul.cfg());
  check_bwp_cfg(start_ev.value_as_CellStartEvent()->init_dl_bwp(), cell_cfg.init_bwp.dl.cfg());
  const fbs::CellEvent& slot_ev = written_events.event(to_du_cell_index(0), 1);
  ASSERT_EQ(slot_ev.value_type(), fbs::CellEventValue::CellSlotEvent);
  ASSERT_EQ(slot_ev.value_as_CellSlotEvent()->slot_tx(), slot_point(0, 5).count());
}

TEST_F(event_trace_writer_registry_test, when_multiple_cells_are_created_then_each_gets_its_own_events)
{
  auto tracer0 = registry.create_cell_tracer(make_test_cell_cfg(to_du_cell_index(0)));
  auto tracer1 = registry.create_cell_tracer(make_test_cell_cfg(to_du_cell_index(1)));

  sched_result result;
  result.success = true;
  tracer0->on_scheduler_result(slot_point{0, 1}, result, std::chrono::microseconds(0));
  tracer1->on_scheduler_result(slot_point{0, 2}, result, std::chrono::microseconds(0));

  for (unsigned i = 0; i != flush_period.count(); ++i) {
    timers.tick();
  }
  worker.run_pending_tasks();

  ASSERT_GE(written_events.cell_events[0].size(), 2U);
  ASSERT_GE(written_events.cell_events[1].size(), 2U);
  ASSERT_EQ(written_events.event(to_du_cell_index(0), 1).value_as_CellSlotEvent()->slot_tx(), slot_point(0, 1).count());
  ASSERT_EQ(written_events.event(to_du_cell_index(1), 1).value_as_CellSlotEvent()->slot_tx(), slot_point(0, 2).count());
}

TEST_F(event_trace_writer_registry_test, when_tracer_is_destroyed_then_stop_event_is_flushed_and_channel_is_freed)
{
  {
    auto tracer = registry.create_cell_tracer(make_test_cell_cfg(to_du_cell_index(0)));
    // Tracer goes out of scope, pushing a STOP event to the queue.
  }

  for (unsigned i = 0; i != flush_period.count(); ++i) {
    timers.tick();
  }
  worker.run_pending_tasks();

  // The STOP event should have been flushed.
  ASSERT_FALSE(written_events.cell_events[0].empty());
  const size_t last = written_events.cell_events[0].size() - 1;
  ASSERT_EQ(written_events.event(to_du_cell_index(0), last).value_type(), fbs::CellEventValue::CellStopEvent);

  // After channel destruction, the same cell index can be reused.
  ASSERT_NO_FATAL_FAILURE(registry.create_cell_tracer(make_test_cell_cfg(to_du_cell_index(0))));
}

TEST_F(event_trace_writer_registry_test, when_multiple_slots_are_pushed_then_all_are_flushed_in_one_period)
{
  const auto& cell_cfg = make_test_cell_cfg(to_du_cell_index(0));
  auto        tracer   = registry.create_cell_tracer(cell_cfg);

  sched_result result;
  result.success = true;
  for (unsigned slot = 0; slot < 5; ++slot) {
    tracer->on_scheduler_result(slot_point{0, slot}, result, std::chrono::microseconds(slot * 10));
  }

  for (unsigned i = 0; i != flush_period.count(); ++i) {
    timers.tick();
  }
  worker.run_pending_tasks();

  ASSERT_EQ(written_events.cell_events[0].size(), 6U); // 1 START + 5 slot events
  const fbs::CellEvent& start_ev = written_events.event(to_du_cell_index(0), 0);
  ASSERT_EQ(start_ev.value_type(), fbs::CellEventValue::CellStartEvent);
  check_bwp_cfg(start_ev.value_as_CellStartEvent()->init_ul_bwp(), cell_cfg.init_bwp.ul.cfg());
  check_bwp_cfg(start_ev.value_as_CellStartEvent()->init_dl_bwp(), cell_cfg.init_bwp.dl.cfg());
  for (unsigned slot = 0; slot < 5; ++slot) {
    ASSERT_EQ(written_events.event(to_du_cell_index(0), 1 + slot).value_as_CellSlotEvent()->slot_tx(),
              slot_point(0, slot).count());
  }
}
