// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/e1ap/common/e1ap_types.h"
#include "ocudu/e1ap/cu_cp/e1ap_cu_cp_bearer_context_update.h"
#include "ocudu/ngap/ngap_pdu_session.h"
#include "ocudu/ran/cu_cp_ue_context_release.h"

namespace ocudu::ocucp {

/// \brief Generate a dummy UE Context Release Command.
/// \param[in] ue_index The UE Index to use.
/// \param[in] cause The cause to use.
/// \return The dummy UE Context Release Command.
cu_cp_ue_context_release_command
generate_ue_context_release_command(cu_cp_ue_index_t ue_index,
                                    ngap_cause_t     cause = ngap_cause_radio_network_t::unspecified);

/// \brief Generate a dummy PDU Session Resource Setup request.
ngap_pdu_session_resource_setup_request
generate_pdu_session_resource_setup(cu_cp_ue_index_t ue_index         = cu_cp_ue_index_t::min,
                                    unsigned         num_pdu_sessions = 1,
                                    unsigned         num_qos_flows    = 1);

/// \brief Generate a dummy PDU Session Resource Setup request with a specific PDU session id and QoS flow id.
ngap_pdu_session_resource_setup_request
generate_pdu_session_resource_setup(cu_cp_ue_index_t ue_index, pdu_session_id_t psi, qos_flow_id_t qfi);

/// \brief Generate a dummy PDU Session Resource Release Command.
ngap_pdu_session_resource_release_command
generate_pdu_session_resource_release(cu_cp_ue_index_t ue_index = cu_cp_ue_index_t::min,
                                      pdu_session_id_t psi      = pdu_session_id_t{1});

/// \brief Generate a dummy PDU Session Resource Modification Request.
ngap_pdu_session_resource_modify_request
generate_pdu_session_resource_modification(cu_cp_ue_index_t ue_index = cu_cp_ue_index_t::min,
                                           unsigned         psi      = 1,
                                           unsigned         qfi      = 2);

/// \brief Generate a dummy PDU Session Resource Modification Request with QoS flow to remove item.
ngap_pdu_session_resource_modify_request
generate_pdu_session_resource_modification_with_qos_flow_removal(qos_flow_id_t flow_id);

/// \brief Generate a dummy E1AP Bearer Context Setup Response.
e1ap_bearer_context_setup_response generate_e1ap_bearer_context_setup_response(gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id,
                                                                               gnb_cu_up_ue_e1ap_id_t cu_up_ue_e1ap_id);

/// \brief Generate a dummy E1AP Bearer Context Modification Response.
e1ap_bearer_context_modification_response
generate_e1ap_bearer_context_modification_response(gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id,
                                                   gnb_cu_up_ue_e1ap_id_t cu_up_ue_e1ap_id);

} // namespace ocudu::ocucp
