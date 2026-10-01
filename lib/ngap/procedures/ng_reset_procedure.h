// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ngap_transaction_manager.h"
#include "ue_context/ngap_ue_context.h"
#include "ocudu/asn1/ngap/ngap_pdu_contents.h"
#include "ocudu/ngap/ngap.h"
#include "ocudu/support/async/async_task.h"

namespace ocudu {
namespace ocucp {

class ng_reset_procedure
{
public:
  ng_reset_procedure(ngap_context_t&           context_,
                     const cu_cp_reset&        msg_,
                     ngap_message_notifier&    amf_notif_,
                     ngap_transaction_manager& ev_mng_,
                     ngap_ue_context_list&     ue_ctxt_list_,
                     ocudulog::basic_logger&   logger_);

  void operator()(coro_context<async_task<void>>& ctx);

  static const char* name() { return "NG Reset Procedure"; }

  bool send_ng_reset();

private:
  ngap_context_t&           context;
  const cu_cp_reset         msg;
  ngap_message_notifier&    amf_notifier;
  ngap_transaction_manager& ev_mng;
  ngap_ue_context_list&     ue_ctxt_list;
  ocudulog::basic_logger&   logger;

  protocol_transaction_outcome_observer<asn1::ngap::ng_reset_ack_s> transaction_sink;
};

} // namespace ocucp
} // namespace ocudu
