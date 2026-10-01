// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "xnap_asn1_utils.h"
#include "ocudu/asn1/xnap/xnap.h"
#include "ocudu/asn1/xnap/xnap_ies.h"
#include "ocudu/asn1/xnap/xnap_pdu_contents.h"
#include "ocudu/xnap/xnap_types.h"

using namespace ocudu;
using namespace ocudu::ocucp;
using namespace asn1::xnap;

const char* ocudu::ocucp::asn1_utils::get_cause_str(const asn1::xnap::cause_c& cause)
{
  using namespace asn1::xnap;
  switch (cause.type()) {
    case cause_c::types_opts::radio_network:
      return cause.radio_network().to_string();
    case cause_c::types_opts::transport:
      return cause.transport().to_string();
    case cause_c::types_opts::protocol:
      return cause.protocol().to_string();
    case cause_c::types_opts::misc:
      return cause.misc().to_string();
    default:
      break;
  }
  return "unknown";
}

const char* ocudu::ocucp::asn1_utils::get_message_type_str(const asn1::xnap::xn_ap_pdu_c& pdu)
{
  switch (pdu.type().value) {
    case xn_ap_pdu_c::types_opts::init_msg:
      return pdu.init_msg().value.type().to_string();
    case xn_ap_pdu_c::types_opts::successful_outcome:
      return pdu.successful_outcome().value.type().to_string();
    case xn_ap_pdu_c::types_opts::unsuccessful_outcome:
      return pdu.unsuccessful_outcome().value.type().to_string();
    default:
      break;
  }
  report_fatal_error("Invalid XNAP PDU type \"{}\"", pdu.type().to_string());
}

std::optional<local_xnap_ue_id_t>
ocudu::ocucp::asn1_utils::get_local_xnap_ue_id(const asn1::xnap::successful_outcome_s& success_outcome)
{
  using namespace asn1::xnap;
  using success_types = xnap_elem_procs_o::successful_outcome_c::types_opts;

  switch (success_outcome.value.type()) {
    case success_types::ho_request_ack:
      return uint_to_local_xnap_ue_id(success_outcome.value.ho_request_ack()->source_ng_ra_nnode_ue_xn_ap_id);
    case success_types::retrieve_ue_context_resp:
      // The Retrieve UE Context Response is sent from the source to the target, and it is the target that initiated
      // the procedure, so the local UE ID is the new NG-RAN node UE XnAP ID.
      return uint_to_local_xnap_ue_id(success_outcome.value.retrieve_ue_context_resp()->new_ng_ra_nnode_ue_xn_ap_id);
    default:
      break;
  }

  return std::nullopt;
}

std::optional<local_xnap_ue_id_t>
ocudu::ocucp::asn1_utils::get_local_xnap_ue_id(const asn1::xnap::unsuccessful_outcome_s& unsuccessful_outcome)
{
  using namespace asn1::xnap;
  using unsuccess_types = xnap_elem_procs_o::unsuccessful_outcome_c::types_opts;

  switch (unsuccessful_outcome.value.type()) {
    case unsuccess_types::ho_prep_fail:
      return uint_to_local_xnap_ue_id(unsuccessful_outcome.value.ho_prep_fail()->source_ng_ra_nnode_ue_xn_ap_id);
    case unsuccess_types::retrieve_ue_context_fail:
      // See the note on the Retrieve UE Context Response above: the local UE ID is the new NG-RAN node UE XnAP ID.
      return uint_to_local_xnap_ue_id(
          unsuccessful_outcome.value.retrieve_ue_context_fail()->new_ng_ra_nnode_ue_xn_ap_id);
    default:
      break;
  }

  return std::nullopt;
}

xnap_ue_context*
ocudu::ocucp::asn1_utils::get_ue_ctxt_in_ue_assoc_msg(const asn1::xnap::successful_outcome_s& successful_outcome,
                                                      xnap_ue_context_list&                   ue_ctxts,
                                                      ocudulog::basic_logger&                 logger)
{
  std::optional<local_xnap_ue_id_t> local_xnap_ue_id = asn1_utils::get_local_xnap_ue_id(successful_outcome);
  // The Source NG-RAN node UE XnAP ID field is mandatory in all UE associated successful messages.
  if (!local_xnap_ue_id.has_value()) {
    logger.warning("Discarding get_local_xnap_ue_id \"{}\". Cause: Source NG-RAN node UE XnAP ID field is mandatory",
                   successful_outcome.value.type().to_string());
    return nullptr;
  }

  xnap_ue_context* ue_ctxt = ue_ctxts.find(*local_xnap_ue_id);
  if (ue_ctxt == nullptr) {
    logger.warning("local_xnap_ue={}: Discarding received \"{}\". Cause: UE was not found",
                   *local_xnap_ue_id,
                   successful_outcome.value.type().to_string());
    return nullptr;
  }
  return ue_ctxt;
}

xnap_ue_context*
ocudu::ocucp::asn1_utils::get_ue_ctxt_in_ue_assoc_msg(const asn1::xnap::unsuccessful_outcome_s& unsuccessful_outcome,
                                                      xnap_ue_context_list&                     ue_ctxts,
                                                      ocudulog::basic_logger&                   logger)
{
  std::optional<local_xnap_ue_id_t> local_xnap_ue_id = asn1_utils::get_local_xnap_ue_id(unsuccessful_outcome);
  // The Source NG-RAN node UE XnAP ID field is mandatory in all UE associated unsuccessful messages.
  if (!local_xnap_ue_id.has_value()) {
    logger.warning("Discarding received \"{}\". Cause: Source NG-RAN node UE XnAP ID field is mandatory",
                   unsuccessful_outcome.value.type().to_string());
    return nullptr;
  }

  xnap_ue_context* ue_ctxt = ue_ctxts.find(*local_xnap_ue_id);
  if (ue_ctxt == nullptr) {
    logger.warning("local_xnap_ue={}: Discarding received \"{}\". Cause: UE was not found",
                   *local_xnap_ue_id,
                   unsuccessful_outcome.value.type().to_string());
    return nullptr;
  }
  return ue_ctxt;
}
