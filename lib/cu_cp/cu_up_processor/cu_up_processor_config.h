// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/cu_cp/cu_cp_configuration.h"
#include <string>

namespace ocudu {

class async_task_scheduler;

namespace ocucp {

/// CU-UP processor configuration.
struct cu_up_processor_config {
  std::string         name        = "ocucp";
  cu_cp_cu_up_index_t cu_up_index = cu_cp_cu_up_index_t::invalid;
  e1ap_configuration  e1ap;
  uint32_t            max_nof_ues;
};

/// CU-UP processor dependencies.
struct cu_up_processor_dependencies {
  task_executor&         cu_cp_executor;
  timer_manager&         timers;
  e1ap_message_notifier& e1ap_notifier;
  e1ap_cu_cp_notifier&   cu_cp_notifier;
  async_task_scheduler&  common_task_sched;
};

} // namespace ocucp
} // namespace ocudu
