// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ngap_task_scheduler.h"

using namespace ocudu;
using namespace ocucp;

ngap_task_scheduler::ngap_task_scheduler(const ngap_task_scheduler_config&       cfg,
                                         const ngap_task_scheduler_dependencies& dependencies) :
  timers(dependencies.timers), exec(dependencies.exec), logger(dependencies.logger)
{
  // Initialize AMF control loops.
  for (size_t i = 0, e = cfg.max_nof_amfs; i != e; ++i) {
    constexpr size_t number_of_pending_amf_procedures = 16;
    amf_ctrl_loop.emplace(uint_to_cu_cp_amf_index(i), number_of_pending_amf_procedures);
  }
}

bool ngap_task_scheduler::handle_amf_async_task(cu_cp_amf_index_t amf_index, async_task<void>&& task)
{
  logger.debug("amf={}: Scheduling async task", amf_index);
  return amf_ctrl_loop.at(amf_index).schedule(std::move(task));
}

unique_timer ngap_task_scheduler::make_unique_timer() const
{
  return timers.create_unique_timer(exec);
}
timer_manager& ngap_task_scheduler::get_timer_manager() const
{
  return timers;
}
