// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "trp_information_exchange_procedure.h"
#include "../nrppa_asn1_converters.h"
#include "../nrppa_helper.h"
#include "ocudu/asn1/nrppa/common.h"
#include "ocudu/asn1/nrppa/nrppa_pdu_contents.h"
#include "ocudu/support/async/coroutine.h"

using namespace ocudu;
using namespace ocucp;

trp_information_exchange_procedure::trp_information_exchange_procedure(
    cu_cp_amf_index_t                     amf_index_,
    const trp_information_request_t&      request_,
    uint16_t                              transaction_id_,
    nrppa_cu_cp_notifier&                 cu_cp_notifier_,
    std::map<trp_id_t, cu_cp_du_index_t>& trp_id_to_du_idx_,
    nrppa_du_context_list&                du_ctxt_list_,
    ocudulog::basic_logger&               logger_) :
  amf_index(amf_index_),
  trp_info_request(request_),
  transaction_id(transaction_id_),
  cu_cp_notifier(cu_cp_notifier_),
  trp_id_to_du_idx(trp_id_to_du_idx_),
  du_ctxt_list(du_ctxt_list_),
  logger(logger_)
{
}

static bool all_trp_ids_known(const std::vector<trp_id_t>&                trp_list,
                              const std::map<trp_id_t, cu_cp_du_index_t>& trp_id_to_du_idx)
{
  return std::all_of(trp_list.begin(), trp_list.end(), [&](const trp_id_t& trp_id) {
    return trp_id_to_du_idx.find(trp_id) != trp_id_to_du_idx.end();
  });
}

void trp_information_exchange_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.debug("\"{}\" initialized", name());

  // If the TRP list is empty or contains unknown TRP IDs, we forward the request to the CU-CP, as we need the
  // information about TRPs from all DUs.
  if (trp_info_request.trp_list.empty() or !all_trp_ids_known(trp_info_request.trp_list, trp_id_to_du_idx)) {
    CORO_AWAIT_VALUE(cu_cp_response, cu_cp_notifier.on_trp_information_request(trp_info_request));
  }

  handle_procedure_outcome();

  logger.debug("\"{}\" finished successfully", name());

  CORO_RETURN();
}

void trp_information_exchange_procedure::handle_procedure_outcome()
{
  if (cu_cp_response.trp_info_responses.empty()) {
    trp_info_outcome = create_trp_info_failure(nrppa_cause_radio_network_t::unspecified);
    logger.debug("\"{}\" failed", name());
  } else {
    for (const auto& [du_idx, trp_info_resp] : cu_cp_response.trp_info_responses) {
      for (const auto& trp_info_list_trp_resp_item : trp_info_resp.trp_info_list_trp_resp) {
        trp_id_to_du_idx.emplace(trp_info_list_trp_resp_item.trp_info.trp_id, du_idx);
      }

      if (cu_cp_response.f1ap_notifiers.find(du_idx) == cu_cp_response.f1ap_notifiers.end()) {
        logger.error("F1AP notifier not found for DU {}", du_idx);
        trp_info_outcome = create_trp_info_failure(nrppa_cause_protocol_t::unspecified);
        send_ul_nrppa_pdu(trp_info_outcome);
        return;
      }

      du_ctxt_list.add_du(du_idx, *cu_cp_response.f1ap_notifiers.at(du_idx));
    }

    trp_info_outcome = create_trp_info_response();
    logger.debug("\"{}\" finished successfully", name());
  }

  // Send response to CU-CP.
  send_ul_nrppa_pdu(trp_info_outcome);
}

void trp_information_exchange_procedure::send_ul_nrppa_pdu(const asn1::nrppa::nr_ppa_pdu_c& pdu)
{
  // Pack into PDU.
  ul_nrppa_pdu =
      pack_into_pdu(pdu,
                    pdu.type().value == asn1::nrppa::nr_ppa_pdu_c::types_opts::successful_outcome ? "TRPInfoResponse"
                                                                                                  : "TRPInfoFailure");

  // Log Tx message.
  log_nrppa_message(logger, Tx, ul_nrppa_pdu, pdu);
  // Send response to CU-CP.
  cu_cp_notifier.on_ul_nrppa_pdu(ul_nrppa_pdu, amf_index);
}

asn1::nrppa::nr_ppa_pdu_c trp_information_exchange_procedure::create_trp_info_failure(nrppa_cause_t cause) const
{
  asn1::nrppa::nr_ppa_pdu_c asn1_fail;

  asn1_fail.set_unsuccessful_outcome().load_info_obj(ASN1_NRPPA_ID_T_RP_INFO_EXCHANGE);
  asn1_fail.unsuccessful_outcome().nrppatransaction_id = transaction_id;
  asn1::nrppa::trp_info_fail_s& trp_info_fail          = asn1_fail.unsuccessful_outcome().value.trp_info_fail();

  trp_info_fail->cause = cause_to_asn1(cause);

  return asn1_fail;
}

asn1::nrppa::nr_ppa_pdu_c trp_information_exchange_procedure::create_trp_info_response()
{
  asn1::nrppa::nr_ppa_pdu_c asn1_resp;

  asn1_resp.set_successful_outcome().load_info_obj(ASN1_NRPPA_ID_T_RP_INFO_EXCHANGE);
  asn1_resp.successful_outcome().nrppatransaction_id = transaction_id;

  for (const auto& trp_info_response : cu_cp_response.trp_info_responses) {
    for (const auto& trp_info_list_trp_response_item : trp_info_response.second.trp_info_list_trp_resp) {
      asn1_resp.successful_outcome().value.trp_info_resp()->trp_info_list_trp_resp.push_back(
          trp_information_list_trp_response_item_to_asn1(trp_info_list_trp_response_item));
    }

    // TODO: add criticality diagnostics.
  }

  return asn1_resp;
}
