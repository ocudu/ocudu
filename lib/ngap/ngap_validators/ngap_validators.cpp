// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ngap_validators.h"
#include "ocudu/ran/cause/common.h"
#include "ocudu/ran/qos/five_qi_qos_mapping.h"
#include <unordered_set>

using namespace ocudu;
using namespace ocucp;

/// \brief Determine whether a QoS flow is a GBR QoS flow, as per TS 23.501 section 5.7.3.2.
///
/// For a standardized 5QI, the resource type is given by TS 23.501 table 5.7.4-1. For a dynamic 5QI, the Delay Critical
/// and Averaging Window IEs are present for GBR QoS flows only, see TS 38.413 section 9.3.1.18.
static bool is_gbr_qos_flow(const qos_flow_level_qos_parameters& qos_params)
{
  if (qos_params.qos_desc.is_dyn_5qi()) {
    const dyn_5qi_descriptor& dyn_5qi = qos_params.qos_desc.get_dyn_5qi();
    return dyn_5qi.is_delay_critical.has_value() or dyn_5qi.averaging_win.has_value();
  }

  const standardized_qos_characteristics* qos_char =
      get_5qi_to_qos_characteristics_mapping(qos_params.qos_desc.get_nondyn_5qi().five_qi);
  return qos_char != nullptr and qos_char->res_type != qos_flow_resource_type::non_gbr;
}

/// \brief Determine whether the companion IEs required for the QoS characteristics of a QoS flow are present, as per
/// TS 38.413 section 8.2.1.4.
static bool has_required_qos_companion_ies(const qos_flow_level_qos_parameters& qos_params)
{
  // The GBR QoS Flow Information IE shall be present for GBR QoS flows.
  if (is_gbr_qos_flow(qos_params) and not qos_params.gbr_qos_info.has_value()) {
    return false;
  }

  // The Maximum Data Burst Volume IE shall be present if the Delay Critical IE is set to "delay critical".
  if (qos_params.qos_desc.is_dyn_5qi()) {
    const dyn_5qi_descriptor& dyn_5qi = qos_params.qos_desc.get_dyn_5qi();
    if (dyn_5qi.is_delay_critical.value_or(false) and not dyn_5qi.max_data_burst_volume.has_value()) {
      return false;
    }
  }

  return true;
}

pdu_session_resource_setup_validation_outcome
ocudu::ocucp::verify_pdu_session_resource_setup_request(const ngap_pdu_session_resource_setup_request&     request,
                                                        const asn1::ngap::pdu_session_res_setup_request_s& asn1_request,
                                                        const ngap_ue_logger&                              ue_logger)
{
  pdu_session_resource_setup_validation_outcome verification_outcome;

  std::unordered_set<pdu_session_id_t> psis;
  std::unordered_set<pdu_session_id_t> failed_psis;
  for (const auto& asn1_pdu_session_item : asn1_request->pdu_session_res_setup_list_su_req) {
    pdu_session_id_t psi = uint_to_pdu_session_id(asn1_pdu_session_item.pdu_session_id);
    // Check for duplicate PDU Session IDs.
    if (!psis.emplace(psi).second) {
      ue_logger.log_warning("Duplicate {} in PduSessionResourceSetupRequest", psi);
      // Make sure to only add each duplicate psi once.
      if (failed_psis.emplace(psi).second) {
        // Add failed psi to response.
        ngap_pdu_session_res_setup_failed_item failed_item;
        failed_item.pdu_session_id              = psi;
        failed_item.unsuccessful_transfer.cause = ngap_cause_radio_network_t::multiple_pdu_session_id_instances;
        verification_outcome.response.pdu_session_res_failed_to_setup_items.emplace(psi, failed_item);
      }
    }
  }

  // Check for unsupported PDU Session Types.
  for (const auto& pdu_session_item : request.pdu_session_res_setup_items) {
    if (pdu_session_item.pdu_session_type == pdu_session_type_t::ipv4v6) {
      ue_logger.log_warning("Unsupported PDU Session Type: {}", pdu_session_item.pdu_session_type);
      failed_psis.emplace(pdu_session_item.pdu_session_id);
      // Add failed psi to response.
      ngap_pdu_session_res_setup_failed_item failed_item;
      failed_item.pdu_session_id              = pdu_session_item.pdu_session_id;
      failed_item.unsuccessful_transfer.cause = cause_protocol_t::unspecified;
      verification_outcome.response.pdu_session_res_failed_to_setup_items.emplace(pdu_session_item.pdu_session_id,
                                                                                  failed_item);
    }
  }

  // Remove failed psis from psis.
  for (const auto& failed_psi : failed_psis) {
    psis.erase(failed_psi);
  }

  // If only duplicate PDU session IDs are present, return.
  if (psis.empty()) {
    return verification_outcome;
  }

  // Add a PDU session to the response as failed, with a cause reporting an invalid QoS combination.
  auto fail_pdu_session = [&](pdu_session_id_t psi) {
    failed_psis.emplace(psi);
    ngap_pdu_session_res_setup_failed_item failed_item;
    failed_item.pdu_session_id              = psi;
    failed_item.unsuccessful_transfer.cause = ngap_cause_radio_network_t::invalid_qos_combination;
    verification_outcome.response.pdu_session_res_failed_to_setup_items.emplace(psi, failed_item);
  };

  // Check that the IEs required by the QoS parameters of the requested QoS flows are present.
  for (const auto& psi : psis) {
    const auto& setup_item = request.pdu_session_res_setup_items[psi];

    // If a Non-GBR QoS flow is requested then the PDU Session Aggregate Maximum Bit Rate must be present.
    bool non_gbr_qos_flow_requested = std::any_of(setup_item.qos_flow_setup_request_items.begin(),
                                                  setup_item.qos_flow_setup_request_items.end(),
                                                  [](const qos_flow_setup_request_item& qos_flow_item) {
                                                    return not is_gbr_qos_flow(qos_flow_item.qos_flow_level_qos_params);
                                                  });
    if (non_gbr_qos_flow_requested and not setup_item.pdu_session_aggregate_maximum_bit_rate_dl.has_value()) {
      ue_logger.log_warning("Non-GBR QoS flow for {} present but PduSessionAggregateMaximumBitRate not set", psi);
      // If the PDU Session Aggregate Maximum Bit Rate is missing, then the whole PDU session fails.
      fail_pdu_session(psi);
      continue;
    }

    // Collect the QoS flows whose QoS parameters lack a required IE.
    ngap_pdu_session_res_setup_response_item response_item;
    response_item.pdu_session_id = psi;
    auto& failed_qos_flows = response_item.pdu_session_resource_setup_response_transfer.qos_flow_failed_to_setup_list;
    for (const auto& qos_flow_item : setup_item.qos_flow_setup_request_items) {
      if (has_required_qos_companion_ies(qos_flow_item.qos_flow_level_qos_params)) {
        continue;
      }
      ue_logger.log_warning("Incomplete QoS parameters for {} of {}", qos_flow_item.qos_flow_id, psi);
      ngap_qos_flow_failed_to_setup_item failed_qos_flow;
      failed_qos_flow.qos_flow_id = qos_flow_item.qos_flow_id;
      failed_qos_flow.cause       = ngap_cause_radio_network_t::invalid_qos_combination;
      failed_qos_flows.emplace(failed_qos_flow.qos_flow_id, failed_qos_flow);
    }

    if (failed_qos_flows.empty()) {
      continue;
    }
    if (failed_qos_flows.size() == setup_item.qos_flow_setup_request_items.size()) {
      // If all requested QoS flows fail, then the whole PDU session fails.
      fail_pdu_session(psi);
      continue;
    }
    // The remaining QoS flows of the PDU session are set up, so report the failed ones in the response.
    verification_outcome.response.pdu_session_res_setup_response_items.emplace(psi, std::move(response_item));
  }

  // Remove failed psis from psis.
  for (const auto& failed_psi : failed_psis) {
    psis.erase(failed_psi);
  }

  // Add remaining PDU sessions to verified request, leaving out the QoS flows that failed the verification.
  for (const auto& psi : psis) {
    auto& setup_item =
        verification_outcome.request.pdu_session_res_setup_items.emplace(psi, request.pdu_session_res_setup_items[psi]);
    if (verification_outcome.response.pdu_session_res_setup_response_items.contains(psi)) {
      for (const auto& failed_qos_flow :
           verification_outcome.response.pdu_session_res_setup_response_items[psi]
               .pdu_session_resource_setup_response_transfer.qos_flow_failed_to_setup_list) {
        setup_item.qos_flow_setup_request_items.erase(failed_qos_flow.qos_flow_id);
      }
    }
  }
  verification_outcome.request.ue_index     = request.ue_index;
  verification_outcome.request.ue_ambr      = request.ue_ambr;
  verification_outcome.request.serving_plmn = request.serving_plmn;

  return verification_outcome;
}

pdu_session_resource_modify_validation_outcome ocudu::ocucp::verify_pdu_session_resource_modify_request(
    const ngap_pdu_session_resource_modify_request&     request,
    const asn1::ngap::pdu_session_res_modify_request_s& asn1_request,
    const ngap_ue_logger&                               ue_logger)
{
  pdu_session_resource_modify_validation_outcome verification_outcome;

  std::unordered_set<pdu_session_id_t> psis;
  std::unordered_set<pdu_session_id_t> failed_psis;
  for (const auto& pdu_session_item : asn1_request->pdu_session_res_modify_list_mod_req) {
    pdu_session_id_t psi = uint_to_pdu_session_id(pdu_session_item.pdu_session_id);
    // Check for duplicate PDU Session IDs.
    if (!psis.emplace(psi).second) {
      ue_logger.log_warning("Duplicate {} in PduSessionResourceModifyRequest", psi);
      // Make sure to only add each duplicate psi once.
      if (failed_psis.emplace(psi).second) {
        // Add failed psi to response.
        ngap_pdu_session_res_setup_failed_item failed_item;
        failed_item.pdu_session_id              = psi;
        failed_item.unsuccessful_transfer.cause = ngap_cause_radio_network_t::multiple_pdu_session_id_instances;
        verification_outcome.response.pdu_session_res_failed_to_modify_list.emplace(psi, failed_item);
      }
    }
  }

  // Remove failed psis from psis.
  for (const auto& failed_psi : failed_psis) {
    psis.erase(failed_psi);
  }

  // Add remaining PDU sessions to verified request.
  for (const auto& psi : psis) {
    verification_outcome.request.pdu_session_res_modify_items.emplace(psi, request.pdu_session_res_modify_items[psi]);
  }
  verification_outcome.request.ue_index = request.ue_index;

  return verification_outcome;
}
