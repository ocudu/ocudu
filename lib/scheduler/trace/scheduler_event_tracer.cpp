// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/instrumentation/traces/scheduler_event_tracer.h"
#include "event_trace_writer_registry.h"
#include "fbs/cell_event_generated.h"
#include "ocudu/support/executors/task_executor.h"
#include "ocudu/support/timers.h"
#include <chrono>
#include <fmt/format.h>
#include <fstream>

using namespace ocudu;
using namespace schedtrace;

namespace {

/// Event trace writer that writes trace events into a file.
class file_trace_writer final : public event_trace_writer
{
public:
  file_trace_writer(const std::string& base_path, du_cell_index_t cell_idx) :
    cell_index(cell_idx),
    fptr(fmt::format("{}/schedtrace_cell{}.bin", base_path, fmt::underlying(cell_idx)),
         std::ios::binary | std::ios::out | std::ios::trunc)
  {
    report_fatal_error_if_not(
        fptr.is_open(), "Failed to open scheduler cell tracer with path {} for cell {}", base_path, cell_index);
  }

  ~file_trace_writer() override
  {
    if (fptr.is_open()) {
      fptr.flush();
      fptr.close();
    }
  }

  bool on_flush_triggered(cell_event_channel& ev_queue) override
  {
    bool stop_received = false;
    while (ev_queue.consume([this, &stop_received](span<const uint8_t> popped_ev) {
      write_event(popped_ev);
      // Note: The builder buffer is aligned, so decoding the event in place is safe.
      if (fbs::GetSizePrefixedCellEvent(popped_ev.data())->value_type() == fbs::CellEventValue::CellStopEvent) {
        fptr.flush();
        stop_received = true;
      }
    })) {
      if (stop_received) {
        // STOP command was received. Stop popping tasks and signal the close to the caller via the return.
        break;
      }
    }
    return not stop_received;
  }

private:
  void write_event(span<const uint8_t> ev)
  {
    // Note: The event bytes are already size-prefixed by the producer.
    fptr.write(reinterpret_cast<const char*>(ev.data()), static_cast<std::streamsize>(ev.size()));
  }

  const du_cell_index_t cell_index;
  std::ofstream         fptr;
};

} // namespace

/// Single cell event file writer registry.
static std::unique_ptr<event_trace_writer_registry> registry;

void schedtrace::init_tracer(const std::string&        dir_path,
                             std::chrono::milliseconds flush_period,
                             timer_manager&            timers,
                             task_executor&            pool_executor)
{
  report_fatal_error_if_not(registry == nullptr, "Scheduler trace handling registry has already been initialized");
  registry = std::make_unique<event_trace_writer_registry>(
      flush_period, timers, pool_executor, [dir_path](du_cell_index_t cell_idx) {
        return std::make_unique<file_trace_writer>(dir_path, cell_idx);
      });
}

std::unique_ptr<cell_event_tracer> schedtrace::create_cell_tracer(const ocudu::cell_configuration& cell_cfg)
{
  if (registry == nullptr) {
    // Create disabled producer.
    return std::make_unique<cell_event_tracer>();
  }

  return registry->create_cell_tracer(cell_cfg);
}

void schedtrace::close_tracer()
{
  registry = nullptr;
}
