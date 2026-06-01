// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/asn1/f1ap/f1ap.h"
#include "ocudu/f1ap/f1ap_message_notifier.h"
#include "ocudu/support/async/async_task.h"

namespace ocudu {
namespace ocucp {

class f1ap_du_processor_notifier;
class f1ap_ue_context_list;

/// \brief Handle the F1 Removal Procedure as per TS 38.473 section 8.2.8.
class f1_removal_procedure
{
public:
  f1_removal_procedure(const asn1::f1ap::f1_removal_request_s& request,
                       f1ap_message_notifier&                  pdu_notifier,
                       f1ap_du_processor_notifier&             cu_cp_notifier,
                       f1ap_ue_context_list&                   ue_list,
                       ocudulog::basic_logger&                 logger);

  void operator()(coro_context<async_task<void>>& ctx);

private:
  const char*      name() const { return "F1 Removal"; }
  async_task<void> handle_ue_transaction_info_loss();
  void             send_f1_removal_response();

  const asn1::f1ap::f1_removal_request_s request;
  f1ap_message_notifier&                 pdu_notifier;
  f1ap_du_processor_notifier&            cu_cp_notifier;
  f1ap_ue_context_list&                  ue_list;
  ocudulog::basic_logger&                logger;
};

} // namespace ocucp
} // namespace ocudu
