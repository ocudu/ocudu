// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/byte_buffer.h"
#include "ocudu/asn1/xnap/xnap.h"

namespace ocudu {

namespace ocucp {

struct xnap_message;

} // namespace ocucp

namespace test_helpers {

/// \brief Check that two XNAP PDUs have the same type.
bool is_same_pdu_type(const ocucp::xnap_message& lhs, const ocucp::xnap_message& rhs);
bool is_pdu_type(const ocucp::xnap_message& pdu, const asn1::xnap::xnap_elem_procs_o::init_msg_c::types& type);
bool is_pdu_type(const ocucp::xnap_message&                                        pdu,
                 const asn1::xnap::xnap_elem_procs_o::successful_outcome_c::types& type);

bool is_valid_handover_request(const ocucp::xnap_message& msg);

bool is_valid_handover_request_ack(const ocucp::xnap_message& msg);

bool is_valid_handover_cancel(const ocucp::xnap_message& msg);

bool is_valid_sn_status_transfer(const ocucp::xnap_message& msg);

bool is_valid_ue_context_release(const ocucp::xnap_message& msg);

bool is_valid_retrieve_ue_context_request(const ocucp::xnap_message& msg);

bool is_valid_retrieve_ue_context_response(const ocucp::xnap_message& msg);

bool is_valid_retrieve_ue_context_failure(const ocucp::xnap_message& msg);

byte_buffer get_rrc_container(const ocucp::xnap_message& msg);

} // namespace test_helpers
} // namespace ocudu
