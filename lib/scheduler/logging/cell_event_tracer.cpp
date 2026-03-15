// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "cell_event_tracer.h"
#include "ocudu/ocudulog/ocudulog.h"

#ifndef OCUDU_HAS_SCHEDTRACE

using namespace ocudu::schedtrace;

cell_event_tracer::cell_event_tracer() :
  cell_index(INVALID_DU_CELL_INDEX), logger(ocudulog::fetch_basic_logger("SCHED"))
{
}

cell_event_tracer::cell_event_tracer(du_cell_index_t          cell_idx,
                                     cell_event_channel&      ev_queue_,
                                     const bwp_configuration& dl_bwp,
                                     const bwp_configuration& ul_bwp) :
  cell_index(cell_idx), ev_queue(&ev_queue_), logger(ocudulog::fetch_basic_logger("SCHED"))
{
  (void)cell_index;
}

cell_event_tracer::~cell_event_tracer() {}

void cell_event_tracer::on_cell_start(const bwp_configuration& dl_bwp, const bwp_configuration& ul_bwp) {}

void cell_event_tracer::on_scheduler_result_impl(slot_point                sl,
                                                 const sched_result&       result,
                                                 std::chrono::microseconds slot_latency)
{
}

void cell_event_tracer::on_event_impl(const rach_indication_message& /*prach*/) {}

std::unique_ptr<cell_event_tracer> ocudu::schedtrace::create_cell_tracer(du_cell_index_t          cell_idx,
                                                                         const bwp_configuration& dl_bwp,
                                                                         const bwp_configuration& ul_bwp)
{
  return std::make_unique<cell_event_tracer>();
}

#endif
