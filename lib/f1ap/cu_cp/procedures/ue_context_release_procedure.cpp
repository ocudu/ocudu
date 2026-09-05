// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ue_context_release_procedure.h"
#include "asn1_helpers.h"
#include "ocudu/adt/format.h"
#include "ocudu/asn1/f1ap/common.h"
#include "ocudu/f1ap/f1ap_message.h"
#include "ocudu/ran/rb_id.h"
#include "ocudu/support/ocudu_assert.h"

using namespace ocudu;
using namespace ocudu::ocucp;
using namespace asn1::f1ap;

ue_context_release_procedure::ue_context_release_procedure(const f1ap_configuration&              f1ap_cfg_,
                                                           const f1ap_ue_context_release_command& cmd_,
                                                           f1ap_ue_context&                       ue_ctxt_,
                                                           f1ap_message_notifier&                 f1ap_notif_) :
  f1ap_cfg(f1ap_cfg_), ue_ctxt(ue_ctxt_), f1ap_notifier(f1ap_notif_), logger(ocudulog::fetch_basic_logger("CU-CP-F1"))
{
  command->gnb_cu_ue_f1ap_id = to_underlying(ue_ctxt.ue_ids.cu_ue_f1ap_id);
  command->gnb_du_ue_f1ap_id = to_underlying(*ue_ctxt.ue_ids.du_ue_f1ap_id);
  command->cause             = cause_to_asn1(cmd_.cause);
  if (!cmd_.rrc_pdu.empty()) {
    command->rrc_container_present = true;
    command->rrc_container         = cmd_.rrc_pdu.copy();

    ocudu_assert(cmd_.srb_id.has_value(), "SRB-ID for UE Context Release Command with RRC Container must be set");

    command->srb_id_present = true;
    command->srb_id         = srb_id_to_uint(cmd_.srb_id.value());
  }
}

void ue_context_release_procedure::operator()(coro_context<async_task<cu_cp_ue_index_t>>& ctx)
{
  CORO_BEGIN(ctx);

  if (ue_ctxt.release_complete_event.is_set()) {
    // Already completed, return cached result immediately.
    logger.debug("{}: Returning cached release result", f1ap_ue_log_prefix{ue_ctxt.ue_ids, name()});
    CORO_EARLY_RETURN(ue_ctxt.release_complete_event.get());
  }

  if (ue_ctxt.f1_removal_in_progress) {
    // F1 TNL is being torn down. The DU will drop the UE context locally; skip the F1 round trip to avoid a
    // guaranteed-to-fail transaction and the resulting noisy warning.
    logger.debug("{}: Skipping F1 UEContextReleaseCommand. Cause: F1 removal in progress",
                 f1ap_ue_log_prefix{ue_ctxt.ue_ids, name()});
    release_result = ue_ctxt.ue_ids.ue_index;
    ue_ctxt.release_complete_event.set(release_result);
    CORO_EARLY_RETURN(release_result);
  }

  if (ue_ctxt.marked_for_release) {
    // In progress, wait for completion.
    logger.debug("{}: Waiting for existing release to complete", f1ap_ue_log_prefix{ue_ctxt.ue_ids, name()});
    CORO_AWAIT_VALUE(release_result, ue_ctxt.release_complete_event);
    CORO_EARLY_RETURN(release_result);
  }

  logger.debug("{}: Procedure started...", f1ap_ue_log_prefix{ue_ctxt.ue_ids, name()});

  ue_ctxt.marked_for_release = true;

  transaction_sink.subscribe_to(ue_ctxt.ev_mng.context_release_complete, f1ap_cfg.proc_timeout);

  // Send command to DU.
  send_ue_context_release_command();

  // Await CU response.
  CORO_AWAIT(transaction_sink);

  // Handle response from DU and return UE index
  release_result = create_ue_context_release_complete();

  // Notify any other callers waiting for release.
  ue_ctxt.release_complete_event.set(release_result);

  CORO_RETURN(release_result);
}

void ue_context_release_procedure::send_ue_context_release_command()
{
  // Pack message into PDU
  f1ap_message f1ap_ue_ctxt_rel_msg;
  f1ap_ue_ctxt_rel_msg.pdu.set_init_msg();
  f1ap_ue_ctxt_rel_msg.pdu.init_msg().load_info_obj(ASN1_F1AP_ID_UE_CONTEXT_RELEASE);
  f1ap_ue_ctxt_rel_msg.pdu.init_msg().value.ue_context_release_cmd() = command;

  // send UE Context Release Command
  f1ap_notifier.on_new_message(f1ap_ue_ctxt_rel_msg);
}

cu_cp_ue_index_t ue_context_release_procedure::create_ue_context_release_complete()
{
  if (transaction_sink.successful()) {
    gnb_du_ue_f1ap_id_t du_ue_id = int_to_gnb_du_ue_f1ap_id(transaction_sink.response()->gnb_du_ue_f1ap_id);
    if (!ue_ctxt.ue_ids.du_ue_f1ap_id || du_ue_id != *ue_ctxt.ue_ids.du_ue_f1ap_id) {
      logger.error("{}: Procedure failed. Cause: gNB-DU-UE-F1AP-ID mismatch.",
                   f1ap_ue_log_prefix{ue_ctxt.ue_ids, name()});
      return cu_cp_ue_index_t::invalid;
    }
    return ue_ctxt.ue_ids.ue_index;
  }

  logger.warning("{}: Procedure failed. Cause: {}",
                 f1ap_ue_log_prefix{ue_ctxt.ue_ids, name()},
                 transaction_sink.timeout_expired() ? "Timeout" : "Transaction failed");

  return cu_cp_ue_index_t::invalid;
}
