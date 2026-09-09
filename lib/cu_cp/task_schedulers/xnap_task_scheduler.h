// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ocudulog/logger.h"
#include "ocudu/ran/cu_cp_types.h"
#include "ocudu/support/async/fifo_async_task_scheduler.h"
#include <map>

namespace ocudu::ocucp {

/// \brief Service provided by CU-CP to schedule async tasks for a given Xn-C peer.
class xnap_task_scheduler
{
public:
  explicit xnap_task_scheduler(uint16_t max_nof_xnc_peers, ocudulog::basic_logger& logger_);
  ~xnap_task_scheduler() = default;

  void handle_xnc_async_task(xnc_peer_index_t xnc_index, async_task<void>&& task);

  void clear_pending_tasks(xnc_peer_index_t xnc_index);

private:
  ocudulog::basic_logger& logger;

  // task event loops indexed by xnc_peer_index
  std::map<xnc_peer_index_t, fifo_async_task_scheduler> xnc_ctrl_loop;
};

} // namespace ocudu::ocucp
