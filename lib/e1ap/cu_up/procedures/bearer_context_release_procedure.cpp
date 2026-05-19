// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "bearer_context_release_procedure.h"
#include "common/e1ap_asn1_converters.h"
#include "ocudu/asn1/e1ap/common.h"
#include "ocudu/e1ap/common/e1ap_message.h"
#include "ocudu/e1ap/cu_up/e1ap_cu_up_bearer_context_update.h"

using namespace ocudu;
using namespace ocudu::ocuup;

bearer_context_release_procedure::bearer_context_release_procedure(cu_up_ue_index_t ue_index_,
                                                                   const asn1::e1ap::bearer_context_release_cmd_s& cmd_,
                                                                   e1ap_message_notifier&        pdu_notifier_,
                                                                   e1ap_cu_up_manager_notifier&  cu_up_notifier_,
                                                                   e1ap_cu_up_metrics_collector& metrics_,
                                                                   e1ap_logger&                  logger_) :
  ue_index(ue_index_),
  cmd(cmd_),
  pdu_notifier(pdu_notifier_),
  cu_up_notifier(cu_up_notifier_),
  metrics(metrics_),
  logger(logger_)
{
  proc_start_tp = std::chrono::steady_clock::now();
}

bearer_context_release_procedure::~bearer_context_release_procedure()
{
  auto proc_stop_tp = std::chrono::steady_clock::now();
  metrics.add_context_release(std::chrono::duration_cast<std::chrono::microseconds>(proc_stop_tp - proc_start_tp));
}

void bearer_context_release_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  bearer_context_release_cmd.ue_index = ue_index;
  bearer_context_release_cmd.cause    = asn1_to_cause(cmd->cause);

  // Forward message to CU-UP
  CORO_AWAIT(cu_up_notifier.on_bearer_context_release_command_received(bearer_context_release_cmd));

  e1ap_msg.pdu.set_successful_outcome();
  e1ap_msg.pdu.successful_outcome().load_info_obj(ASN1_E1AP_ID_BEARER_CONTEXT_RELEASE);
  e1ap_msg.pdu.successful_outcome().value.bearer_context_release_complete()->gnb_cu_cp_ue_e1ap_id =
      cmd->gnb_cu_cp_ue_e1ap_id;
  e1ap_msg.pdu.successful_outcome().value.bearer_context_release_complete()->gnb_cu_up_ue_e1ap_id =
      cmd->gnb_cu_up_ue_e1ap_id;

  // send response
  logger.log_debug("ue={} cu_up_ue_e1ap_id={} cu_cp_ue_e1ap_id={}: Sending BearerContextReleaseComplete",
                   fmt::underlying(bearer_context_release_cmd.ue_index),
                   cmd->gnb_cu_up_ue_e1ap_id,
                   cmd->gnb_cu_cp_ue_e1ap_id);
  pdu_notifier.on_new_message(e1ap_msg);
  CORO_RETURN();
}
