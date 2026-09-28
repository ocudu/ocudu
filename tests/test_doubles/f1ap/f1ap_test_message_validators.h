// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/f1ap/f1ap_ue_id_types.h"
#include "ocudu/ran/nr_cgi.h"
#include "ocudu/ran/rb_id.h"
#include "ocudu/ran/rnti.h"
#include <optional>

namespace ocudu {

struct f1ap_message;
class byte_buffer;

namespace odu {
struct gnbdu_config_update_request;
}

namespace test_helpers {

bool is_f1_setup_request_valid(const f1ap_message& msg);

bool is_gnb_du_config_update_valid(const f1ap_message& msg);
bool is_gnb_du_config_update_valid(const f1ap_message& msg, const odu::gnbdu_config_update_request& req);

bool is_gnb_cu_config_update_acknowledge_valid(const f1ap_message& msg);
bool is_gnb_cu_config_update_acknowledge_valid(const f1ap_message& msg, const f1ap_message& req);
bool is_gnb_cu_config_update_failure_valid(const f1ap_message& msg);
bool is_gnb_cu_config_update_failure_valid(const f1ap_message& msg, const f1ap_message& req);

/// \brief Check if an F1AP message is a valid Initial UL RRC Message Transfer message.
bool is_init_ul_rrc_msg_transfer_valid(const f1ap_message&                       msg,
                                       rnti_t                                    rnti,
                                       const std::optional<nr_cell_global_id_t>& nci = {});

bool is_valid_dl_rrc_message_transfer(const f1ap_message& msg);

const byte_buffer& get_rrc_container(const f1ap_message& msg);

byte_buffer get_du_to_cu_container(const f1ap_message& msg);

bool is_valid_dl_rrc_message_transfer_with_msg4(const f1ap_message& msg);

bool is_ul_rrc_msg_transfer_valid(const f1ap_message& msg, srb_id_t srb_id);

// UE Context Setup
bool is_valid_ue_context_setup_request(const f1ap_message& msg, bool cell_group_config_present = false);
bool is_valid_ue_context_setup_request_with_ue_capabilities(const f1ap_message& msg);
bool is_valid_ue_context_setup_response(const f1ap_message& msg);
bool is_valid_ue_context_setup_response(const f1ap_message& resp_msg, const f1ap_message& req_msg);

// UE Context Modification
bool is_valid_ue_context_modification_request(const f1ap_message& msg);
enum class ue_context_mod_context { default_case, reestablistment };
bool is_valid_ue_context_modification_response(const f1ap_message&    msg,
                                               ue_context_mod_context context = ue_context_mod_context::default_case);
bool is_valid_ue_context_modification_response(const f1ap_message&    resp_msg,
                                               const f1ap_message&    req_msg,
                                               ue_context_mod_context context = ue_context_mod_context::default_case);

// UE Context Release
bool is_valid_ue_context_release_request(const f1ap_message& msg);
bool is_valid_ue_context_release_request(const f1ap_message& msg, gnb_du_ue_f1ap_id_t du_ue_id);
bool is_valid_ue_context_release_command(const f1ap_message& msg);
bool is_valid_ue_context_release_complete(const f1ap_message& msg);
bool is_valid_ue_context_release_complete(const f1ap_message& msg, const f1ap_message& rel_cmd);

bool is_valid_paging(const f1ap_message& msg);

bool is_valid_f1_reset_ack(const f1ap_message& msg);
bool is_valid_f1_reset_ack(const f1ap_message& req, const f1ap_message& resp);

bool is_valid_positioning_information_response(const f1ap_message& msg);

bool is_valid_f1ap_trp_information_request(const f1ap_message& msg);

bool is_valid_f1ap_trp_information_response(const f1ap_message& msg);

bool is_valid_f1ap_positioning_information_request(const f1ap_message& msg);

bool is_valid_f1ap_positioning_information_response(const f1ap_message& msg);

bool is_valid_f1ap_positioning_activation_request(const f1ap_message& msg);

bool is_valid_f1ap_positioning_measurement_request(const f1ap_message& msg);

bool is_valid_f1ap_positioning_measurement_response(const f1ap_message& msg);

/// \brief Check if an F1AP message is a valid E-CID MEASUREMENT INITIATION REQUEST, as per TS 38.473,
/// Section 9.2.12.20.
bool is_valid_e_cid_measurement_initiation_request(const f1ap_message& msg);

bool is_valid_f1ap_positioning_measurement_failure(const f1ap_message& msg);

/// \brief Check if an F1AP message is a valid E-CID MEASUREMENT INITIATION RESPONSE.
bool is_valid_f1ap_e_cid_measurement_initiation_response(const f1ap_message& msg);

/// \brief Check if an F1AP message is a valid E-CID MEASUREMENT INITIATION FAILURE.
bool is_valid_f1ap_e_cid_measurement_initiation_failure(const f1ap_message& msg);

bool is_valid_gnb_cu_configuration_update(const f1ap_message& msg);

bool is_valid_gnb_cu_configuration_update_acknowledge(const f1ap_message& msg);

} // namespace test_helpers
} // namespace ocudu
