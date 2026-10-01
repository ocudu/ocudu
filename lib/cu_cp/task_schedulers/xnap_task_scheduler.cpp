// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "xnap_task_scheduler.h"

using namespace ocudu;
using namespace ocucp;

xnap_task_scheduler::xnap_task_scheduler(uint16_t max_nof_xnc_peers, ocudulog::basic_logger& logger_) : logger(logger_)
{
  const size_t number_of_pending_xnc_procedures = 16;
  for (size_t i = 0; i < max_nof_xnc_peers; ++i) {
    xnc_ctrl_loop.emplace(uint_to_xnc_peer_index(i), number_of_pending_xnc_procedures);
  }
}

void xnap_task_scheduler::handle_xnc_async_task(xnc_peer_index_t xnc_index, async_task<void>&& task)
{
  logger.debug("xnc_peer={}: Scheduling async task", xnc_index);
  xnc_ctrl_loop.at(xnc_index).schedule(std::move(task));
}

void xnap_task_scheduler::clear_pending_tasks(xnc_peer_index_t xnc_index)
{
  auto it = xnc_ctrl_loop.find(xnc_index);
  if (it != xnc_ctrl_loop.end()) {
    it->second.clear_pending_tasks();
  }
}
