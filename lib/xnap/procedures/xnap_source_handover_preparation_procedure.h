// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../ue_context/xnap_ue_context.h"
#include "ocudu/asn1/xnap/xnap_pdu_contents.h"
#include "ocudu/support/async/protocol_transaction_manager.h"
#include "ocudu/xnap/xnap.h"
#include "ocudu/xnap/xnap_handover.h"

namespace ocudu::ocucp {

class xnap_source_handover_preparation_procedure
{
public:
  xnap_source_handover_preparation_procedure(const xnap_handover_request& request_,
                                             xnap_ue_context_list&        ue_ctxt_list_,
                                             xnap_message_notifier&       xnc_notifier_,
                                             xnap_cu_cp_notifier&         cu_cp_notifier_,
                                             timer_factory                timers);

  void operator()(coro_context<async_task<xnap_handover_preparation_response>>& ctx);

  static const char* name() { return "Handover Preparation Procedure"; }

  const xnap_handover_request request;
  xnap_ue_context_list&       ue_ctxt_list;
  xnap_message_notifier&      xnc_notifier;
  xnap_cu_cp_notifier&        cu_cp_notifier;

  unique_timer     txn_reloc_prep_timer;
  xnap_ue_context* ue_ctxt = nullptr;

  protocol_transaction_outcome_observer<asn1::xnap::ho_request_ack_s, asn1::xnap::ho_prep_fail_s> transaction_sink;
  byte_buffer                                                                                     rrc_ho_cmd_pdu;
  bool                               rrc_reconfig_success = false;
  xnap_handover_preparation_response response;

  bool send_handover_request();
  bool send_handover_cancel();

  // ASN.1 helpers
  void fill_asn1_pdu_session_res_list(asn1::xnap::pdu_session_res_to_be_setup_list_l& pdu_session_res_list);

  // TXN Reloc Prep timeout for Handover Preparation procedure.
  // Handover cancellation procedure will be initialized if timer fires.
  // FIXME: Set to appropriate value
  const std::chrono::milliseconds txn_reloc_prep_ms{1000};
};

} // namespace ocudu::ocucp
