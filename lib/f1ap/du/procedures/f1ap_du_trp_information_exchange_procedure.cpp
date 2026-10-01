// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "f1ap_du_trp_information_exchange_procedure.h"
#include "../../asn1_helpers.h"
#include "ocudu/adt/format.h"
#include "ocudu/asn1/f1ap/common.h"
#include "ocudu/asn1/f1ap/f1ap_pdu_contents.h"
#include "ocudu/f1ap/du/f1ap_du.h"
#include "ocudu/f1ap/du/f1ap_du_positioning_handler.h"
#include "ocudu/f1ap/f1ap_message.h"
#include "ocudu/f1ap/f1ap_message_notifier.h"

using namespace ocudu;
using namespace odu;
using namespace asn1::f1ap;

f1ap_du_trp_information_exchange_procedure::f1ap_du_trp_information_exchange_procedure(
    const trp_info_request_s&    msg_,
    f1ap_du_positioning_handler& du_mng_,
    f1ap_message_notifier&       cu_notifier_) :
  msg(msg_), du_mng(du_mng_), cu_notifier(cu_notifier_), logger(ocudulog::fetch_basic_logger("DU-F1"))
{
}

void f1ap_du_trp_information_exchange_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  if (not validate_request()) {
    send_failure();
    CORO_EARLY_RETURN();
  }

  // Request TRPs from DU.
  du_trp_info_response du_resp = du_mng.request_trp_info();

  if (du_resp.trps.empty()) {
    logger.debug("TRP information exchange procedure failed: no TRPs found");
    send_failure();
    CORO_EARLY_RETURN();
  }

  // Send response back to CU-CP.
  send_response(du_resp);

  CORO_RETURN();
}

bool f1ap_du_trp_information_exchange_procedure::validate_request() const
{
  if (msg->trp_list_present) {
    logger.error("TRP list in TRP information request not supported");
    return false;
  }

  return true;
}

void f1ap_du_trp_information_exchange_procedure::send_response(const du_trp_info_response& du_resp) const
{
  f1ap_message resp_msg;

  resp_msg.pdu.set_successful_outcome().load_info_obj(ASN1_F1AP_ID_TRP_INFO_EXCHANGE);
  auto& resp = resp_msg.pdu.successful_outcome().value.trp_info_resp();

  resp->transaction_id = msg->transaction_id;

  resp->trp_info_list_trp_resp.resize(du_resp.trps.size());
  for (unsigned i = 0, e = du_resp.trps.size(); i != e; ++i) {
    resp->trp_info_list_trp_resp[i].load_info_obj(ASN1_F1AP_ID_TRP_INFO_ITEM);
    trp_info_s& asn1_trp_info = resp->trp_info_list_trp_resp[i].value().trp_info_item().trp_info;
    asn1_trp_info             = trp_info_to_asn1(du_resp.trps[i]);
  }

  // Send response to CU-CP.
  cu_notifier.on_new_message(resp_msg);
}

void f1ap_du_trp_information_exchange_procedure::send_failure() const
{
  f1ap_message fail_msg;

  fail_msg.pdu.set_unsuccessful_outcome().load_info_obj(ASN1_F1AP_ID_TRP_INFO_EXCHANGE);
  auto& fail = fail_msg.pdu.unsuccessful_outcome().value.trp_info_fail();

  fail->transaction_id         = msg->transaction_id;
  fail->cause.set_misc().value = cause_misc_opts::unspecified;

  // Send response to CU-CP.
  cu_notifier.on_new_message(fail_msg);
}
