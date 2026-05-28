// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "positioning_activation_procedure.h"
#include "../nrppa_asn1_converters.h"
#include "../nrppa_helper.h"
#include "ocudu/asn1/nrppa/common.h"
#include "ocudu/asn1/nrppa/nrppa.h"
#include "ocudu/asn1/nrppa/nrppa_pdu_contents.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/ran/cause/nrppa_cause.h"
#include "ocudu/support/async/coroutine.h"

using namespace ocudu;
using namespace ocucp;

positioning_activation_procedure::positioning_activation_procedure(const positioning_activation_request_t& request_,
                                                                   uint16_t                transaction_id_,
                                                                   cu_cp_du_index_t        du_index_,
                                                                   nrppa_du_context_list&  du_ctxt_list_,
                                                                   nrppa_cu_cp_notifier&   cu_cp_notifier_,
                                                                   ocudulog::basic_logger& logger_) :
  pos_act_request(request_),
  transaction_id(transaction_id_),
  du_index(du_index_),
  du_ctxt_list(du_ctxt_list_),
  cu_cp_notifier(cu_cp_notifier_),
  logger(logger_)
{
}

void positioning_activation_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.info("ue={}: \"{}\" initialized", pos_act_request.ue_index, name());

  // Sanity check.
  if (!du_ctxt_list.contains(du_index)) {
    logger.error("DU context not found for UE {}", pos_act_request.ue_index);
    logger.info("ue={}: \"{}\" failed", pos_act_request.ue_index, name());
    send_ul_nrppa_pdu(
        create_positioning_activation_failure(nrppa_cause_protocol_t::msg_not_compatible_with_receiver_state));
    CORO_EARLY_RETURN();
  }

  CORO_AWAIT_VALUE(procedure_outcome, du_ctxt_list[du_index].f1ap->on_positioning_activation_request(pos_act_request));

  // Pack positioning activation outcome and forward to CU-CP.
  handle_procedure_outcome();

  CORO_RETURN();
}

void positioning_activation_procedure::handle_procedure_outcome()
{
  if (!procedure_outcome.has_value()) {
    asn1_pos_act_outcome = create_positioning_activation_failure(procedure_outcome.error().cause);
    logger.info("ue={}: \"{}\" failed", pos_act_request.ue_index, name());
  } else {
    asn1_pos_act_outcome = create_positioning_activation_response();
    logger.info("ue={}: \"{}\" finished successfully", pos_act_request.ue_index, name());
  }

  // Send response to CU-CP.
  send_ul_nrppa_pdu(asn1_pos_act_outcome);
}

void positioning_activation_procedure::send_ul_nrppa_pdu(const asn1::nrppa::nr_ppa_pdu_c& outcome)
{
  // Pack into PDU.
  ul_nrppa_pdu = pack_into_pdu(outcome,
                               outcome.type().value == asn1::nrppa::nr_ppa_pdu_c::types_opts::successful_outcome
                                   ? "PositioningActivationResponse"
                                   : "PositioningActivationFailure");

  // Log Tx message.
  log_nrppa_message(ocudulog::fetch_basic_logger("NRPPA"), Tx, ul_nrppa_pdu, outcome);

  // Send response to CU-CP.
  cu_cp_notifier.on_ul_nrppa_pdu(ul_nrppa_pdu, pos_act_request.ue_index);
}

asn1::nrppa::nr_ppa_pdu_c
positioning_activation_procedure::create_positioning_activation_failure(nrppa_cause_t cause) const
{
  asn1::nrppa::nr_ppa_pdu_c asn1_fail;

  asn1_fail.set_unsuccessful_outcome().load_info_obj(ASN1_NRPPA_ID_POSITIONING_ACTIVATION);
  asn1_fail.unsuccessful_outcome().nrppatransaction_id = transaction_id;
  asn1::nrppa::positioning_activation_fail_s& pos_act_fail =
      asn1_fail.unsuccessful_outcome().value.positioning_activation_fail();
  pos_act_fail->cause = cause_to_asn1(cause);

  return asn1_fail;
}

asn1::nrppa::nr_ppa_pdu_c positioning_activation_procedure::create_positioning_activation_response()
{
  asn1::nrppa::nr_ppa_pdu_c asn1_resp;

  asn1_resp.set_successful_outcome().load_info_obj(ASN1_NRPPA_ID_POSITIONING_ACTIVATION);
  asn1_resp.successful_outcome().nrppatransaction_id = transaction_id;

  asn1::nrppa::positioning_activation_resp_s& asn1_pos_act_resp =
      asn1_resp.successful_outcome().value.positioning_activation_resp();

  const positioning_activation_response_t& pos_act_resp = procedure_outcome.value();

  // TODO: Fill criticality diagnostics.

  // Fill sys frame number.
  if (pos_act_resp.sys_frame_num.has_value()) {
    asn1_pos_act_resp->sys_frame_num = pos_act_resp.sys_frame_num.value();
  }

  // Fill slot number.
  if (pos_act_resp.slot_num.has_value()) {
    asn1_pos_act_resp->slot_num = pos_act_resp.slot_num.value();
  }

  return asn1_resp;
}
