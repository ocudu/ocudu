// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "xnap_test_message_validators.h"
#include "ocudu/asn1/xnap/common.h"
#include "ocudu/asn1/xnap/xnap_pdu_contents.h"
#include "ocudu/xnap/xnap_message.h"

using namespace ocudu;

using ocucp::xnap_message;

#define TRUE_OR_RETURN(cond)                                                                                           \
  if (not(cond))                                                                                                       \
    return false;

bool ocudu::test_helpers::is_same_pdu_type(const xnap_message& lhs, const xnap_message& rhs)
{
  if (lhs.pdu.type().value != rhs.pdu.type().value) {
    return false;
  }
  switch (lhs.pdu.type().value) {
    case asn1::xnap::xn_ap_pdu_c::types_opts::init_msg:
      return lhs.pdu.init_msg().value.type().value == rhs.pdu.init_msg().value.type().value;
    case asn1::xnap::xn_ap_pdu_c::types_opts::successful_outcome:
      return lhs.pdu.successful_outcome().value.type().value == rhs.pdu.successful_outcome().value.type().value;
    case asn1::xnap::xn_ap_pdu_c::types_opts::unsuccessful_outcome:
      return lhs.pdu.unsuccessful_outcome().value.type().value == rhs.pdu.unsuccessful_outcome().value.type().value;
    default:
      break;
  }
  return false;
}

bool ocudu::test_helpers::is_pdu_type(const xnap_message&                                     pdu,
                                      const asn1::xnap::xnap_elem_procs_o::init_msg_c::types& type)
{
  if (pdu.pdu.type().value != asn1::xnap::xn_ap_pdu_c::types_opts::init_msg) {
    return false;
  }
  return pdu.pdu.init_msg().value.type().value == type;
}

bool ocudu::test_helpers::is_pdu_type(const xnap_message&                                               pdu,
                                      const asn1::xnap::xnap_elem_procs_o::successful_outcome_c::types& type)
{
  if (pdu.pdu.type().value != asn1::xnap::xn_ap_pdu_c::types_opts::successful_outcome) {
    return false;
  }
  return pdu.pdu.successful_outcome().value.type().value == type;
}

bool ocudu::test_helpers::is_valid_handover_request(const xnap_message& msg)
{
  TRUE_OR_RETURN(msg.pdu.type() == asn1::xnap::xn_ap_pdu_c::types_opts::init_msg);
  TRUE_OR_RETURN(msg.pdu.init_msg().proc_code == ASN1_XNAP_ID_HO_PREP);
  return true;
}

bool ocudu::test_helpers::is_valid_handover_request_ack(const xnap_message& msg)
{
  TRUE_OR_RETURN(msg.pdu.type() == asn1::xnap::xn_ap_pdu_c::types_opts::successful_outcome);
  TRUE_OR_RETURN(msg.pdu.successful_outcome().proc_code == ASN1_XNAP_ID_HO_PREP);
  return true;
}

bool ocudu::test_helpers::is_valid_handover_cancel(const xnap_message& msg)
{
  TRUE_OR_RETURN(msg.pdu.type() == asn1::xnap::xn_ap_pdu_c::types_opts::init_msg);
  TRUE_OR_RETURN(msg.pdu.init_msg().proc_code == ASN1_XNAP_ID_HO_CANCEL);
  return true;
}

bool ocudu::test_helpers::is_valid_sn_status_transfer(const xnap_message& msg)
{
  TRUE_OR_RETURN(msg.pdu.type() == asn1::xnap::xn_ap_pdu_c::types_opts::init_msg);
  TRUE_OR_RETURN(msg.pdu.init_msg().proc_code == ASN1_XNAP_ID_S_N_STATUS_TRANSFER);
  return true;
}

bool ocudu::test_helpers::is_valid_ue_context_release(const xnap_message& msg)
{
  TRUE_OR_RETURN(msg.pdu.type() == asn1::xnap::xn_ap_pdu_c::types_opts::init_msg);
  TRUE_OR_RETURN(msg.pdu.init_msg().proc_code == ASN1_XNAP_ID_U_E_CONTEXT_RELEASE);
  return true;
}

bool ocudu::test_helpers::is_valid_retrieve_ue_context_request(const xnap_message& msg)
{
  TRUE_OR_RETURN(msg.pdu.type() == asn1::xnap::xn_ap_pdu_c::types_opts::init_msg);
  TRUE_OR_RETURN(msg.pdu.init_msg().proc_code == ASN1_XNAP_ID_RETRIEVE_UE_CONTEXT);
  return true;
}

bool ocudu::test_helpers::is_valid_retrieve_ue_context_response(const xnap_message& msg)
{
  TRUE_OR_RETURN(msg.pdu.type() == asn1::xnap::xn_ap_pdu_c::types_opts::successful_outcome);
  TRUE_OR_RETURN(msg.pdu.successful_outcome().proc_code == ASN1_XNAP_ID_RETRIEVE_UE_CONTEXT);
  return true;
}

bool ocudu::test_helpers::is_valid_retrieve_ue_context_failure(const xnap_message& msg)
{
  TRUE_OR_RETURN(msg.pdu.type() == asn1::xnap::xn_ap_pdu_c::types_opts::unsuccessful_outcome);
  TRUE_OR_RETURN(msg.pdu.unsuccessful_outcome().proc_code == ASN1_XNAP_ID_RETRIEVE_UE_CONTEXT);
  return true;
}

byte_buffer ocudu::test_helpers::get_rrc_container(const xnap_message& msg)
{
  if (msg.pdu.init_msg().proc_code == ASN1_XNAP_ID_HO_PREP) {
    return msg.pdu.init_msg().value.ho_request()->ue_context_info_ho_request.rrc_context.copy();
  }

  return byte_buffer{};
}
