// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "cu_up/ue_context/e1ap_cu_up_ue_context.h"
#include "ocudu/asn1/e1ap/e1ap.h"
#include "ocudu/asn1/e1ap/e1ap_pdu_contents.h"
#include "ocudu/e1ap/cu_up/e1ap_cu_up.h"
#include "ocudu/support/async/async_task.h"

namespace ocudu {
namespace ocuup {

/// This coroutines handles the E1AP CU UP release procedure as per TS 37.483, 8.2.7.2.2.
class e1ap_cu_up_reset_procedure
{
public:
  e1ap_cu_up_reset_procedure(asn1::e1ap::reset_s          reset_msg_,
                             e1ap_ue_context_list&        ue_ctxt_list_,
                             e1ap_cu_up_manager_notifier& cu_up_notifier_,
                             e1ap_message_notifier&       tx_pdu_notifier_,
                             e1ap_logger&                 logger_);

  void operator()(coro_context<async_task<void>>& ctx);

private:
  static const char* name() { return "E1AP CU-UP Reset Procedure"; }
  async_task<void>   handle_e1_interface_reset(const asn1::e1ap::reset_all_e& reset_all);
  async_task<void>
       handle_part_of_e1_interface_reset(const asn1::e1ap::ue_associated_lc_e1_conn_list_res_l& ue_reset_list);
  void send_e1ap_reset_ack();

  asn1::e1ap::reset_s          reset_msg;
  e1ap_ue_context_list&        ue_ctxt_list;
  e1ap_cu_up_manager_notifier& cu_up_notifier;
  e1ap_message_notifier&       tx_pdu_notifier;
  e1ap_logger&                 logger;
};

} // namespace ocuup
} // namespace ocudu
