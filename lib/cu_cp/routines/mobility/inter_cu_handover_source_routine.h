// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../../cu_up_processor/cu_up_processor_repository.h"
#include "../../du_processor/du_processor_repository.h"
#include "../../ue_manager/ue_manager_impl.h"
#include "ocudu/ocudulog/logger.h"
#include "ocudu/ran/cu_cp_types.h"
#include "ocudu/support/async/async_task.h"

namespace ocudu::ocucp {

class inter_cu_handover_source_routine
{
public:
  inter_cu_handover_source_routine(cu_cp_rrc_handover_command    command_,
                                   ue_manager&                   ue_mng_,
                                   du_processor_repository&      du_db_,
                                   cu_up_processor_repository&   cu_up_db_,
                                   ngap_control_message_handler& ngap_,
                                   xnap_interface*               xnap_,
                                   ocudulog::basic_logger&       logger_);

  void operator()(coro_context<async_task<bool>>& ctx);

  static const char* name() { return "Inter CU Handover Source Routine"; }

private:
  void fill_e1ap_bearer_context_modification_request();
  bool fill_status_transfer();

  cu_cp_rrc_handover_command    command;
  cu_cp_ue_index_t              ue_index;
  ue_manager&                   ue_mng;
  du_processor_repository&      du_db;
  cu_up_processor_repository&   cu_up_db;
  ngap_control_message_handler& ngap;
  xnap_interface*               xnap = nullptr;

  ocudulog::basic_logger& logger;

  byte_buffer                               ho_reconfig_pdu;
  e1ap_bearer_context_modification_request  bearer_mod_req          = {};
  e1ap_bearer_context_modification_response bearer_mod_resp         = {};
  f1ap_ue_context_modification_response     ue_context_mod_response = {};
  f1ap_ue_context_modification_request      ue_context_mod_request  = {};
  cu_cp_status_transfer                     status_transfer         = {};
};

} // namespace ocudu::ocucp
