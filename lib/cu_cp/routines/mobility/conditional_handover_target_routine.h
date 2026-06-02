// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../../cu_cp_impl_interface.h"
#include "../../mobility_manager/mobility_manager_impl.h"
#include "../../ue_manager/ue_manager_impl.h"
#include "ocudu/f1ap/cu_cp/f1ap_cu_ue_context_update.h"
#include "ocudu/support/async/async_task.h"

namespace ocudu::ocucp {

class du_processor_repository;
class cu_up_processor_repository;

/// \brief Completes CHO execution on the target side upon RRCReconfigurationComplete.
///
/// This routine runs on the target UE's task scheduler for each CHO candidate. It waits for target-side
/// RRCReconfigurationComplete (or timeout). On success, it owns the full CHO winner finalization (NGAP/E1AP context
/// transfer, bearer context modification, F1AP UE context modification, CHO context cleanup, metrics).
class conditional_handover_target_routine
{
public:
  conditional_handover_target_routine(const cu_cp_cho_target_request&        request_,
                                      ue_manager&                            ue_mng_,
                                      du_processor_repository&               du_db_,
                                      cu_up_processor_repository&            cu_up_db_,
                                      cu_cp_ue_context_manipulation_handler& cu_cp_handler_,
                                      cu_cp_ue_context_release_handler&      ue_context_release_handler_,
                                      mobility_manager&                      mobility_mng_,
                                      ocudulog::basic_logger&                logger_);

  void operator()(coro_context<async_task<void>>& ctx);

  static const char* name() { return "CHO Target Routine"; }

private:
  bool fill_bearer_context_security_info(e1ap_bearer_context_modification_request& bearer_mod_request,
                                         const security::sec_as_config&            sec_cfg);

  /// Schedule the source UE release onto the source UE's own task scheduler. Kept out of the coroutine body because
  /// CORO_BEGIN cannot be nested (macro-local name collision).
  void schedule_source_release_on_source_task_sched(cu_cp_ue_index_t source_ue_index);

  const cu_cp_cho_target_request         request;
  cu_cp_ue*                              target_ue = nullptr;
  ue_manager&                            ue_mng;
  du_processor_repository&               du_db;
  cu_up_processor_repository&            cu_up_db;
  cu_cp_ue_context_manipulation_handler& cu_cp_handler;
  cu_cp_ue_context_release_handler&      ue_context_release_handler;
  mobility_manager&                      mobility_mng;

  ocudulog::basic_logger& logger;

  bool reconf_result = false;

  e1ap_bearer_context_modification_request  bearer_ctx_mod_request;
  e1ap_bearer_context_modification_response bearer_ctx_mod_response;
  f1ap_ue_context_modification_request      target_du_context_mod_request;
  cu_cp_ue_context_release_command          release_cmd;
  cu_cp_ue_context_release_complete         release_complete;
};

} // namespace ocudu::ocucp
