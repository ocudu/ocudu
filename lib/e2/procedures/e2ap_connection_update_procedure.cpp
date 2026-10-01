// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "e2ap_connection_update_procedure.h"

using namespace ocudu;
using namespace asn1::e2ap;

e2ap_connection_update_procedure::e2ap_connection_update_procedure(const e2conn_upd_s&     request_,
                                                                   e2_message_notifier&    ric_notif_,
                                                                   timer_factory           timers_,
                                                                   ocudulog::basic_logger& logger_) :
  request(request_), logger(logger_), ric_notif(ric_notif_), timers(timers_)
{
}

void e2ap_connection_update_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  logger.info("E2AP: Received E2 Connection Update");
  CORO_BEGIN(ctx);
  logger.debug("\"{}\" initialized", name());

  // E2 Connection Update not implemented -> always send failure.
  send_e2_connection_update_failure();

  logger.debug("\"{}\" finalized", name());
  CORO_RETURN();
}

void e2ap_connection_update_procedure::send_e2_connection_update_ack()
{
  logger.info("E2AP: Sending E2 Connection Update Ack");
  e2_message msg;
  msg.pdu.set_successful_outcome().load_info_obj(ASN1_E2AP_ID_E2CONN_UPD);
  msg.pdu.successful_outcome().value.e2conn_upd_ack()->transaction_id = request->transaction_id;
  ric_notif.on_new_message(msg);
}

void e2ap_connection_update_procedure::send_e2_connection_update_failure()
{
  logger.info("E2AP: Sending E2 Connection Update Failure");
  e2_message msg;
  msg.pdu.set_unsuccessful_outcome().load_info_obj(ASN1_E2AP_ID_E2CONN_UPD);
  msg.pdu.unsuccessful_outcome().value.e2conn_upd_fail()->transaction_id = request->transaction_id;
  msg.pdu.unsuccessful_outcome().value.e2conn_upd_fail()->cause_present  = true;
  msg.pdu.unsuccessful_outcome().value.e2conn_upd_fail()->cause.set_transport() =
      cause_transport_e::transport_res_unavailable;
  ric_notif.on_new_message(msg);
}
