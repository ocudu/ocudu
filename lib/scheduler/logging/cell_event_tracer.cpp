// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#ifndef OCUDU_HAS_SCHEDTRACE

#include "cell_event_tracer.h"
#include "../config/cell_configuration.h"
#include "ocudu/instrumentation/traces/scheduler_event_tracer.h"
#include "ocudu/ocudulog/ocudulog.h"

using namespace ocudu;
using namespace schedtrace;

cell_event_tracer::cell_event_tracer() :
  cell_index(INVALID_DU_CELL_INDEX), logger(ocudulog::fetch_basic_logger("SCHED"))
{
}

cell_event_tracer::cell_event_tracer(const ocudu::cell_configuration& cell_cfg, cell_event_channel& ev_queue_) :
  cell_index(cell_cfg.cell_index), ev_queue(&ev_queue_), logger(ocudulog::fetch_basic_logger("SCHED"))
{
  (void)cell_index;
}

cell_event_tracer::~cell_event_tracer() {}

void cell_event_tracer::on_cell_start(const ocudu::cell_configuration& cell_cfg) {}

void cell_event_tracer::on_scheduler_result_impl(slot_point                sl,
                                                 const sched_result&       result,
                                                 std::chrono::microseconds slot_latency)
{
}

void cell_event_tracer::on_event_impl(const rach_indication_message& /*unused*/) {}
void cell_event_tracer::on_event_impl(const harq_ack_event& /*unused*/) {}
void cell_event_tracer::on_event_impl(const csi_report_event& /*unused*/) {}
void cell_event_tracer::on_event_impl(const sr_event& /*unused*/) {}

std::unique_ptr<cell_event_tracer> ocudu::schedtrace::create_cell_tracer(const ocudu::cell_configuration& cell_cfg)
{
  return std::make_unique<cell_event_tracer>();
}

void schedtrace::init_tracer(const std::string&        dir_path,
                             std::chrono::milliseconds flush_period,
                             timer_manager&            timers,
                             task_executor&            pool_executor)
{
  // Do nothing.
}

void schedtrace::close_tracer()
{
  // Do nothing.
}

#endif
