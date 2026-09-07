// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "fbs/cell_event_generated.h"
#include "lib/scheduler/logging/scheduler_result_logger.h"
#include "lib/scheduler/trace/event_converter.h"
#include "lib/scheduler/trace/trace_to_log.h"
#include "roundtrip/full_sched_result.h"
#include "tests/test_doubles/scheduler/trace/test_messages.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/ocudulog/sink.h"
#include "ocudu/scheduler/result/sched_result.h"
#include <algorithm>
#include <gtest/gtest.h>
#include <sstream>
#include <string>
#include <vector>

using namespace ocudu;
using namespace schedtrace;
using namespace test_helper;
using namespace schedtrace::roundtrip_test;

namespace {

/// Log sink that captures all formatted messages into a vector of strings.
class capturing_sink : public ocudulog::sink
{
public:
  capturing_sink() : ocudulog::sink(ocudulog::create_text_formatter()) {}

  ocudulog::detail::error_string write(ocudulog::detail::memory_buffer buffer) override
  {
    messages_.emplace_back(buffer.begin(), buffer.end());
    return {};
  }

  ocudulog::detail::error_string flush() override { return {}; }

  const std::vector<std::string>& messages() const { return messages_; }

  void clear() { messages_.clear(); }

private:
  std::vector<std::string> messages_;
};

/// Builds a stream of size-prefixed CellEvent flatbuffers in memory, suitable for feeding to trace_to_log.
///
/// Appends a CellStartEvent at construction and a CellStopEvent at destruction.
/// Use handle_slot() to append CellSlotEvents between start and stop.
/// Retrieve the complete stream (including the stop event) via get_stream().
class cell_trace_stream_builder
{
public:
  cell_trace_stream_builder(uint16_t pci_, const bwp_configuration& dl_bwp, const bwp_configuration& ul_bwp) : pci(pci_)
  {
    flatbuffers::FlatBufferBuilder fbb;
    // Serialize the standard test PUCCH resource list so that scheduled PUCCHs can be resolved on replay.
    std::vector<flatbuffers::Offset<fbs::PucchResource>> res_offs;
    for (const auto& res : make_test_schedtrace_cell_cfg().pucch_resources) {
      res_offs.push_back(convert_pucch_resource_to_fb(fbb, res));
    }
    const auto res_vec = fbb.CreateVector(res_offs);
    const auto ul      = convert_bwp_cfg_to_fb(ul_bwp);
    const auto dl      = convert_bwp_cfg_to_fb(dl_bwp);
    const auto start   = fbs::CreateCellStartEvent(fbb, pci, &ul, &dl, res_vec);
    finish_and_append(fbb, fbs::CellEventValue::CellStartEvent, start.Union());
  }

  ~cell_trace_stream_builder()
  {
    if (not finalised) {
      write_stop();
    }
  }

  void handle_slot(slot_point sl, const sched_result& result, std::chrono::microseconds latency)
  {
    flatbuffers::FlatBufferBuilder fbb;
    const auto                     decision = convert_decision_to_fb(fbb, result);
    const auto                     slot_ev = fbs::CreateCellSlotEvent(fbb, sl.count(), latency.count(), 0, 0, decision);
    finish_and_append(fbb, fbs::CellEventValue::CellSlotEvent, slot_ev.Union());
  }

  /// Returns the complete serialized stream, including the CellStopEvent.
  std::string get_stream()
  {
    if (not finalised) {
      write_stop();
    }
    return buf.str();
  }

private:
  void write_stop()
  {
    flatbuffers::FlatBufferBuilder fbb;
    finish_and_append(fbb, fbs::CellEventValue::CellStopEvent, fbs::CreateCellStopEvent(fbb, pci).Union());
    finalised = true;
  }

  void finish_and_append(flatbuffers::FlatBufferBuilder& fbb, fbs::CellEventValue type, flatbuffers::Offset<void> value)
  {
    fbs::FinishSizePrefixedCellEventBuffer(fbb, fbs::CreateCellEvent(fbb, 0, type, value));
    buf.write(reinterpret_cast<const char*>(fbb.GetBufferPointer()), fbb.GetSize());
  }

  const uint16_t     pci;
  std::ostringstream buf;
  bool               finalised = false;
};

} // namespace

class trace_to_log_test : public ::testing::Test
{
protected:
  /// Single sink shared across all tests in this suite, so both the passed logger and the
  /// internally-fetched "SCHED" logger (used by scheduler_result_logger) write to the same place.
  static capturing_sink& shared_sink()
  {
    static capturing_sink instance;
    return instance;
  }

  static void SetUpTestSuite()
  {
    ocudulog::set_default_sink(shared_sink());
    ocudulog::init();
    auto& sched_logger = ocudulog::fetch_basic_logger("SCHED", shared_sink(), false);
    sched_logger.set_level(ocudulog::basic_levels::debug);
  }

  static std::string unique_logger_name()
  {
    static int counter = 0;
    return "trace_to_log_test_" + std::to_string(counter++);
  }

  ocudulog::basic_logger& logger;

  trace_to_log_test() : logger(ocudulog::fetch_basic_logger(unique_logger_name(), shared_sink(), false))
  {
    logger.set_level(ocudulog::basic_levels::debug);
    shared_sink().clear();
  }

  bool run(const std::string& stream_data)
  {
    std::istringstream ss(stream_data);
    const bool         result = trace_to_log(ss, logger);
    ocudulog::flush();
    return result;
  }

  static bool messages_contain(const std::string& substr)
  {
    return std::any_of(shared_sink().messages().begin(),
                       shared_sink().messages().end(),
                       [&substr](const std::string& m) { return m.find(substr) != std::string::npos; });
  }

  static void log_to_stdout()
  {
    fmt::print("Log Result:\n");
    for (const auto& msg : shared_sink().messages()) {
      fmt::print("{}", msg);
    }
  }
};

TEST_F(trace_to_log_test, when_stream_is_empty_then_returns_true_with_no_log_messages)
{
  std::istringstream ss{};
  ASSERT_TRUE(trace_to_log(ss, logger));
  ocudulog::flush();

  ASSERT_TRUE(shared_sink().messages().empty());
}

TEST_F(trace_to_log_test, when_cell_starts_and_stops_then_start_and_stop_are_logged)
{
  cell_trace_stream_builder builder{0, make_test_bwp_cfg(), make_test_bwp_cfg()};
  ASSERT_TRUE(run(builder.get_stream()));

  ASSERT_EQ(shared_sink().messages().size(), 2);
  ASSERT_TRUE(messages_contain("Cell started"));
  ASSERT_TRUE(messages_contain("Cell stopped"));
  log_to_stdout();
}

TEST_F(trace_to_log_test, when_cell_starts_then_log_contains_pci_and_bwp_info)
{
  // The CellStartEvent "pci" field carries the DU cell index, which has a small valid range.
  cell_trace_stream_builder builder{1, make_test_bwp_cfg(), make_test_bwp_cfg()};
  ASSERT_TRUE(run(builder.get_stream()));

  ASSERT_TRUE(messages_contain("pci=1"));
  ASSERT_TRUE(messages_contain("30kHz"));
  log_to_stdout();
}

TEST_F(trace_to_log_test, when_slot_results_are_in_stream_then_each_slot_produces_log_output)
{
  // Any non-empty decision will do; what is under test is that each slot in the stream reaches the logger.
  const sched_result result = make_full_sched_result();

  cell_trace_stream_builder builder{0, make_test_bwp_cfg(), make_test_bwp_cfg()};
  builder.handle_slot(slot_point{0, 1}, result, std::chrono::microseconds(10));
  builder.handle_slot(slot_point{0, 2}, result, std::chrono::microseconds(20));
  builder.handle_slot(slot_point{0, 3}, result, std::chrono::microseconds(30));
  ASSERT_TRUE(run(builder.get_stream()));
  log_to_stdout();

  // Start + one line per slot (2 for the first one) + stop.
  ASSERT_EQ(shared_sink().messages().size(), 6U);
  ASSERT_NE(shared_sink().messages()[2].find("t=10us"), std::string::npos);
  ASSERT_NE(shared_sink().messages()[3].find("t=20us"), std::string::npos);
  ASSERT_NE(shared_sink().messages()[4].find("t=30us"), std::string::npos);
}

TEST_F(trace_to_log_test, logged_slot_decisions_survive_the_trace_roundtrip)
{
  const sched_result result = make_full_sched_result();

  // Log the decision directly, through the same "SCHED" logger trace_to_log replays into.
  scheduler_result_logger direct{true, 0};
  direct.on_scheduler_result(result, std::chrono::microseconds(10));
  ocudulog::flush();
  ASSERT_EQ(shared_sink().messages().size(), 1U);
  const std::string expected = shared_sink().messages().front();
  shared_sink().clear();

  cell_trace_stream_builder builder{0, make_test_bwp_cfg(), make_test_bwp_cfg()};
  builder.handle_slot(slot_point{0, 1}, result, std::chrono::microseconds(10));
  ASSERT_TRUE(run(builder.get_stream()));

  const auto replayed =
      std::find_if(shared_sink().messages().begin(), shared_sink().messages().end(), [](const std::string& m) {
        return m.find("Slot decisions") != std::string::npos;
      });
  ASSERT_NE(replayed, shared_sink().messages().end());
  EXPECT_EQ(expected, *replayed);
}
