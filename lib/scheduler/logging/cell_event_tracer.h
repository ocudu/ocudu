// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ocudulog/logger.h"
#include "ocudu/ran/du_types.h"
#include "ocudu/ran/slot_point.h"
#include "ocudu/scheduler/input/uci_inputs.h"
#include "ocudu/support/rtsan.h"

namespace ocudu {

struct bwp_configuration;
struct rach_indication_message;
struct sched_result;

namespace schedtrace {

class cell_event_channel;

/// Class responsible for buffering all the events that occurred in a given slot for a given scheduler cell, and
/// forward them to the schedtrace backend.
class cell_event_tracer
{
public:
  cell_event_tracer();
  cell_event_tracer(du_cell_index_t          cell_idx,
                    cell_event_channel&      ev_queue,
                    const bwp_configuration& dl_bwp,
                    const bwp_configuration& ul_bwp);
  cell_event_tracer(const cell_event_tracer&) = delete;
  cell_event_tracer(cell_event_tracer&&)      = delete;
  ~cell_event_tracer();
  cell_event_tracer& operator=(const cell_event_tracer&) = delete;
  cell_event_tracer& operator=(cell_event_tracer&&)      = delete;

  template <typename... Args>
  void on_event(Args&&... args)
  {
    if (not enabled()) {
      return;
    }
    on_event_impl(std::forward<Args>(args)...);
  }

  /// Called on each produced slot result by the scheduler cell.
  void on_scheduler_result(slot_point                sl,
                           const sched_result&       result,
                           std::chrono::microseconds slot_latency) noexcept OCUDU_RTSAN_NONBLOCKING
  {
    if (not enabled()) {
      return;
    }
    on_scheduler_result_impl(sl, result, slot_latency);
  }

private:
  /// Determines whether cell_event_tracer is active.
  bool enabled() const { return ev_queue != nullptr; }

  /// Called when the cell starts.
  void on_cell_start(const bwp_configuration& dl_bwp, const bwp_configuration& ul_bwp);

  /// Called when the scheduler makes a decision for a given cell and slot.
  void on_scheduler_result_impl(slot_point sl, const sched_result& result, std::chrono::microseconds slot_latency);

  /// Called on every detected RACH indication.
  void on_event_impl(const rach_indication_message& msg);

  /// Called on every HARQ-ACK event.
  void on_event_impl(const harq_ack_event& ev);

  /// Called on every SR event.
  void on_event_impl(const sr_event& ev);

  /// Called on every CSI report event.
  void on_event_impl(const csi_report_event& ev);

  /// The DU-specific index of this cell.
  const du_cell_index_t cell_index;

  /// View to event queue (producer -> consumer) used to propagate events to the tracing backend.
  cell_event_channel* ev_queue = nullptr;

  ocudulog::basic_logger& logger;
};

/// \brief Create scheduler cell-specific trace producer.
/// \param cell_idx Index of the DU cell.
/// \param dl_bwp Initial DL BWP configuration of the cell.
/// \param ul_bwp Initial UL BWP configuration of the cell.
/// \return Created producer.
std::unique_ptr<cell_event_tracer>
create_cell_tracer(du_cell_index_t cell_idx, const bwp_configuration& dl_bwp, const bwp_configuration& ul_bwp);

} // namespace schedtrace
} // namespace ocudu
