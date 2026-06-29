// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "write_replace_warning_procedure.h"
#include "asn1_helpers.h"
#include "ocudu/asn1/f1ap/common.h"
#include "ocudu/asn1/f1ap/f1ap_pdu_contents.h"
#include "ocudu/f1ap/f1ap_message.h"
#include <vector>

using namespace ocudu;
using namespace ocudu::ocucp;
using namespace asn1::f1ap;

write_replace_warning_procedure::write_replace_warning_procedure(const f1ap_configuration&          f1ap_cfg_,
                                                                 f1ap_write_replace_warning_request request_,
                                                                 f1ap_message_notifier&             f1ap_notif_,
                                                                 f1ap_event_manager&                ev_mng_,
                                                                 ocudulog::basic_logger&            logger_) :
  f1ap_cfg(f1ap_cfg_), request(std::move(request_)), f1ap_notifier(f1ap_notif_), ev_mng(ev_mng_), logger(logger_)
{
}

void write_replace_warning_procedure::operator()(coro_context<async_task<f1ap_write_replace_warning_response>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.info("\"{}\" started...", name());

  transaction = ev_mng.transactions.create_transaction(f1ap_cfg.proc_timeout);

  send_write_replace_warning_request();

  CORO_AWAIT(transaction);

  CORO_RETURN(handle_procedure_result());
}

void write_replace_warning_procedure::send_write_replace_warning_request()
{
  f1ap_message msg;
  msg.pdu.set_init_msg().load_info_obj(ASN1_F1AP_ID_WRITE_REPLACE_WARNING);
  auto& req = msg.pdu.init_msg().value.write_replace_warning_request();

  req->transaction_id = transaction.id();

  req->pws_sys_info.sib_type = request.pws_sys_info.sib_type;
  const std::vector<uint8_t> sib_bytes(request.pws_sys_info.sib_msg.begin(), request.pws_sys_info.sib_msg.end());
  req->pws_sys_info.sib_msg.from_bytes(sib_bytes);

  req->repeat_period           = request.repeat_period;
  req->numof_broadcast_request = request.nof_broadcasts_requested;

  if (request.cells_to_be_broadcast.has_value()) {
    req->cells_to_be_broadcast_list_present = true;
    for (const auto& cgi : *request.cells_to_be_broadcast) {
      asn1::protocol_ie_single_container_s<cells_to_be_broadcast_list_item_ies_o> item;
      item->cells_to_be_broadcast_item().nr_cgi = cgi_to_asn1(cgi);
      req->cells_to_be_broadcast_list.push_back(item);
    }
  }

  f1ap_notifier.on_new_message(msg);
}

f1ap_write_replace_warning_response write_replace_warning_procedure::handle_procedure_result()
{
  response.success = false;

  if (not transaction.valid()) {
    logger.debug("\"{}\" cancelled. Cause: Failed to allocate transaction", name());
    return response;
  }

  if (transaction.aborted()) {
    logger.debug("\"{}\" cancelled. Cause: Timeout reached", name());
    return response;
  }

  if (transaction.has_response() and transaction.response().has_value()) {
    const auto& resp = transaction.response().value().value.write_replace_warning_resp();
    response.success = true;

    if (resp->cells_broadcast_completed_list_present) {
      for (const auto& item : resp->cells_broadcast_completed_list) {
        auto cgi = cgi_from_asn1(item->cells_broadcast_completed_item().nr_cgi);
        if (cgi.has_value()) {
          response.cells_broadcast_completed.push_back(cgi.value());
        }
      }
    }

    logger.info("\"{}\" finished successfully", name());
  }

  return response;
}
