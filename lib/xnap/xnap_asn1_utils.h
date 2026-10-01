// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ue_context/xnap_ue_context.h"
#include "ocudu/asn1/xnap/xnap.h"
#include "ocudu/xnap/xnap_types.h"

namespace asn1::xnap {

struct cause_c;

}

namespace ocudu::ocucp::asn1_utils {

/// Get string with XNAP error cause.
const char* get_cause_str(const asn1::xnap::cause_c& cause);

/// Extracts message type.
const char* get_message_type_str(const asn1::xnap::xn_ap_pdu_c& pdu);

// Extracts LOCAL/PEER XNAP UE ID from XNAP PDU.
std::optional<local_xnap_ue_id_t> get_local_xnap_ue_id(const asn1::xnap::successful_outcome_s& success_outcome);
std::optional<local_xnap_ue_id_t> get_local_xnap_ue_id(const asn1::xnap::unsuccessful_outcome_s& unsuccessful_outcome);

// Find UE context based on LOCAL/PEER XNAP UE ID in the given successful/unsuccessful outcome message, and return
// pointer to it.
xnap_ue_context* get_ue_ctxt_in_ue_assoc_msg(const asn1::xnap::successful_outcome_s& successful_outcome,
                                             xnap_ue_context_list&                   ue_ctxts,
                                             ocudulog::basic_logger&                 logger);
xnap_ue_context* get_ue_ctxt_in_ue_assoc_msg(const asn1::xnap::unsuccessful_outcome_s& unsuccessful_outcome,
                                             xnap_ue_context_list&                     ue_ctxts,
                                             ocudulog::basic_logger&                   logger);

} // namespace ocudu::ocucp::asn1_utils
