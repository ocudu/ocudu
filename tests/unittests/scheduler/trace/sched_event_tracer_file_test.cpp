// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "fbs/cell_event_generated.h"
#include "lib/scheduler/logging/cell_event_tracer.h"
#include "tests/test_doubles/scheduler/trace/test_messages.h"
#include "trace_test_utils.h"
#include "ocudu/instrumentation/traces/scheduler_event_tracer.h"
#include "ocudu/scheduler/result/sched_result.h"
#include "ocudu/support/executors/manual_task_worker.h"
#include "ocudu/support/timers.h"
#include <chrono>
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>

using namespace ocudu;
using namespace schedtrace;
using namespace ocudu::schedtrace::test_helper;

/// Reads all size-prefixed event records of a schedtrace file, verifying each record.
static std::vector<std::vector<uint8_t>> read_cell_events(const std::filesystem::path& dir, du_cell_index_t cell_idx)
{
  std::ifstream f(dir / fmt::format("schedtrace_cell{}.bin", fmt::underlying(cell_idx)), std::ios::binary);
  EXPECT_TRUE(f.is_open());

  std::vector<std::vector<uint8_t>> events;
  while (true) {
    uint32_t size = 0;
    f.read(reinterpret_cast<char*>(&size), sizeof(size));
    if (f.eof()) {
      break;
    }
    EXPECT_TRUE(f.good());

    // Store the whole size-prefixed record so it can be decoded with GetSizePrefixedCellEvent().
    std::vector<uint8_t> record(sizeof(size) + size);
    std::memcpy(record.data(), &size, sizeof(size));
    f.read(reinterpret_cast<char*>(record.data() + sizeof(size)), size);
    EXPECT_TRUE(f.good());

    flatbuffers::Verifier verifier(record.data(), record.size());
    EXPECT_TRUE(fbs::VerifySizePrefixedCellEventBuffer(verifier));
    events.push_back(std::move(record));
  }
  return events;
}

/// Decodes a size-prefixed record read back from a file.
static const fbs::CellEvent& decode(const std::vector<uint8_t>& record)
{
  return *fbs::GetSizePrefixedCellEvent(record.data());
}

class sched_event_tracer_file_test : public ::testing::Test
{
protected:
  static constexpr std::chrono::milliseconds flush_period{10};

  std::filesystem::path temp_dir; // set in SetUp to be unique per test
  timer_manager         timers{8};
  manual_task_worker    worker{32};

  void SetUp() override
  {
    const auto* test_info = ::testing::UnitTest::GetInstance()->current_test_info();
    const auto  ts        = std::chrono::steady_clock::now().time_since_epoch().count();
    temp_dir = std::filesystem::temp_directory_path() / fmt::format("schedtrace_{}_{}", test_info->name(), ts);
    std::filesystem::create_directories(temp_dir);
    init_tracer(temp_dir.string(), flush_period, timers, worker);
  }

  void TearDown() override
  {
    worker.run_pending_tasks();
    schedtrace::close_tracer();
    std::filesystem::remove_all(temp_dir);
  }

  void tick_until_flush()
  {
    for (unsigned i = 0; i != flush_period.count(); ++i) {
      timers.tick();
    }
    worker.run_pending_tasks();
  }

  /// Destroy the tracer (pushes STOP) and tick until the STOP is processed, flushed, and the
  /// file is closed. After this call the .bin file is safe to read.
  void destroy_and_flush(std::unique_ptr<cell_event_tracer>& tracer)
  {
    tracer.reset();
    tick_until_flush();
  }
};

TEST_F(sched_event_tracer_file_test, when_slot_result_is_pushed_then_file_contains_slot_event)
{
  const auto& cell_cfg = make_test_cell_cfg(to_du_cell_index(0));
  auto        tracer   = schedtrace::create_cell_tracer(cell_cfg);

  const slot_point sl_tx{0, 5};
  sched_result     result;
  result.success = true;
  tracer->on_scheduler_result(sl_tx, result, std::chrono::microseconds(0));

  tick_until_flush();
  destroy_and_flush(tracer);

  const auto events = read_cell_events(temp_dir, to_du_cell_index(0));
  ASSERT_GE(events.size(), 3U);
  ASSERT_EQ(decode(events.front()).value_type(), fbs::CellEventValue::CellStartEvent);
  check_bwp_cfg(decode(events.front()).value_as_CellStartEvent()->init_ul_bwp(), cell_cfg.init_bwp.ul.cfg());
  check_bwp_cfg(decode(events.front()).value_as_CellStartEvent()->init_dl_bwp(), cell_cfg.init_bwp.dl.cfg());
  ASSERT_EQ(decode(events[1]).value_type(), fbs::CellEventValue::CellSlotEvent);
  EXPECT_EQ(decode(events[1]).value_as_CellSlotEvent()->slot_tx(), sl_tx.count());
  EXPECT_EQ(decode(events.back()).value_type(), fbs::CellEventValue::CellStopEvent);
}

TEST_F(sched_event_tracer_file_test, when_multiple_slot_results_are_pushed_then_all_appear_in_file_in_order)
{
  const auto& cell_cfg = make_test_cell_cfg(to_du_cell_index(0));
  auto        tracer   = schedtrace::create_cell_tracer(cell_cfg);

  sched_result result;
  result.success = true;
  tracer->on_scheduler_result(slot_point{0, 1}, result, std::chrono::microseconds(0));
  tracer->on_scheduler_result(slot_point{0, 2}, result, std::chrono::microseconds(0));
  tracer->on_scheduler_result(slot_point{0, 3}, result, std::chrono::microseconds(0));

  tick_until_flush();
  destroy_and_flush(tracer);

  const auto events = read_cell_events(temp_dir, to_du_cell_index(0));
  ASSERT_EQ(events.size(), 5U); // 1 START + 3 slot events + 1 STOP
  ASSERT_EQ(decode(events[0]).value_type(), fbs::CellEventValue::CellStartEvent);
  check_bwp_cfg(decode(events[0]).value_as_CellStartEvent()->init_ul_bwp(), cell_cfg.init_bwp.ul.cfg());
  check_bwp_cfg(decode(events[0]).value_as_CellStartEvent()->init_dl_bwp(), cell_cfg.init_bwp.dl.cfg());
  for (unsigned i = 1; i != 4; ++i) {
    ASSERT_EQ(decode(events[i]).value_type(), fbs::CellEventValue::CellSlotEvent);
    EXPECT_EQ(decode(events[i]).value_as_CellSlotEvent()->slot_tx(), slot_point(0, i).count());
  }
  EXPECT_EQ(decode(events[4]).value_type(), fbs::CellEventValue::CellStopEvent);
}

TEST_F(sched_event_tracer_file_test, when_multiple_cells_are_traced_then_separate_files_are_created)
{
  const auto& cell_cfg0 = make_test_cell_cfg(to_du_cell_index(0));
  const auto& cell_cfg1 = make_test_cell_cfg(to_du_cell_index(1));
  auto        tracer0   = schedtrace::create_cell_tracer(cell_cfg0);
  auto        tracer1   = schedtrace::create_cell_tracer(cell_cfg1);

  sched_result result;
  result.success = true;
  tracer0->on_scheduler_result(slot_point{0, 10}, result, std::chrono::microseconds(0));
  tracer1->on_scheduler_result(slot_point{0, 20}, result, std::chrono::microseconds(0));

  tick_until_flush();
  destroy_and_flush(tracer0);
  destroy_and_flush(tracer1);

  const auto events0 = read_cell_events(temp_dir, to_du_cell_index(0));
  ASSERT_GE(events0.size(), 3U);
  ASSERT_EQ(decode(events0[0]).value_type(), fbs::CellEventValue::CellStartEvent);
  ASSERT_EQ(decode(events0[0]).value_as_CellStartEvent()->pci(), 0U);
  check_bwp_cfg(decode(events0[0]).value_as_CellStartEvent()->init_ul_bwp(), cell_cfg0.init_bwp.ul.cfg());
  check_bwp_cfg(decode(events0[0]).value_as_CellStartEvent()->init_dl_bwp(), cell_cfg0.init_bwp.dl.cfg());
  ASSERT_EQ(decode(events0[1]).value_type(), fbs::CellEventValue::CellSlotEvent);
  ASSERT_EQ(decode(events0[1]).value_as_CellSlotEvent()->slot_tx(), slot_point(0, 10).count());
  ASSERT_EQ(decode(events0[2]).value_type(), fbs::CellEventValue::CellStopEvent);

  const auto events1 = read_cell_events(temp_dir, to_du_cell_index(1));
  ASSERT_GE(events1.size(), 3U);
  ASSERT_EQ(decode(events1[0]).value_type(), fbs::CellEventValue::CellStartEvent);
  ASSERT_EQ(decode(events1[0]).value_as_CellStartEvent()->pci(), 1U);
  check_bwp_cfg(decode(events1[0]).value_as_CellStartEvent()->init_ul_bwp(), cell_cfg1.init_bwp.ul.cfg());
  check_bwp_cfg(decode(events1[0]).value_as_CellStartEvent()->init_dl_bwp(), cell_cfg1.init_bwp.dl.cfg());
  ASSERT_EQ(decode(events1[1]).value_type(), fbs::CellEventValue::CellSlotEvent);
  EXPECT_EQ(decode(events1[1]).value_as_CellSlotEvent()->slot_tx(), slot_point(0, 20).count());
  ASSERT_EQ(decode(events1[2]).value_type(), fbs::CellEventValue::CellStopEvent);
}
