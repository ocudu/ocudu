// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/asn1/ngap/ngap.h"
#include "ocudu/ngap/ngap_types.h"
#include "ocudu/ran/cu_types.h"

namespace asn1::ngap {

struct cause_c;
struct init_msg_s;
struct successful_outcome_s;
struct unsuccessful_outcome_s;
struct ngap_pdu_c;

} // namespace asn1::ngap

namespace ocudu::ocucp::asn1_utils {

/// Get string with NGAP error cause.
const char* get_cause_str(const asn1::ngap::cause_c& cause);

/// Extracts message type.
const char* get_message_type_str(const asn1::ngap::ngap_pdu_c& pdu);

/// Extracts RAN-UE-NGAP-ID from NGAP PDU
std::optional<ran_ue_id_t> get_ran_ue_id(const asn1::ngap::init_msg_s& init_msg);
std::optional<ran_ue_id_t> get_ran_ue_id(const asn1::ngap::successful_outcome_s& success_outcome);
std::optional<ran_ue_id_t> get_ran_ue_id(const asn1::ngap::unsuccessful_outcome_s& unsuccessful_outcome);
std::optional<ran_ue_id_t> get_ran_ue_id(const asn1::ngap::ngap_pdu_c& pdu);

std::optional<amf_ue_id_t> get_amf_ue_id(const asn1::ngap::init_msg_s& init_msg);
std::optional<amf_ue_id_t> get_amf_ue_id(const asn1::ngap::successful_outcome_s& success_outcome);
std::optional<amf_ue_id_t> get_amf_ue_id(const asn1::ngap::unsuccessful_outcome_s& unsuccessful_outcome);
std::optional<amf_ue_id_t> get_amf_ue_id(const asn1::ngap::ngap_pdu_c& pdu);

} // namespace ocudu::ocucp::asn1_utils
