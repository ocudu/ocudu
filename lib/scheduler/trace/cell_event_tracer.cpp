// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "../logging/cell_event_tracer.h"
#include "../config/cell_configuration.h"
#include "cell_event_channel.h"
#include "event_converter.h"
#include "fbs/cell_event_generated.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/scheduler/input/uci_inputs.h"
#include "ocudu/scheduler/result/sched_result.h"

using namespace ocudu::schedtrace;

/// Returns true if nothing was scheduled and no allocation attempt failed this slot, i.e. encoding a \c SlotDecision
/// would produce an all-empty table.
static bool is_decision_empty(const ocudu::sched_result& result)
{
  return result.dl.dl_pdcchs.empty() && result.dl.ul_pdcchs.empty() && result.dl.bc.sibs.empty() &&
         result.dl.rar_grants.empty() && result.dl.paging_grants.empty() && result.dl.ue_grants.empty() &&
         result.dl.csi_rs.empty() && result.dl.bc.ssb_info.empty() && result.ul.puschs.empty() &&
         result.ul.pucchs.empty() && result.ul.prachs.empty() && result.ul.srss.empty() &&
         result.failed_attempts.dl_pdcch == 0 && result.failed_attempts.ul_pdcch == 0 &&
         result.failed_attempts.common_dl_pdcch == 0 && result.failed_attempts.common_ul_pdcch == 0 &&
         result.failed_attempts.uci == 0;
}

/// Finishes the size-prefixed CellEvent buffer wrapping the given union member, stamping the current time.
static void
finish_cell_event(flatbuffers::FlatBufferBuilder& fbb, fbs::CellEventValue type, flatbuffers::Offset<void> value)
{
  const int64_t timestamp_us =
      std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch())
          .count();
  fbs::FinishSizePrefixedCellEventBuffer(fbb, fbs::CreateCellEvent(fbb, timestamp_us, type, value));
}

cell_event_tracer::cell_event_tracer() :
  cell_index(INVALID_DU_CELL_INDEX), logger(ocudulog::fetch_basic_logger("SCHED"))
{
}

cell_event_tracer::cell_event_tracer(const ocudu::cell_configuration& cell_cfg, cell_event_channel& ev_queue_) :
  cell_index(cell_cfg.cell_index), ev_queue(&ev_queue_), logger(ocudulog::fetch_basic_logger("SCHED"))
{
  // Push the start event.
  on_cell_start(cell_cfg);
}

cell_event_tracer::~cell_event_tracer()
{
  if (ev_queue == nullptr) {
    return;
  }

  flatbuffers::FlatBufferBuilder& fbb = ev_queue->next();
  finish_cell_event(fbb,
                    fbs::CellEventValue::CellStopEvent,
                    fbs::CreateCellStopEvent(fbb, static_cast<uint16_t>(cell_index)).Union());
  ev_queue->force_commit();
}

void cell_event_tracer::on_cell_start(const ocudu::cell_configuration& cell_cfg)
{
  flatbuffers::FlatBufferBuilder& fbb = ev_queue->next();
  finish_cell_event(fbb, fbs::CellEventValue::CellStartEvent, convert_cell_cfg_to_fb(fbb, cell_cfg).Union());

  // Cell start event should never fail to be pushed, because it is the first one.
  bool success = ev_queue->commit();
  report_fatal_error_if_not(success, "Failed to push cell start event to tracer");
}

void cell_event_tracer::on_scheduler_result_impl(slot_point                sl,
                                                 const sched_result&       result,
                                                 std::chrono::microseconds slot_latency)
{
  // Fill sched decision in slot event. The buffered inputs were already built into this builder.
  flatbuffers::FlatBufferBuilder& fbb = ev_queue->next();
  const auto                      decision =
      is_decision_empty(result) ? flatbuffers::Offset<fbs::SlotDecision>() : convert_decision_to_fb(fbb, result);

  const auto pending_types  = ev_queue->pending_input_types();
  const auto pending_values = ev_queue->pending_input_values();
  const auto input_types    = pending_types.empty() ? 0 : fbb.CreateVector(pending_types.data(), pending_types.size());
  const auto input_values = pending_values.empty() ? 0 : fbb.CreateVector(pending_values.data(), pending_values.size());

  const auto slot_ev =
      fbs::CreateCellSlotEvent(fbb, sl.count(), slot_latency.count(), input_types, input_values, decision);
  finish_cell_event(fbb, fbs::CellEventValue::CellSlotEvent, slot_ev.Union());

  // Push the completed slot to the backend.
  if (not ev_queue->commit()) {
    logger.warning(
        "cell={} slot={}: Failed to push cell slot trace event. Cause: Event queue is full.", cell_index, sl);
  }
}

void cell_event_tracer::on_event_impl(const rach_indication_message& msg)
{
  ev_queue->add_input(fbs::SlotInput::RachIndication, convert_rach_indication_to_fb(ev_queue->next(), msg).Union());
}

void cell_event_tracer::on_event_impl(const harq_ack_event& ev)
{
  ev_queue->add_input(fbs::SlotInput::HarqAckEvent, convert_harq_ack_event_to_fb(ev_queue->next(), ev).Union());
}

void cell_event_tracer::on_event_impl(const sr_event& ev)
{
  ev_queue->add_input(fbs::SlotInput::SrEvent, convert_sr_event_to_fb(ev_queue->next(), ev).Union());
}

void cell_event_tracer::on_event_impl(const csi_report_event& ev)
{
  ev_queue->add_input(fbs::SlotInput::CsiReportEvent, convert_csi_report_event_to_fb(ev_queue->next(), ev).Union());
}
