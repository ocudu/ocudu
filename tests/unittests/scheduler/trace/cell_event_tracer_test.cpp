// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "fbs/cell_event_generated.h"
#include "lib/scheduler/logging/cell_event_tracer.h"
#include "lib/scheduler/trace/cell_event_channel.h"
#include "lib/scheduler/trace/event_converter.h"
#include "roundtrip/full_sched_result.h"
#include "tests/test_doubles/scheduler/trace/test_messages.h"
#include "trace_test_utils.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/scheduler/result/sched_result.h"
#include "ocudu/scheduler/scheduler_rach_handler.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace schedtrace;
using namespace schedtrace::roundtrip_test;
using namespace test_helper;

/// Helper that owns the pool + queues and provides a convenient pop interface for tests.
struct test_queue_set {
  static constexpr unsigned queue_capacity = 128;

  cell_event_channel ev_queue{to_du_cell_index(0), queue_capacity, ocudulog::fetch_basic_logger("SCHED")};

  /// Pops the next completed event, invokes \p fn with the decoded CellEvent, then returns the builder to the pool.
  /// Returns false if the work queue is empty.
  template <typename Fn>
  bool pop_and_process(Fn&& fn)
  {
    return ev_queue.consume([&fn](span<const uint8_t> bytes) {
      flatbuffers::Verifier verifier(bytes.data(), bytes.size());
      ASSERT_TRUE(fbs::VerifySizePrefixedCellEventBuffer(verifier));
      fn(*fbs::GetSizePrefixedCellEvent(bytes.data()));
    });
  }
};

class cell_event_tracer_test : public testing::Test
{
protected:
  test_queue_set                   qs;
  const ocudu::cell_configuration& cell_cfg = make_test_cell_cfg(to_du_cell_index(0));
  cell_event_tracer                tracer{cell_cfg, qs.ev_queue};

  void SetUp() override
  {
    // Drain and verify the start event pushed by the constructor.
    bool popped = qs.pop_and_process([this](const fbs::CellEvent& ev) {
      ASSERT_EQ(ev.value_type(), fbs::CellEventValue::CellStartEvent);
      const fbs::CellStartEvent& start = *ev.value_as_CellStartEvent();
      ASSERT_EQ(start.pci(), static_cast<uint16_t>(cell_cfg.cell_index));
      check_bwp_cfg(start.init_ul_bwp(), cell_cfg.init_bwp.ul.cfg());
      check_bwp_cfg(start.init_dl_bwp(), cell_cfg.init_bwp.dl.cfg());
    });
    ASSERT_TRUE(popped);
  }
};

/// Serializes \p result into a standalone SlotDecision buffer.
static std::vector<uint8_t> decision_to_bytes(const sched_result& result)
{
  flatbuffers::FlatBufferBuilder fbb;
  fbb.Finish(convert_decision_to_fb(fbb, result));
  return {fbb.GetBufferPointer(), fbb.GetBufferPointer() + fbb.GetSize()};
}

/// Asserts that \p actual matches \p result: converts \p actual back to a sched_result and compares the deterministic
/// re-serialization of both.
static void check_slot_decision(const fbs::SlotDecision* actual, const ocudu::sched_result& result)
{
  ASSERT_NE(actual, nullptr);
  sched_result actual_native;
  convert_fb_to_decision(actual_native, *actual, make_test_schedtrace_cell_cfg());
  EXPECT_EQ(decision_to_bytes(actual_native), decision_to_bytes(result));
}

TEST(custom_cell_event_tracer_test, when_disabled_tracer_receives_slot_result_then_no_event_is_emitted)
{
  cell_event_tracer disabled_tracer;

  const sched_result result = make_full_sched_result();
  disabled_tracer.on_scheduler_result(slot_point{0, 0}, result, std::chrono::microseconds(50));
}

TEST(custom_cell_event_tracer_test, when_enabled_tracer_is_created_and_destroyed_then_start_and_stop_events_are_pushed)
{
  test_queue_set                   qs;
  const ocudu::cell_configuration& cell_cfg = make_test_cell_cfg(to_du_cell_index(0));

  {
    cell_event_tracer local_tracer{cell_cfg, qs.ev_queue};

    // Constructor pushes a START event.
    bool popped = qs.pop_and_process([&cell_cfg](const fbs::CellEvent& ev) {
      ASSERT_EQ(ev.value_type(), fbs::CellEventValue::CellStartEvent);
      const fbs::CellStartEvent& start = *ev.value_as_CellStartEvent();
      EXPECT_EQ(start.pci(), static_cast<uint16_t>(cell_cfg.cell_index));
      check_bwp_cfg(start.init_ul_bwp(), cell_cfg.init_bwp.ul.cfg());
      check_bwp_cfg(start.init_dl_bwp(), cell_cfg.init_bwp.dl.cfg());
    });
    ASSERT_TRUE(popped);
    ASSERT_TRUE(qs.ev_queue.empty());
    // Destructor of local_tracer pushes a STOP event.
  }

  bool popped = qs.pop_and_process([](const fbs::CellEvent& ev) {
    ASSERT_EQ(ev.value_type(), fbs::CellEventValue::CellStopEvent);
    ASSERT_EQ(ev.value_as_CellStopEvent()->cell_idx(), 0U);
  });
  ASSERT_TRUE(popped);
}

TEST_F(cell_event_tracer_test, when_slot_result_has_no_allocations_then_decision_is_absent)
{
  slot_point   sl_tx{0, 3};
  sched_result result;
  result.success = true;

  tracer.on_scheduler_result(sl_tx, result, std::chrono::microseconds(0));

  bool popped = qs.pop_and_process([sl_tx](const fbs::CellEvent& ev) {
    ASSERT_EQ(ev.value_type(), fbs::CellEventValue::CellSlotEvent);
    const fbs::CellSlotEvent& slot_ev = *ev.value_as_CellSlotEvent();
    ASSERT_EQ(slot_ev.slot_tx(), sl_tx.count());
    // Nothing was scheduled and no allocation attempt failed; the decision table is skipped entirely.
    ASSERT_EQ(slot_ev.decision(), nullptr);
  });
  ASSERT_TRUE(popped);
}

TEST_F(cell_event_tracer_test, when_slot_result_is_pushed_to_tracer_then_a_slot_event_is_forwarded_to_backend)
{
  ASSERT_TRUE(qs.ev_queue.empty());

  // Push slot result.
  const slot_point   sl_tx{0, 5};
  const sched_result result = make_full_sched_result();
  tracer.on_scheduler_result(sl_tx, result, std::chrono::milliseconds(100));

  bool popped = qs.pop_and_process([&sl_tx, &result](const fbs::CellEvent& ev) {
    ASSERT_EQ(ev.value_type(), fbs::CellEventValue::CellSlotEvent);
    EXPECT_GT(ev.timestamp_us(), 0);
    const fbs::CellSlotEvent& slot_ev = *ev.value_as_CellSlotEvent();

    ASSERT_EQ(slot_ev.slot_tx(), sl_tx.count());
    ASSERT_EQ(slot_ev.latency_us(), std::chrono::microseconds(std::chrono::milliseconds(100)).count());
    ASSERT_EQ(slot_ev.inputs(), nullptr);
    check_slot_decision(slot_ev.decision(), result);
  });
  ASSERT_TRUE(popped);
}

TEST_F(cell_event_tracer_test, when_rach_indication_is_traced_then_slot_event_includes_rach_input)
{
  rach_indication_message rach_msg;
  rach_msg.cell_index = to_du_cell_index(0);
  rach_msg.slot_rx    = slot_point{0, 2};

  tracer.on_event(rach_msg);

  sched_result result;
  result.success = true;
  tracer.on_scheduler_result(slot_point{0, 2}, result, std::chrono::microseconds(0));

  bool popped = qs.pop_and_process([](const fbs::CellEvent& ev) {
    const fbs::CellSlotEvent& slot_ev = *ev.value_as_CellSlotEvent();
    ASSERT_NE(slot_ev.inputs(), nullptr);
    ASSERT_EQ(slot_ev.inputs()->size(), 1U);
    ASSERT_EQ(slot_ev.inputs_type()->Get(0), fbs::SlotInput::RachIndication);
  });
  ASSERT_TRUE(popped);
}

TEST_F(cell_event_tracer_test, when_multiple_slot_results_are_pushed_then_each_produces_a_separate_event)
{
  sched_result result;
  result.success = true;

  tracer.on_scheduler_result(slot_point{0, 1}, result, std::chrono::microseconds(10));
  tracer.on_scheduler_result(slot_point{0, 2}, result, std::chrono::microseconds(20));
  tracer.on_scheduler_result(slot_point{0, 3}, result, std::chrono::microseconds(30));

  bool popped = qs.pop_and_process([](const fbs::CellEvent& ev) {
    ASSERT_EQ(ev.value_as_CellSlotEvent()->slot_tx(), slot_point(0, 1).count());
    ASSERT_EQ(ev.value_as_CellSlotEvent()->latency_us(), 10);
  });
  ASSERT_TRUE(popped);

  popped = qs.pop_and_process([](const fbs::CellEvent& ev) {
    ASSERT_EQ(ev.value_as_CellSlotEvent()->slot_tx(), slot_point(0, 2).count());
    ASSERT_EQ(ev.value_as_CellSlotEvent()->latency_us(), 20);
  });
  ASSERT_TRUE(popped);

  popped = qs.pop_and_process([](const fbs::CellEvent& ev) {
    ASSERT_EQ(ev.value_as_CellSlotEvent()->slot_tx(), slot_point(0, 3).count());
    ASSERT_EQ(ev.value_as_CellSlotEvent()->latency_us(), 30);
  });
  ASSERT_TRUE(popped);

  ASSERT_TRUE(qs.ev_queue.empty());
}

TEST(custom_cell_event_tracer_test, when_event_queue_is_full_push_is_dropped_but_allocator_slot_is_never_exhausted)
{
  // Use a small queue so it is easy to fill without many iterations.
  static constexpr unsigned cap = 4;

  cell_event_channel ev_queue{to_du_cell_index(0), cap, ocudulog::fetch_basic_logger("SCHED")};
  cell_event_tracer  tracer{make_test_cell_cfg(to_du_cell_index(0)), ev_queue};

  auto consume_and_check_slot = [&ev_queue](uint32_t expected_slot_count) {
    return ev_queue.consume([expected_slot_count](span<const uint8_t> bytes) {
      const fbs::CellEvent& ev = *fbs::GetSizePrefixedCellEvent(bytes.data());
      EXPECT_EQ(ev.value_as_CellSlotEvent()->slot_tx(), expected_slot_count);
    });
  };

  // Drain the start event.
  {
    bool consumed = ev_queue.consume([](span<const uint8_t> bytes) {
      EXPECT_EQ(fbs::GetSizePrefixedCellEvent(bytes.data())->value_type(), fbs::CellEventValue::CellStartEvent);
    });
    ASSERT_TRUE(consumed);
  }

  sched_result result;
  result.success = true;

  // Fill the event queue to capacity.
  slot_point next_slot{0, 0};
  for (unsigned i = 0; i != cap; ++i) {
    tracer.on_scheduler_result(next_slot++, result, std::chrono::microseconds(i));
  }
  ASSERT_EQ(ev_queue.size(), cap);

  // Event queue is now full — these pushes should be dropped.
  // Note: the current builder must remain valid: the pool is NOT called on a failed push (the existing builder is
  // reused), so it stays non-null.
  tracer.on_scheduler_result(next_slot++, result, std::chrono::microseconds(0));
  tracer.on_scheduler_result(next_slot++, result, std::chrono::microseconds(0));

  // Drain one event, returning its builder to the pool's free list.
  ASSERT_TRUE(consume_and_check_slot(0));

  // A fresh push must succeed after one builder was freed.
  auto last_ev_slot = next_slot++;
  tracer.on_scheduler_result(last_ev_slot, result, std::chrono::microseconds(0));

  // Drain remaining cap-1 old events and verify their order.
  for (unsigned i = 1; i < cap; ++i) {
    ASSERT_TRUE(consume_and_check_slot(i));
  }

  // The final event must be the one we just pushed.
  ASSERT_TRUE(consume_and_check_slot(last_ev_slot.count()));
}

TEST_F(cell_event_tracer_test, when_rach_event_precedes_second_slot_result_then_first_slot_result_has_no_rach_input)
{
  sched_result result;
  result.success = true;

  // Push first slot result - no RACH event before it.
  tracer.on_scheduler_result(slot_point{0, 1}, result, std::chrono::microseconds(0));

  // Push RACH event, then second slot result.
  rach_indication_message rach_msg;
  rach_msg.cell_index = to_du_cell_index(0);
  rach_msg.slot_rx    = slot_point{0, 2};
  tracer.on_event(rach_msg);
  tracer.on_scheduler_result(slot_point{0, 2}, result, std::chrono::microseconds(0));

  bool popped =
      qs.pop_and_process([](const fbs::CellEvent& ev) { ASSERT_EQ(ev.value_as_CellSlotEvent()->inputs(), nullptr); });
  ASSERT_TRUE(popped);

  popped = qs.pop_and_process([](const fbs::CellEvent& ev) {
    ASSERT_NE(ev.value_as_CellSlotEvent()->inputs(), nullptr);
    ASSERT_EQ(ev.value_as_CellSlotEvent()->inputs()->size(), 1U);
  });
  ASSERT_TRUE(popped);
}
