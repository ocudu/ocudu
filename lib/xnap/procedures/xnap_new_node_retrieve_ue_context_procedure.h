// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../ue_context/xnap_ue_context.h"
#include "ocudu/asn1/xnap/xnap_pdu_contents.h"
#include "ocudu/support/async/protocol_transaction_manager.h"
#include "ocudu/xnap/xnap.h"
#include "ocudu/xnap/xnap_ue_context_retrieval.h"

namespace ocudu::ocucp {

/// \brief Retrieve UE Context procedure at the new NG-RAN node, as defined in TS 38.423 section 8.2.4.
///
/// The new NG-RAN node is the node the UE moved to, and it is the node that initiates this procedure. This is the
/// mirror image of the Handover Preparation procedure, which the node the UE moves away from initiates.
class xnap_new_node_retrieve_ue_context_procedure
{
public:
  xnap_new_node_retrieve_ue_context_procedure(const xnap_retrieve_ue_context_request& request_,
                                              xnap_ue_context_list&                   ue_ctxt_list_,
                                              xnap_message_notifier&                  tx_notifier_);

  void operator()(coro_context<async_task<xnap_retrieve_ue_context_response>>& ctx);

  static const char* name() { return "New NG-RAN Node Retrieve UE Context Procedure"; }

private:
  bool send_retrieve_ue_context_request();

  const xnap_retrieve_ue_context_request request;
  xnap_ue_context_list&                  ue_ctxt_list;
  xnap_message_notifier&                 tx_notifier;

  xnap_ue_context* ue_ctxt = nullptr;

  protocol_transaction_outcome_observer<asn1::xnap::retrieve_ue_context_resp_s, asn1::xnap::retrieve_ue_context_fail_s>
      transaction_sink;

  xnap_retrieve_ue_context_response response;
};

} // namespace ocudu::ocucp
