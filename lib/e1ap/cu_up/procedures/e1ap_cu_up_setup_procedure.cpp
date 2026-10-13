// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "e1ap_cu_up_setup_procedure.h"
#include "../../common/e1ap_asn1_utils.h"
#include "../e1ap_cu_up_asn1_helpers.h"
#include "ocudu/e1ap/common/e1ap_message.h"

using namespace ocudu;
using namespace ocudu::ocuup;
using namespace asn1::e1ap;

static constexpr std::chrono::milliseconds e1_setup_response_timeout{3000};

e1ap_cu_up_setup_procedure::e1ap_cu_up_setup_procedure(const e1ap_cu_up_setup_procedure_configuration& cfg,
                                                       const e1ap_cu_up_setup_procedure_dependencies&  dependencies) :
  request(cfg.request),
  cu_cp_notifier(dependencies.cu_cp_notif),
  ev_mng(dependencies.ev_mng),
  logger(dependencies.logger)
{
}

void e1ap_cu_up_setup_procedure::operator()(coro_context<async_task<cu_up_e1_setup_response>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.log_debug("\"{}\" initialized", name());

  transaction = ev_mng.transactions.create_transaction(e1_setup_response_timeout);

  // Send request to CU.
  send_cu_up_e1_setup_request();

  // Await CU response.
  // Note: The caller retries a failed E1 Setup, honouring the Time to Wait commanded by the CU-CP.
  CORO_AWAIT(transaction);

  // Forward procedure result to DU manager.
  CORO_RETURN(create_e1_setup_result());
}

void e1ap_cu_up_setup_procedure::send_cu_up_e1_setup_request()
{
  e1ap_message msg = {};
  // set E1AP PDU contents
  msg.pdu.set_init_msg();
  msg.pdu.init_msg().load_info_obj(ASN1_E1AP_ID_GNB_CU_UP_E1_SETUP);
  gnb_cu_up_e1_setup_request_s& setup_req = msg.pdu.init_msg().value.gnb_cu_up_e1_setup_request();

  setup_req = cu_up_e1_setup_request_to_asn1(request);

  setup_req->transaction_id = transaction.id();

  // Capture packed request bytes for later use.
  {
    byte_buffer   packed{byte_buffer::fallback_allocation_tag{}};
    asn1::bit_ref bref(packed);
    if (asn1::pack(bref, msg.pdu) == asn1::OCUDUASN_SUCCESS) {
      captured_request = std::move(packed);
    } else {
      logger.log_warning("\"{}\": failed to pack E1SetupRequest", name());
    }
  }

  cu_cp_notifier.on_new_message(msg);
}

cu_up_e1_setup_response e1ap_cu_up_setup_procedure::create_e1_setup_result()
{
  cu_up_e1_setup_response res{};

  if (transaction.aborted()) {
    // Abortion/timeout case.
    logger.log_warning("\"{}\" failed (timeout reached)", name());
    res.success = false;
    return res;
  }
  const e1ap_transaction_response& cu_cp_pdu_response = transaction.response();

  if (cu_cp_pdu_response.has_value() and
      cu_cp_pdu_response.value().value.type().value ==
          e1ap_elem_procs_o::successful_outcome_c::types_opts::gnb_cu_up_e1_setup_resp) {
    res.success = true;

    if (cu_cp_pdu_response.value().value.gnb_cu_up_e1_setup_resp()->gnb_cu_cp_name_present) {
      res.gnb_cu_cp_name = cu_cp_pdu_response.value().value.gnb_cu_up_e1_setup_resp()->gnb_cu_cp_name.to_string();
    }

    // Capture packed request/response bytes.
    res.packed_e1_setup_request = std::move(captured_request);
    {
      e1ap_message resp_msg                 = {};
      resp_msg.pdu.set_successful_outcome() = cu_cp_pdu_response.value();
      byte_buffer   packed{byte_buffer::fallback_allocation_tag{}};
      asn1::bit_ref bref(packed);
      if (asn1::pack(bref, resp_msg.pdu) == asn1::OCUDUASN_SUCCESS) {
        res.packed_e1_setup_response = std::move(packed);
      } else {
        logger.log_warning("\"{}\": failed to pack E1SetupResponse", name());
      }
    }

    logger.log_info("\"{}\" finalized", name());

  } else if (cu_cp_pdu_response.has_value() and
             cu_cp_pdu_response.error().value.type().value !=
                 e1ap_elem_procs_o::unsuccessful_outcome_c::types_opts::gnb_cu_up_e1_setup_fail) {
    logger.log_error("\"{}\" failed with cause={}", name(), cu_cp_pdu_response.value().value.type().to_string());
    res.success = false;
  } else {
    const gnb_cu_up_e1_setup_fail_ies_container& e1_setup_fail =
        *cu_cp_pdu_response.error().value.gnb_cu_up_e1_setup_fail();
    logger.log_error("\"{}\" failed with cause={}", name(), get_cause_str(e1_setup_fail.cause));
    res.success = false;
    res.cause   = asn1_to_cause(e1_setup_fail.cause);
    if (e1_setup_fail.time_to_wait_present) {
      res.time_to_wait = std::chrono::seconds{e1_setup_fail.time_to_wait.to_number()};
    }
  }
  return res;
}
