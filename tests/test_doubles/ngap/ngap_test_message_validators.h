// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/byte_buffer.h"
#include "ocudu/ran/cu_types.h"

namespace ocudu {

namespace ocucp {

struct ngap_message;

} // namespace ocucp

namespace test_helpers {

bool is_valid_init_ue_message(const ocucp::ngap_message& msg);

bool is_valid_ul_nas_transport_message(const ocucp::ngap_message& msg);

bool is_valid_initial_context_setup_response(const ocucp::ngap_message& msg);

bool is_valid_initial_context_setup_failure(const ocucp::ngap_message& msg);

bool is_valid_ue_context_release_request(const ocucp::ngap_message& msg);

bool is_valid_ue_radio_capability_info_indication(const ocucp::ngap_message& msg);

bool is_valid_ue_context_release_complete(const ocucp::ngap_message& msg);

bool is_valid_pdu_session_resource_setup_response(const ocucp::ngap_message& msg);

bool is_valid_pdu_session_resource_release_response(const ocucp::ngap_message& msg);

bool is_valid_pdu_session_resource_modify_response(const ocucp::ngap_message& msg);

bool is_valid_error_indication(const ocucp::ngap_message& msg);

bool is_valid_handover_request_ack(const ocucp::ngap_message& msg);

bool is_valid_handover_notify(const ocucp::ngap_message& msg);

bool is_valid_path_switch_request(const ocucp::ngap_message& msg);

bool is_valid_handover_required(const ocucp::ngap_message& msg);

bool is_valid_handover_cancel(const ocucp::ngap_message& msg);

bool is_valid_ul_ran_status_transfer(const ocucp::ngap_message& msg);

bool is_valid_ng_reset(const ocucp::ngap_message& msg);

bool is_valid_location_report(const ocucp::ngap_message& msg);

bool is_valid_location_reporting_failure_indication(const ocucp::ngap_message& msg);

bool is_valid_ul_ue_associated_nrppa_transport(const ocucp::ngap_message& msg);

bool is_valid_ul_non_ue_associated_nrppa_transport(const ocucp::ngap_message& msg);

// Check if the NGAP PDU contains the expected PDU session setup response.
bool is_expected_pdu_session_resource_setup_response(
    const ocucp::ngap_message&           ngap_pdu,
    const std::vector<pdu_session_id_t>& expected_pdu_sessions_to_setup,
    const std::vector<pdu_session_id_t>& expected_pdu_sessions_failed_to_setup);

byte_buffer get_rrc_container(const ocucp::ngap_message& msg);

} // namespace test_helpers
} // namespace ocudu
