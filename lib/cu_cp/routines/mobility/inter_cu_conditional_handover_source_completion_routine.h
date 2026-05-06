// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../../cu_up_processor/cu_up_processor_repository.h"
#include "../../ue_manager/ue_manager_impl.h"
#include "../../xnap_repository.h"
#include "ocudu/cu_cp/cu_cp_types.h"
#include "ocudu/ocudulog/logger.h"
#include "ocudu/support/async/async_task.h"
#include "ocudu/xnap/xnap.h"
#include "ocudu/xnap/xnap_types.h"

namespace ocudu::ocucp {

/// \brief Source-side completion routine for inter-CU CHO via Xn (TS 38.423 section 8.2.4).
///
/// Triggered when HandoverSuccess is received from the winning target CU-CP.
/// Per spec, the SN Status Transfer MUST be sent AFTER HandoverSuccess, not before.
///
/// Flow:
/// 1. Cancel non-winning candidates (intra-CU via RRC transaction cancel, inter-CU via Xn).
/// 2. Query CU-UP for PDCP SN status.
/// 3. Send SN Status Transfer to the winning target CU-CP via Xn.
/// 4. Release source UE context.
class inter_cu_conditional_handover_source_completion_routine
{
public:
  inter_cu_conditional_handover_source_completion_routine(cu_cp_ue_index_t                  source_ue_index_,
                                                          peer_xnap_ue_id_t                 winner_peer_xnap_ue_id_,
                                                          ue_manager&                       ue_mng_,
                                                          cu_up_processor_repository&       cu_up_db_,
                                                          xnap_interface*                   winner_xnap_,
                                                          xnap_repository*                  xnap_db_,
                                                          cu_cp_ue_context_release_handler& release_handler_,
                                                          ocudulog::basic_logger&           logger_);

  void operator()(coro_context<async_task<void>>& ctx);

  static const char* name() { return "Inter-CU CHO Source Completion Routine"; }

private:
  void fill_e1ap_bearer_modification_request_pdcp_sn_query();
  bool fill_status_transfer();

  const cu_cp_ue_index_t            source_ue_index;
  const peer_xnap_ue_id_t           winner_peer_xnap_ue_id;
  ue_manager&                       ue_mng;
  cu_up_processor_repository&       cu_up_db;
  xnap_interface*                   winner_xnap = nullptr;
  xnap_repository*                  xnap_db     = nullptr;
  cu_cp_ue_context_release_handler& release_handler;
  ocudulog::basic_logger&           logger;

  e1ap_bearer_context_modification_request  bearer_mod_req   = {};
  e1ap_bearer_context_modification_response bearer_mod_resp  = {};
  cu_cp_status_transfer                     status_transfer  = {};
  cu_cp_ue_context_release_command          release_cmd      = {};
  cu_cp_ue_context_release_complete         release_complete = {};
};

} // namespace ocudu::ocucp
