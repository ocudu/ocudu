// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../ue_manager.h"

namespace ocudu::ocuup {

/// Holds the CU-CP E1 connection loss routine configuration parameters.
struct cu_up_e1_connection_loss_routine_config {
  gnb_cu_up_id_t           cu_up_id;
  std::string              cu_up_name;
  std::vector<std::string> plmns;
};

/// Holds the CU-CP E1 connection loss routine dependencies.
struct cu_up_e1_connection_loss_routine_dependencies {
  std::atomic<bool>&      stop_command;
  e1ap_interface&         e1ap;
  ue_manager&             ue_mng;
  timer_manager&          timers;
  task_executor&          ctrl_exec;
  ocudulog::basic_logger& logger;
};

/// CU-CP E1 connection loss routine.
class cu_up_e1_connection_loss_routine
{
public:
  cu_up_e1_connection_loss_routine(cu_up_e1_connection_loss_routine_config              cfg,
                                   const cu_up_e1_connection_loss_routine_dependencies& dependencies);

  void operator()(coro_context<async_task<void>>& ctx);

  static const char* name() { return "CU-UP E1 connection loss routine"; }

private:
  gnb_cu_up_id_t           cu_up_id;
  std::string              cu_up_name;
  std::vector<std::string> plmns;
  std::atomic<bool>&       stop_command;

  unique_timer            retry_timer;
  e1ap_interface&         e1ap;
  ue_manager&             ue_mng;
  ocudulog::basic_logger& logger;

  bool reconnected{false};
};

} // namespace ocudu::ocuup
