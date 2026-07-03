// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "bearer_context_release_procedure.h"
#include "cu_cp/ue_context/e1ap_cu_cp_ue_context.h"
#include "ocudu/support/ocudu_assert.h"

using namespace ocudu;
using namespace ocudu::ocucp;
using namespace asn1::e1ap;

bearer_context_release_procedure::bearer_context_release_procedure(const e1ap_configuration& e1ap_cfg_,
                                                                   const e1ap_message&       command_,
                                                                   cu_cp_ue_index_t          ue_index_,
                                                                   e1ap_ue_context_list&     ue_ctxt_list_,
                                                                   e1ap_message_notifier&    e1ap_notif_) :
  e1ap_cfg(e1ap_cfg_), command(command_), ue_index(ue_index_), ue_ctxt_list(ue_ctxt_list_), e1ap_notifier(e1ap_notif_)
{
  ocudu_assert(ue_ctxt_list.contains(ue_index), "Bearer context does not exist in UE context list.");
}

void bearer_context_release_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  ue_ctxt_list[ue_index].logger.log_info("\"{}\" started...", name());

  // Subscribe to respective publisher to receive BEARER CONTEXT RELEASE COMPLETE message.
  transaction_sink.subscribe_to(ue_ctxt_list[ue_index].bearer_ev_mng.context_release_complete, e1ap_cfg.proc_timeout);

  // Send command to CU-UP.
  send_bearer_context_release_command();

  // Await CU response.
  CORO_AWAIT(transaction_sink);

  handle_bearer_context_release_complete();

  /// NOTE: From this point on the UE context is removed and only locally stored variables can be used.

  // Handle response from CU-UP and return bearer index
  CORO_RETURN();
}

void bearer_context_release_procedure::send_bearer_context_release_command()
{
  // send DL RRC message
  e1ap_notifier.on_new_message(command);
}

void bearer_context_release_procedure::handle_bearer_context_release_complete()
{
  if (transaction_sink.successful()) {
    ue_ctxt_list[ue_index].logger.log_info("\"{}\" finished successfully", name());

  } else {
    ue_ctxt_list[ue_index].logger.log_warning("\"{}\" failed. Cause: timeout.", name());
  }

  // Remove UE context.
  ue_ctxt_list.remove_ue(ue_index);
}
