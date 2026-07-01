// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ngap_path_switch_procedure.h"
#include "ngap_asn1_converters.h"
#include "ue_context/ngap_ue_context.h"
#include "ocudu/asn1/ngap/common.h"
#include "ocudu/asn1/ngap/ngap.h"
#include "ocudu/asn1/ngap/ngap_ies.h"
#include "ocudu/ngap/ngap_message.h"
#include "ocudu/ran/cu_types.h"
#include "ocudu/ran/inter_cu_handover_messages.h"
#include "ocudu/security/security_asn1_utils.h"
#include <variant>

using namespace ocudu;
using namespace ocudu::ocucp;
using namespace asn1::ngap;

ngap_path_switch_procedure::ngap_path_switch_procedure(const cu_cp_path_switch_request& request_,
                                                       ngap_ue_context&                 ue_ctxt_,
                                                       ngap_message_notifier&           amf_notifier_) :
  request(request_), ue_ctxt(ue_ctxt_), amf_notifier(amf_notifier_)
{
}

void ngap_path_switch_procedure::operator()(coro_context<async_task<cu_cp_path_switch_response>>& ctx)
{
  CORO_BEGIN(ctx);
  ue_ctxt.logger.log_info("\"{}\" started...", name());

  // Subscribe to respective publisher to receive PATH SWITCH REQUEST ACK/FAILURE message.
  transaction_sink.subscribe_to(ue_ctxt.ev_mng.path_switch_outcome, std::chrono::milliseconds{5000});

  // Send Path Switch Request to AMF.
  if (!send_path_switch_request()) {
    ue_ctxt.logger.log_warning("\"{}\" failed. Cause: Couldn't send Path Switch Request to AMF", name());
    CORO_EARLY_RETURN(cu_cp_path_switch_request_failure{.ue_index = request.ue_index});
  }

  CORO_AWAIT(transaction_sink);

  if (transaction_sink.timeout_expired()) {
    ue_ctxt.logger.log_warning("\"{}\" timed out after {}ms", name(), 5000);
    CORO_EARLY_RETURN(cu_cp_path_switch_request_failure{.ue_index = request.ue_index});
  }

  if (not transaction_sink.successful()) {
    ue_ctxt.logger.log_warning("\"{}\" failed", name());
    // Convert procedure outcome to procedure response and return.
    CORO_EARLY_RETURN(asn1_to_path_switch_request_failure(transaction_sink.failure()));
  }

  // Convert and handle Path Switch Request Ack.
  procedure_response = handle_successful_outcome(transaction_sink.response());

  if (std::holds_alternative<cu_cp_path_switch_request_failure>(procedure_response)) {
    ue_ctxt.logger.log_warning("\"{}\" failed. Cause: Failure at response handling", name());
  } else {
    ue_ctxt.logger.log_info("\"{}\" finished successfully", name());
  }

  CORO_RETURN(procedure_response);
}

bool ngap_path_switch_procedure::send_path_switch_request()
{
  ngap_message ngap_msg = {};
  ngap_msg.pdu.set_init_msg();
  ngap_msg.pdu.init_msg().load_info_obj(ASN1_NGAP_ID_PATH_SWITCH_REQUEST);

  path_switch_request_s& asn1_path_switch_request = ngap_msg.pdu.init_msg().value.path_switch_request();
  // Fill UE IDs.
  asn1_path_switch_request->ran_ue_ngap_id        = ran_ue_id_to_uint(ue_ctxt.ue_ids.ran_ue_id);
  asn1_path_switch_request->source_amf_ue_ngap_id = amf_ue_id_to_uint(ue_ctxt.ue_ids.amf_ue_id);
  // Fill User Location Information.
  asn1_path_switch_request->user_location_info.set_user_location_info_nr() =
      cu_cp_user_location_info_to_asn1(request.user_location_info);
  // Fill UE Security Capabilities.
  asn1_path_switch_request->ue_security_cap.nr_encryption_algorithms =
      security::supported_algorithms_to_asn1(request.supported_enc_algos);
  asn1_path_switch_request->ue_security_cap.nr_integrity_protection_algorithms =
      security::supported_algorithms_to_asn1(request.supported_int_algos);
  // Fill PDU session resource to be switched list.
  for (const auto& pdu_session_res : request.pdu_session_res_to_be_switched_dl_list) {
    pdu_session_res_to_be_switched_dl_item_s asn1_item;
    asn1_item.pdu_session_id = pdu_session_id_to_uint(pdu_session_res.pdu_session_id);

    asn1::ngap::path_switch_request_transfer_s path_switch_request_transfer;
    // Fill DL NGU UP TNL info.
    up_transport_layer_info_to_asn1(path_switch_request_transfer.dl_ngu_up_tnl_info,
                                    pdu_session_res.dl_ngu_up_tnl_info);
    // Fill DL NGU TNL info reused.
    if (pdu_session_res.dl_ngu_tnl_info_reused) {
      path_switch_request_transfer.dl_ngu_tnl_info_reused_present = true;
      path_switch_request_transfer.dl_ngu_tnl_info_reused =
          asn1::ngap::dl_ngu_tnl_info_reused_opts::options::true_value;
    }
    // Fill User Plane Security Information.
    if (pdu_session_res.user_plane_security_info.has_value()) {
      path_switch_request_transfer.user_plane_security_info_present = true;
      // Fill Security Result.
      path_switch_request_transfer.user_plane_security_info.security_result =
          cu_cp_security_result_to_ngap_security_result(pdu_session_res.user_plane_security_info->security_result);
      // Fill Security Indication.
      security_indication_to_asn1(path_switch_request_transfer.user_plane_security_info.security_ind,
                                  pdu_session_res.user_plane_security_info->security_ind);
    }
    // Fill QoS flow accepted list.
    for (const auto& qos_flow_id : pdu_session_res.qos_flow_accepted_list) {
      qos_flow_accepted_item_s qos_flow_accepted_item;
      // Fill QoS flow ID.
      qos_flow_accepted_item.qos_flow_id = qos_flow_id_to_uint(qos_flow_id);
      path_switch_request_transfer.qos_flow_accepted_list.push_back(qos_flow_accepted_item);
    }

    // Pack path_switch_request_transfer_s.
    byte_buffer pdu = pack_into_pdu(path_switch_request_transfer);

    if (!asn1_item.path_switch_request_transfer.resize(pdu.length())) {
      return false;
    }
    std::copy(pdu.begin(), pdu.end(), asn1_item.path_switch_request_transfer.begin());

    asn1_path_switch_request->pdu_session_res_to_be_switched_dl_list.push_back(asn1_item);
  }
  // Fill PDU session resource failed to setup list.
  for (const auto& failed_item : request.pdu_session_res_failed_to_setup_list_ps_req) {
    if (!std::holds_alternative<ngap_cause_t>(failed_item.cause)) {
      // If cause is not of type ngap_cause_t, skip this item as it's an unsupported cause type for NGAP Path Switch
      // Request message.
      continue;
    }
    pdu_session_res_failed_to_setup_item_ps_req_s asn1_failed_item;
    // Fill PDU session ID.
    asn1_failed_item.pdu_session_id = pdu_session_id_to_uint(failed_item.pdu_session_id);
    // Fill Path Switch Request Setup Failed Transfer.
    path_switch_request_setup_failed_transfer_s asn1_failed_transfer;
    asn1_failed_transfer.cause = cause_to_asn1(std::get<ngap_cause_t>(failed_item.cause));
    // Pack path_switch_request_setup_failed_transfer_s.
    byte_buffer pdu = pack_into_pdu(asn1_failed_transfer);
    if (!asn1_failed_item.path_switch_request_setup_failed_transfer.resize(pdu.length())) {
      return false;
    }
    std::copy(pdu.begin(), pdu.end(), asn1_failed_item.path_switch_request_setup_failed_transfer.begin());

    asn1_path_switch_request->pdu_session_res_failed_to_setup_list_ps_req.push_back(asn1_failed_item);
  }
  // Fill RRC resume cause.
  if (request.rrc_resume_cause.has_value()) {
    asn1_path_switch_request->rrc_resume_cause_present = true;
    asn1_path_switch_request->rrc_resume_cause         = establishment_cause_to_asn1(request.rrc_resume_cause.value());
  }

  // Send Path Switch Request to AMF.
  return amf_notifier.on_new_message(ngap_msg);
}

cu_cp_path_switch_response
ngap_path_switch_procedure::handle_successful_outcome(const asn1::ngap::path_switch_request_ack_s& asn1_response)
{
  // Compare context UE IDs based on UE IDs in the response.
  if (ue_ctxt.ue_ids.ran_ue_id != uint_to_ran_ue_id(asn1_response->ran_ue_ngap_id) ||
      ue_ctxt.ue_ids.amf_ue_id != uint_to_amf_ue_id(asn1_response->amf_ue_ngap_id)) {
    ue_ctxt.logger.log_warning(
        "UE ID mismatch between context and response. Patch Switch Request Ack: ran_ue_id={}, amf_ue_id={}",
        asn1_response->ran_ue_ngap_id,
        asn1_response->amf_ue_ngap_id);
    return cu_cp_path_switch_request_failure{.ue_index = ue_ctxt.ue_ids.ue_index};
  }

  cu_cp_path_switch_request_ack request_ack;
  request_ack.ue_index = ue_ctxt.ue_ids.ue_index;
  if (!asn1_to_path_switch_request_ack(request_ack, asn1_response)) {
    return cu_cp_path_switch_request_failure{.ue_index = ue_ctxt.ue_ids.ue_index};
  }

  // Store Core Network Assist Info for Inactive if present.
  if (request_ack.core_network_assist_info_for_inactive.has_value()) {
    ue_ctxt.core_network_assist_info_for_inactive = request_ack.core_network_assist_info_for_inactive.value();
  }

  // Store RRC inactive transition report request if present.
  if (request_ack.rrc_inactive_transition_report_request.has_value()) {
    ue_ctxt.rrc_inactive_transition_report_request = request_ack.rrc_inactive_transition_report_request.value();
  }

  // Log security context.
  ue_ctxt.logger.log_debug(request_ack.security_context.k.data(), 32, "K_gnb");
  ue_ctxt.logger.log_debug("Supported integrity algorithms: {}", request_ack.security_context.supported_int_algos);
  ue_ctxt.logger.log_debug("Supported ciphering algorithms: {}", request_ack.security_context.supported_enc_algos);

  return request_ack;
}

bool ngap_path_switch_procedure::asn1_to_path_switch_request_ack(
    cu_cp_path_switch_request_ack&               res,
    const asn1::ngap::path_switch_request_ack_s& asn1_response) const
{
  // Convert Security Context.
  security::asn1_to_key(res.security_context.k, asn1_response->security_context.next_hop_nh);
  res.security_context.ncc = asn1_response->security_context.next_hop_chaining_count;
  security::asn1_to_supported_algorithms(res.security_context.supported_int_algos,
                                         asn1_response->ue_security_cap.nr_integrity_protection_algorithms);
  security::asn1_to_supported_algorithms(res.security_context.supported_enc_algos,
                                         asn1_response->ue_security_cap.nr_encryption_algorithms);

  // Convert PDU session resource switched list.
  for (const auto& asn1_switched_item : asn1_response->pdu_session_res_switched_list) {
    cu_cp_pdu_session_res_switched_item switched_item;
    // Convert PDU session ID.
    switched_item.pdu_session_id = uint_to_pdu_session_id(asn1_switched_item.pdu_session_id);

    // Unpack path_switch_request_ack_transfer.
    asn1::cbit_ref                     bref{asn1_switched_item.path_switch_request_ack_transfer};
    path_switch_request_ack_transfer_s path_switch_request_ack_transfer;
    if (path_switch_request_ack_transfer.unpack(bref) != asn1::OCUDUASN_SUCCESS) {
      return false;
    }

    // Convert UL NGU UP TNL info if present.
    if (path_switch_request_ack_transfer.ul_ngu_up_tnl_info_present) {
      switched_item.ul_ngu_up_tnl_info =
          asn1_to_up_transport_layer_info(path_switch_request_ack_transfer.ul_ngu_up_tnl_info);
    }
    // Convert Security Indication if present.
    if (path_switch_request_ack_transfer.security_ind_present) {
      switched_item.security_ind.emplace();
      asn1_to_security_indication(switched_item.security_ind.value(), path_switch_request_ack_transfer.security_ind);
    }

    res.pdu_session_res_switched_list.push_back(switched_item);
  }

  // Convert PDU session resource released list if present.
  if (asn1_response->pdu_session_res_released_list_ps_ack_present) {
    for (const auto& asn1_release_item : asn1_response->pdu_session_res_released_list_ps_ack) {
      cu_cp_pdu_session_res_released_item released_item;
      released_item.pdu_session_id = uint_to_pdu_session_id(asn1_release_item.pdu_session_id);

      // Unpack path_switch_request_unsuccessful_transfer to get the cause of release.
      asn1::cbit_ref                              bref{asn1_release_item.path_switch_request_unsuccessful_transfer};
      path_switch_request_unsuccessful_transfer_s path_switch_request_unsuccessful_transfer;
      if (path_switch_request_unsuccessful_transfer.unpack(bref) != asn1::OCUDUASN_SUCCESS) {
        return false;
      }

      released_item.cause = asn1_to_cause(path_switch_request_unsuccessful_transfer.cause);
      res.pdu_session_res_released_list.push_back(released_item);
    }
  }

  // Convert allowed NSSAI.
  for (const auto& asn1_allowed_nssai : asn1_response->allowed_nssai) {
    res.allowed_nssai.push_back(ngap_asn1_to_s_nssai(asn1_allowed_nssai.s_nssai));
  }

  // Convert Core Network Assist Info for Inactive if present.
  if (asn1_response->core_network_assist_info_for_inactive_present) {
    res.core_network_assist_info_for_inactive =
        asn1_to_core_network_assist_info_for_inactive(asn1_response->core_network_assist_info_for_inactive);
  }

  // Convert RRC inactive transition report request if present.
  if (asn1_response->rrc_inactive_transition_report_request_present) {
    res.rrc_inactive_transition_report_request =
        asn1_to_rrc_inactive_transition_report_request(asn1_response->rrc_inactive_transition_report_request);
  }

  return true;
}

cu_cp_path_switch_request_failure ngap_path_switch_procedure::asn1_to_path_switch_request_failure(
    const asn1::ngap::path_switch_request_fail_s& asn1_fail) const
{
  cu_cp_path_switch_request_failure fail;

  // Compare context UE IDs based on UE IDs in the response.
  if (ue_ctxt.ue_ids.ran_ue_id != uint_to_ran_ue_id(asn1_fail->ran_ue_ngap_id) ||
      ue_ctxt.ue_ids.amf_ue_id != uint_to_amf_ue_id(asn1_fail->amf_ue_ngap_id)) {
    ue_ctxt.logger.log_warning(
        "UE ID mismatch between context and response. Patch Switch Request Failure: ran_ue_id={}, amf_ue_id={}",
        asn1_fail->ran_ue_ngap_id,
        asn1_fail->amf_ue_ngap_id);
    return fail;
  }
  fail.ue_index = ue_ctxt.ue_ids.ue_index;

  // Convert PDU session resource released list if present.
  for (const auto& asn1_release_item : asn1_fail->pdu_session_res_released_list_ps_fail) {
    cu_cp_pdu_session_res_released_item released_item;
    released_item.pdu_session_id = uint_to_pdu_session_id(asn1_release_item.pdu_session_id);

    // Unpack path_switch_request_unsuccessful_transfer to get the cause of release.
    asn1::cbit_ref                              bref{asn1_release_item.path_switch_request_unsuccessful_transfer};
    path_switch_request_unsuccessful_transfer_s path_switch_request_unsuccessful_transfer;
    if (path_switch_request_unsuccessful_transfer.unpack(bref) != asn1::OCUDUASN_SUCCESS) {
      return fail;
    }

    released_item.cause = asn1_to_cause(path_switch_request_unsuccessful_transfer.cause);
    fail.pdu_session_res_released_list.push_back(released_item);
  }

  return fail;
}
