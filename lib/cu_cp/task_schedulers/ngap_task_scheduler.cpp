// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ngap_task_scheduler.h"

using namespace ocudu;
using namespace ocucp;

ngap_task_scheduler::ngap_task_scheduler(timer_manager&          timers_,
                                         task_executor&          exec_,
                                         uint16_t                max_nof_amfs,
                                         ocudulog::basic_logger& logger_) :
  timers(timers_), exec(exec_), logger(logger_)
{
  // init AMF control loops
  const size_t number_of_pending_amf_procedures = 16;
  for (size_t i = 0; i < max_nof_amfs; ++i) {
    amf_ctrl_loop.emplace(uint_to_cu_cp_amf_index(i), number_of_pending_amf_procedures);
  }
}

// AMF task scheduler
void ngap_task_scheduler::handle_amf_async_task(cu_cp_amf_index_t amf_index, async_task<void>&& task)
{
  logger.debug("amf={}: Scheduling async task", amf_index);
  amf_ctrl_loop.at(amf_index).schedule(std::move(task));
}

unique_timer ngap_task_scheduler::make_unique_timer()
{
  return timers.create_unique_timer(exec);
}
timer_manager& ngap_task_scheduler::get_timer_manager()
{
  return timers;
}
