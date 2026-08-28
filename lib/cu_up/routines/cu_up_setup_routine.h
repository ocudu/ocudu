// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/cu_up/cu_up_e1_setup_notifier.h"
#include "ocudu/e1ap/common/e1_setup_messages.h"
#include "ocudu/e1ap/cu_up/e1ap_cu_up.h"
#include "ocudu/support/async/async_task.h"

namespace ocudu::ocuup {

/// Holds the CU-UP setup routine configuration parameters.
struct cu_up_setup_routine_config {
  gnb_cu_up_id_t           cu_up_id;
  std::string              cu_up_name;
  std::vector<std::string> plmns;
};

/// Holds the CU-UP setup routine dependencies.
struct cu_up_setup_routine_dependencies {
  ocudulog::basic_logger&           logger;
  e1ap_connection_manager&          e1ap_conn_mng;
  cu_up_e1_setup_complete_notifier* e1_setup_notifier = nullptr;
};

/// CU-UP setup routine implementation.
class cu_up_setup_routine
{
public:
  cu_up_setup_routine(cu_up_setup_routine_config cfg, const cu_up_setup_routine_dependencies& dependencies);

  void operator()(coro_context<async_task<bool>>& ctx);

  static const char* name() { return "CU-UP setup routine"; }

private:
  async_task<cu_up_e1_setup_response> start_cu_up_e1_setup_request();
  void                                handle_cu_up_e1_setup_response(const cu_up_e1_setup_response& resp);

  gnb_cu_up_id_t           cu_up_id;
  std::string              cu_up_name;
  std::vector<std::string> plmns;

  ocudulog::basic_logger&           logger;
  e1ap_connection_manager&          e1ap_conn_mng;
  cu_up_e1_setup_complete_notifier* e1_setup_notifier;

  cu_up_e1_setup_response response_msg = {};
};

} // namespace ocudu::ocuup
