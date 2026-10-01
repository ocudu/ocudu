// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../../ue_manager/ue_manager_impl.h"
#include "../../xnap_repository.h"
#include "ocudu/e1ap/cu_cp/e1ap_cu_cp.h"
#include "ocudu/ngap/ngap.h"
#include "ocudu/support/async/async_task.h"

namespace ocudu::ocucp {

/// \brief Moves the user plane of a UE whose context was retrieved from a peer NG-RAN node over to this node, and
/// releases the context at the peer (TS 38.300 sections 9.2.2.4.1 and 9.2.3.3, the steps after Msg4 is confirmed).
///
/// Shared by both retrieval triggers, which only differ in how the bearers were established beforehand. It runs once
/// the UE has confirmed Msg4, so that the DL path is only moved when the UE is actually being served here.
class retrieved_context_path_switch_routine
{
public:
  retrieved_context_path_switch_routine(cu_cp_ue&                        ue_,
                                        const cu_cp_path_switch_request& path_switch_request_,
                                        e1ap_bearer_context_manager&     e1ap_bearer_ctxt_mng_,
                                        ngap_interface&                  ngap_,
                                        xnap_interface*                  xnap_,
                                        ocudulog::basic_logger&          logger_);

  void operator()(coro_context<async_task<bool>>& ctx);

  static const char* name() { return "Retrieved UE Context Path Switch Routine"; }

private:
  cu_cp_ue&                       ue;
  const cu_cp_path_switch_request path_switch_request;
  e1ap_bearer_context_manager&    e1ap_bearer_ctxt_mng;
  ngap_interface&                 ngap;
  xnap_interface*                 xnap;
  ocudulog::basic_logger&         logger;

  const cu_cp_ue_index_t ue_index;

  cu_cp_path_switch_response               path_switch_response;
  e1ap_bearer_context_modification_request tunnel_context_mod_request;
  cu_cp_ue_context_release_request         ue_context_release_request;
};

} // namespace ocudu::ocucp
