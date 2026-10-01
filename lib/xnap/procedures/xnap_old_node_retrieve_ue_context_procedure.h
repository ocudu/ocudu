// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../ue_context/xnap_ue_context.h"
#include "ocudu/support/async/async_task.h"
#include "ocudu/xnap/xnap.h"
#include "ocudu/xnap/xnap_types.h"
#include "ocudu/xnap/xnap_ue_context_retrieval.h"

namespace ocudu::ocucp {

/// \brief Retrieve UE Context procedure at the old NG-RAN node, as defined in TS 38.423 section 8.2.4.
///
/// The old NG-RAN node is the node that still holds the UE context, and it responds to the procedure the new NG-RAN
/// node initiated.
///
/// TODO: Supervise the retrieval this node granted. Once the response is sent, the XNAP UE context lives until the new
/// NG-RAN node releases it with a UE Context Release, which never arrives if that node fails after the response.
/// TS 38.423 section 8.2.4 defines no timer for this, so the guard has to be a local one.
class xnap_old_node_retrieve_ue_context_procedure
{
public:
  xnap_old_node_retrieve_ue_context_procedure(const xnap_retrieve_ue_context_request& request_,
                                              peer_xnap_ue_id_t                       peer_xnap_ue_id_,
                                              xnap_ue_context_list&                   ue_ctxt_list_,
                                              xnap_cu_cp_notifier&                    cu_cp_notifier_,
                                              xnap_message_notifier&                  tx_notifier_,
                                              ocudulog::basic_logger&                 logger_);

  void operator()(coro_context<async_task<void>>& ctx);

  static const char* name() { return "Old NG-RAN Node Retrieve UE Context Procedure"; }

private:
  bool create_xnap_ue();

  void send_retrieve_ue_context_response();
  void send_retrieve_ue_context_failure(const xnap_cause_t& cause);

  const xnap_retrieve_ue_context_request request;
  const peer_xnap_ue_id_t                peer_xnap_ue_id;
  xnap_ue_context_list&                  ue_ctxt_list;
  xnap_cu_cp_notifier&                   cu_cp_notifier;
  xnap_message_notifier&                 tx_notifier;
  ocudulog::basic_logger&                logger;

  local_xnap_ue_id_t                local_xnap_ue_id = local_xnap_ue_id_t::invalid;
  xnap_retrieve_ue_context_response response;
};

} // namespace ocudu::ocucp
