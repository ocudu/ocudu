// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "trace_to_log.h"
#include "../logging/scheduler_event_logger.h"
#include "../logging/scheduler_result_logger.h"
#include "event_converter.h"
#include "fbs/cell_event_generated.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/ran/bwp/bwp_configuration.h"
#include "ocudu/scheduler/result/sched_result.h"
#include "ocudu/scheduler/scheduler_rach_handler.h"
#include <vector>

using namespace ocudu;
using namespace schedtrace;

namespace {

/// Outcome of reading one record from the input stream.
enum class read_status { ok, clean_eof, truncated, corrupted };

} // namespace

/// Sanity cap on the record size, to bail out early on garbage input.
static constexpr uint32_t max_record_size = 1024 * 1024;

/// Reads the next size-prefixed event record into \p record (size prefix included) and verifies it.
static read_status read_record(std::istream& input, std::vector<uint8_t>& record)
{
  uint32_t size = 0;
  input.read(reinterpret_cast<char*>(&size), sizeof(size));
  if (input.gcount() == 0 and input.eof()) {
    return read_status::clean_eof;
  }
  if (input.gcount() != sizeof(size)) {
    return read_status::truncated;
  }
  if (size > max_record_size) {
    return read_status::corrupted;
  }

  // Copy the whole size-prefixed record into an aligned buffer; never decode file contents in place.
  record.resize(sizeof(size) + size);
  std::memcpy(record.data(), &size, sizeof(size));
  input.read(reinterpret_cast<char*>(record.data() + sizeof(size)), size);
  if (static_cast<uint32_t>(input.gcount()) != size) {
    return read_status::truncated;
  }

  flatbuffers::Verifier verifier(record.data(), record.size());
  if (not fbs::VerifySizePrefixedCellEventBuffer(verifier)) {
    return read_status::corrupted;
  }
  return read_status::ok;
}

bool schedtrace::trace_to_log(std::istream& input, ocudulog::basic_logger& logger)
{
  // Read first event.
  std::vector<uint8_t> record;
  switch (read_record(input, record)) {
    case read_status::clean_eof:
      fmt::print("Warning: Schedtrace file is empty\n");
      return true;
    case read_status::truncated:
    case read_status::corrupted:
      fmt::print(stderr, "ERROR: Schedtrace file is corrupted\n");
      return false;
    case read_status::ok:
      break;
  }

  // First object must be a CellStartEvent.
  const fbs::CellEvent* ev = fbs::GetSizePrefixedCellEvent(record.data());
  if (ev->value_type() != fbs::CellEventValue::CellStartEvent) {
    fmt::print(stderr, "ERROR: Invalid schedtrace file format. Cause: CellStartEvent was not first event\n");
    return false;
  }

  cell_configuration cell_cfg;
  convert_fb_to_cell_cfg(cell_cfg, *ev->value_as_CellStartEvent());
  const auto dl_scs = cell_cfg.init_dl_bwp.scs;
  logger.info("pci={}: Cell started. dl_bwp=[scs={} crbs=[{},{})] ul_bwp=[scs={} crbs=[{},{})]",
              cell_cfg.cell_index,
              to_string(cell_cfg.init_dl_bwp.scs),
              cell_cfg.init_dl_bwp.crbs.start(),
              cell_cfg.init_dl_bwp.crbs.stop(),
              to_string(cell_cfg.init_ul_bwp.scs),
              cell_cfg.init_ul_bwp.crbs.start(),
              cell_cfg.init_ul_bwp.crbs.stop());

  // Constructed after CellStartEvent so the PCI is known.
  scheduler_result_logger result_logger{true, cell_cfg.cell_index};
  scheduler_event_logger  event_logger{to_du_cell_index(0), cell_cfg.cell_index};
  const unsigned          flush_period    = 1024;
  unsigned                rem_until_flush = flush_period;
  bool                    stop_received   = false;
  read_status             status;
  while ((status = read_record(input, record)) == read_status::ok) {
    ev = fbs::GetSizePrefixedCellEvent(record.data());
    switch (ev->value_type()) {
      case fbs::CellEventValue::CellStartEvent: {
        fmt::print(stderr, "Warning: Invalid schedtrace file format. Cause: CellStartEvent was duplicate\n");
      } break;
      case fbs::CellEventValue::CellSlotEvent: {
        const fbs::CellSlotEvent& slot_ev = *ev->value_as_CellSlotEvent();
        sched_result              result;
        result.success = true;
        if (slot_ev.decision() != nullptr) {
          convert_fb_to_decision(result, *slot_ev.decision(), cell_cfg);
        }
        slot_point sl_tx = {dl_scs, slot_ev.slot_tx()};
        logger.set_context(sl_tx.sfn(), sl_tx.slot_index());
        const unsigned nof_inputs = slot_ev.inputs() != nullptr ? slot_ev.inputs()->size() : 0;
        for (unsigned i = 0; i != nof_inputs; ++i) {
          switch (slot_ev.inputs_type()->Get(i)) {
            case fbs::SlotInput::RachIndication: {
              const auto*             rach_ind = static_cast<const fbs::RachIndication*>(slot_ev.inputs()->Get(i));
              rach_indication_message msg;
              convert_fb_to_rach_indication(msg, *rach_ind, cell_cfg.init_ul_bwp.scs, cell_cfg.cell_index);
              event_logger.enqueue(msg);
            } break;
            case fbs::SlotInput::HarqAckEvent: {
              const auto*    harq_ack = static_cast<const fbs::HarqAckEvent*>(slot_ev.inputs()->Get(i));
              harq_ack_event event;
              convert_fb_to_harq_ack_event(event, *harq_ack, cell_cfg.init_ul_bwp.scs, cell_cfg.cell_index);
              event_logger.enqueue(event);
            } break;
            case fbs::SlotInput::SrEvent: {
              const auto* sr_ev = static_cast<const fbs::SrEvent*>(slot_ev.inputs()->Get(i));
              sr_event    event;
              convert_fb_to_sr_event(event, *sr_ev);
              event_logger.enqueue(event);
            } break;
            case fbs::SlotInput::CsiReportEvent: {
              const auto*      csi_report = static_cast<const fbs::CsiReportEvent*>(slot_ev.inputs()->Get(i));
              csi_report_event event;
              convert_fb_to_csi_report_event(event, *csi_report, cell_cfg.init_ul_bwp.scs);
              event_logger.enqueue(event);
            } break;
            default:
              fmt::print(stderr, "ERROR: Unsupported schedtrace slot input");
              return false;
          }
        }
        event_logger.log();
        result_logger.on_scheduler_result(result, std::chrono::microseconds(slot_ev.latency_us()));
      } break;
      case fbs::CellEventValue::CellStopEvent: {
        logger.info("pci={}: Cell stopped", cell_cfg.cell_index);
        stop_received = true;
      } break;
      default:
        fmt::print(stderr, "ERROR: Unsupported schedtrace event type\n");
        return false;
    }
    if (stop_received) {
      break;
    }

    // Flush log periodically to not lose entries.
    if (--rem_until_flush == 0) {
      ocudulog::flush();
      rem_until_flush = flush_period;
    }
  }

  if (not stop_received && status != read_status::clean_eof) {
    fmt::print(stderr, "Warning: Invalid schedtrace file ending. Is it truncated?\n");
    return false;
  }

  return true;
}
