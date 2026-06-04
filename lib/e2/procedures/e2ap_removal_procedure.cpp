// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "e2ap_removal_procedure.h"
#include "ocudu/asn1/asn1_ap_utils.h"

using namespace ocudu;
using namespace asn1::e2ap;

e2ap_removal_procedure::e2ap_removal_procedure(e2_message_notifier&    notif_,
                                               e2_event_manager&       ev_mng_,
                                               timer_factory           timers_,
                                               ocudulog::basic_logger& logger_) :
  notifier(notif_), ev_mng(ev_mng_), logger(logger_)
{
}

void e2ap_removal_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  transaction = ev_mng.transactions.create_transaction(std::chrono::seconds{5});
  send_e2_removal_request();

  CORO_AWAIT(transaction);

  if (transaction.aborted()) {
    if (transaction.failure_cause() == protocol_transaction_failure::timeout) {
      logger.warning("E2 Removal procedure timed out waiting for RIC response. Proceeding with disconnection.");
    } else {
      logger.warning("E2 Removal procedure was cancelled. Proceeding with disconnection.");
    }
  } else if (transaction.response().has_value()) {
    logger.info("E2 Removal procedure successful.");
  } else {
    logger.warning("E2 Removal procedure failed.");
  }

  CORO_RETURN();
}

void e2ap_removal_procedure::send_e2_removal_request()
{
  e2_message msg = {};
  msg.pdu.set_init_msg();
  msg.pdu.init_msg().load_info_obj(ASN1_E2AP_ID_E2REMOVAL);
  auto& rem_req = msg.pdu.init_msg().value.e2_removal_request();

  asn1::protocol_ie_field_s<e2_removal_request_ies_o> ie;
  ie.value().transaction_id() = transaction.id();
  rem_req->push_back(ie);

  notifier.on_new_message(msg);
}
