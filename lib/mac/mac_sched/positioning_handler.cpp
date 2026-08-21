// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "positioning_handler.h"
#include "../mac_ctrl/spsc_metric_report_channel.h"
#include "ocudu/adt/format.h"
#include "ocudu/adt/slotted_array.h"
#include "ocudu/mac/mac_positioning_measurement_handler.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/scheduler/scheduler_positioning_handler.h"
#include "ocudu/support/async/async_no_op_task.h"
#include "ocudu/support/async/manual_event.h"
#include "ocudu/support/executors/execute_until_success.h"
#include "ocudu/support/executors/task_executor.h"

using namespace ocudu;

namespace {

/// MAC scheduler positioning handler.
class pos_handler_impl final : public positioning_handler
{
public:
  class cell_positioning_handler_impl final : public cell_positioning_handler
  {
  public:
    cell_positioning_handler_impl(du_cell_index_t cell_index_, pos_handler_impl& parent_) :
      cell_index(cell_index_), parent(parent_)
    {
    }
    ~cell_positioning_handler_impl() override { parent.rem_cell(cell_index); }

    void handle_srs_indication(const mac_srs_indication_message& msg) override
    {
      parent.handle_srs_indication(cell_index, msg);
    }

  private:
    du_cell_index_t   cell_index;
    pos_handler_impl& parent;
  };

  pos_handler_impl(scheduler_positioning_handler& sched_,
                   task_executor&                 ctrl_exec_,
                   timer_manager&                 timers_,
                   ocudulog::basic_logger&        logger_) :
    sched(sched_), ctrl_exec(ctrl_exec_), timers(timers_), logger(logger_)
  {
  }

  async_task<mac_positioning_measurement_response>
  handle_positioning_measurement_request(const mac_positioning_measurement_request& req) override;

  std::unique_ptr<cell_positioning_handler> add_cell(du_cell_index_t cell_index) override
  {
    ocudu_assert(not cells.contains(cell_index), "Cell already exists");
    cells.emplace(cell_index, *this, cell_index);
    return std::make_unique<cell_positioning_handler_impl>(cell_index, *this);
  }

private:
  using report_queue_type = spsc_metric_report_channel<mac_srs_indication_message>;

  struct cell_meas_context {
    static constexpr size_t MAX_HISTORY_SIZE = 4;

    const du_cell_index_t cell_index;
    rnti_t                pos_rnti = rnti_t::INVALID_RNTI;
    /// Channel used to transfer reports from lower layer execution context to ctrl execution context.
    report_queue_type channel;
    /// Buffer of past positioning measurements.
    std::vector<mac_srs_indication_message> report_history;
    // List of free RNTIs that can be used for positioning measurement for UEs that are not connected to the cell. This
    // list is only accessed by the CTRL executor context.
    std::vector<rnti_t> free_dummy_rnti_list;

    cell_meas_context(pos_handler_impl& parent, du_cell_index_t cell_index_);
  };

  void handle_srs_indication(du_cell_index_t cell_index, const mac_srs_indication_message& msg);

  void handle_pending_srs_reports();

  void handle_pending_srs_report(du_cell_index_t cell_index, mac_srs_indication_message& msg);

  void clean_last_req_resources();

  void rem_cell(du_cell_index_t cell_index);

  scheduler_positioning_handler& sched;
  task_executor&                 ctrl_exec;
  timer_manager&                 timers;
  ocudulog::basic_logger&        logger;

  mac_positioning_measurement_request cur_req;

  slotted_id_table<du_cell_index_t, cell_meas_context, MAX_NOF_DU_CELLS> cells;

  std::atomic<unsigned> job_count{0};

  mac_positioning_measurement_response resp;
  manual_event_flag                    report_completed;
};

pos_handler_impl::cell_meas_context::cell_meas_context(pos_handler_impl& parent, du_cell_index_t cell_index_) :
  cell_index(cell_index_), channel(cell_meas_context::MAX_HISTORY_SIZE, parent.logger), report_history(MAX_HISTORY_SIZE)
{
  // Fill free list of dummy RNTIs.
  for (rnti_t reserved_rnti = rnti_t::MIN_RESERVED_RNTI; reserved_rnti <= rnti_t::MAX_RESERVED_RNTI;
       reserved_rnti        = to_rnti((unsigned)reserved_rnti + 1U)) {
    free_dummy_rnti_list.push_back(reserved_rnti);
  }
}

async_task<mac_positioning_measurement_response>
pos_handler_impl::handle_positioning_measurement_request(const mac_positioning_measurement_request& req)
{
  if (not cur_req.cells.empty()) {
    logger.warning("Received new positioning measurement request while another one is still ongoing");
    return launch_no_op_task(mac_positioning_measurement_response{});
  }

  // Save the current request.
  cur_req = req;

  // Prepare buffers for each cell and allocate POS-RNTI, if required.
  for (const auto& cell_req : req.cells) {
    auto& c = cells[cell_req.cell_index];
    // Drain any previous stray reports.
    while (c.channel.pop()) {
    }

    // Allocate a POS-RNTI for this measurement request.
    if (cell_req.rnti.has_value()) {
      // It is a UE that belongs to the cell.
      c.pos_rnti = *cell_req.rnti;
    } else {
      // It is a measurement for a UE that does not belong to this cell.
      // Use reserved RNTIs for this situation.
      if (c.free_dummy_rnti_list.empty()) {
        logger.warning("cell={}: Received positioning measurement request for a neighbor UE, but no more reserved RNTIs"
                       " are available in the cell",
                       cell_req.cell_index);
        // Cleanup previously allocated pos-RNTIs.
        clean_last_req_resources();
        return launch_no_op_task(mac_positioning_measurement_response{});
      }
      c.pos_rnti = c.free_dummy_rnti_list.back();
      c.free_dummy_rnti_list.pop_back();
    }
  }

  // Command scheduler to start reporting positioning measurements.
  positioning_measurement_request sched_req;
  for (const auto& cell_req : req.cells) {
    auto& sched_cell          = sched_req.cells.emplace_back();
    sched_cell.cell_index     = cell_req.cell_index;
    sched_cell.ue_index       = cell_req.ue_index;
    sched_cell.pos_rnti       = cells[cell_req.cell_index].pos_rnti;
    sched_cell.srs_to_measure = cell_req.srs_to_meas;
  }
  sched.handle_positioning_measurement_request(sched_req);

  return launch_async([this](coro_context<async_task<mac_positioning_measurement_response>>& ctx) {
    CORO_BEGIN(ctx);

    // Await for the completion of a positioning measurement report.
    CORO_AWAIT(report_completed);
    report_completed.reset();

    // Return the measurement report.
    CORO_RETURN(resp);
  });
}

void pos_handler_impl::handle_srs_indication(du_cell_index_t cell_index, const mac_srs_indication_message& msg)
{
  // Verify that there are positioning PDUs in this indication. If not, ignore it.
  const mac_srs_pdu* pdu_ptr = nullptr;
  for (const auto& pdu : msg.srss) {
    const auto* pos_rep = std::get_if<mac_srs_pdu::positioning_report>(&pdu.report);
    if (pos_rep == nullptr) {
      // Not a positioning SRS report.
      continue;
    }
    if (not pos_rep->ul_rtoa.has_value() and not pos_rep->azimuth_aoa.has_value()) {
      // Ignore positioning reports that carry none of the currently supported measurement types.
      logger.warning("rnti={} processing positioning SRS indication with no RTOA nor AoA", pdu.rnti);
      continue;
    }
    if (std::any_of(cells.begin(), cells.end(), [&](const auto& c) { return c.pos_rnti == pdu.rnti; })) {
      // This PDU is for a POS-RNTI being currently measured.
      pdu_ptr = &pdu;
      break;
    }
  }
  if (pdu_ptr == nullptr) {
    // No positioning PDUs in this indication.
    return;
  }

  // Copy the SRS PDUs for positioning to the report channel.
  auto rep = cells[cell_index].channel.get_builder();
  rep->srss.clear();
  rep->sl_rx = msg.sl_rx;
  rep->srss.push_back(*pdu_ptr);
  // Resetting pushes the report to the channel.
  rep.reset();

  // Schedule the processing of pending SRS reports in the ctrl executor, if not already scheduled.
  bool token_acquired = job_count.fetch_add(1, std::memory_order_acq_rel) == 0;
  if (not token_acquired) {
    return;
  }
  defer_until_success(ctrl_exec, timers, [this]() { handle_pending_srs_reports(); });
}

void pos_handler_impl::handle_pending_srs_reports()
{
  auto count = job_count.load(std::memory_order_acquire);
  while (count != 0) {
    bool handled_any = false;
    for (auto& c : cells) {
      auto rep = c.channel.pop();
      if (rep) {
        // Handle the popped SRS report.
        handle_pending_srs_report(c.cell_index, *rep);
        handled_any = true;
        // Decrement the number of pending jobs to handle.
        count = job_count.fetch_sub(1, std::memory_order_acq_rel) - 1;
        if (count == 0) {
          // No more jobs to handle.
          return;
        }
      }
    }
    if (not handled_any) {
      // Unexpected situation: job count is non-zero, but no reports to handle.
      logger.error("Positioning handler job count is inconsistent with pending reports");
      job_count.fetch_sub(count, std::memory_order_acq_rel);
      return;
    }
  }
}

void pos_handler_impl::handle_pending_srs_report(du_cell_index_t cell_index, mac_srs_indication_message& msg)
{
  if (cur_req.cells.empty()) {
    // The previous positioning measurement request has been cleaned up meanwhile.
    // We are likely seeing a stray SRS indication.
    return;
  }

  // Store the report in the history buffer.
  auto&          c           = cells[cell_index];
  const unsigned ring_idx    = msg.sl_rx.count() % c.report_history.size();
  c.report_history[ring_idx] = msg;

  // Check if all cells have reported for this slot.
  for (auto& cell_req : cur_req.cells) {
    if (cells[cell_req.cell_index].report_history[ring_idx].sl_rx != msg.sl_rx) {
      // Report from this cell is missing.
      return;
    }
  }

  // All cells have reported for this slot. Generate response.
  resp.sl_rx = msg.sl_rx;
  resp.cell_results.resize(cur_req.cells.size());
  for (unsigned i = 0; i != cur_req.cells.size(); ++i) {
    const auto& srss = cells[cur_req.cells[i].cell_index].report_history[ring_idx].srss;
    resp.cell_results[i].ul_srs_pos_meass.resize(srss.size());
    for (unsigned j = 0; j != srss.size(); ++j) {
      auto& rep           = std::get<mac_srs_pdu::positioning_report>(srss[j].report);
      auto& out           = resp.cell_results[i].ul_srs_pos_meass[j];
      out.ul_rtoa         = rep.ul_rtoa;
      out.rsrp_dbfs       = rep.ul_rsrp_dBFS;
      out.azimuth_aoa_deg = rep.azimuth_aoa;
      out.zenith_aoa_deg  = rep.zenith_aoa;
    }
  }

  // Notify scheduler to stop scheduling new positioning measurement PDUs.
  positioning_measurement_stop_request stop_req;
  for (const auto& cell_req : cur_req.cells) {
    stop_req.completed_meas.emplace_back(cell_req.cell_index, cells[cell_req.cell_index].pos_rnti);
  }
  sched.handle_positioning_measurement_stop(stop_req);

  // Cleanup resources used for this request.
  clean_last_req_resources();

  // Notify the completion of the positioning report.
  report_completed.set();
}

void pos_handler_impl::clean_last_req_resources()
{
  for (const auto& cell_req : cur_req.cells) {
    auto& c = cells[cell_req.cell_index];
    // Drain any previous pending results and clear history.
    while (c.channel.pop()) {
    }
    c.report_history.clear();
    c.report_history.resize(cell_meas_context::MAX_HISTORY_SIZE);

    // Deallocate any past pos-RNTI.
    if (c.pos_rnti >= rnti_t::MIN_RESERVED_RNTI and c.pos_rnti <= rnti_t::MAX_RESERVED_RNTI) {
      c.free_dummy_rnti_list.push_back(c.pos_rnti);
    }
    c.pos_rnti = rnti_t::INVALID_RNTI;
  }

  // Delete previous request.
  cur_req.cells.clear();
}

void pos_handler_impl::rem_cell(du_cell_index_t cell_index)
{
  ocudu_assert(cells.contains(cell_index), "Cell does not exist");
  cells.erase(cell_index);
}

} // namespace

std::unique_ptr<positioning_handler> ocudu::create_positioning_handler(scheduler_positioning_handler& sched,
                                                                       task_executor&                 ctrl_exec,
                                                                       timer_manager&                 timers,
                                                                       ocudulog::basic_logger&        logger)
{
  return std::make_unique<pos_handler_impl>(sched, ctrl_exec, timers, logger);
}
