// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/cu_cp/up_context.h"
#include "ocudu/ngap/ngap_handover.h"
#include "ocudu/xnap/xnap_handover.h"

namespace ocudu::ocucp {

/// \brief Generate an NGAP Handover Preparation Request message based on the provided parameters.
/// \param[in] source_ue_index The UE index of the source UE context.
/// \param[in] target_gnb_id The gNB ID of the target cell.
/// \param[in] target_plmn The PLMN Identity of the target cell.
/// \param[in] target_tac The TAC of the target cell.
/// \param[in] target_nci The NR Cell Identity of the target cell.
/// \param[in] pdu_sessions The PDU sessions of the UE, including their QoS flows and UP context information.
/// \returns The generated NGAP Handover Preparation Request message.
ngap_handover_preparation_request
generate_ngap_handover_preparation_request(cu_cp_ue_index_t                                          source_ue_index,
                                           gnb_id_t                                                  target_gnb_id,
                                           plmn_identity                                             target_plmn,
                                           tac_t                                                     target_tac,
                                           nr_cell_identity                                          target_nci,
                                           const std::map<pdu_session_id_t, up_pdu_session_context>& pdu_sessions);

/// \brief Generate an XNAP Handover Request message based on the provided parameters.
/// \param[in] source_ue_index The UE index of the source UE context.
/// \param[in] target_nr_cgi The NR Cell Global ID of the target cell.
/// \param[in] guami The GUAMI of the UE.
/// \param[in] source_amf_ue_id The AMF UE ID of the source UE context.
/// \param[in] ue_ambr The UE AMBR of the source UE context.
/// \param[in] security_context The security context of the UE.
/// \param[in] pdu_sessions The PDU sessions of the UE, including their QoS flows and UP context information.
/// \param[in] rrc_handover_preparation_information The RRC Handover Preparation Information.
/// \param[in] location_report_cfg The NGAP Location Reporting configuration.
/// \returns The generated XNAP Handover Request message.
xnap_handover_request
generate_xnap_handover_request(cu_cp_ue_index_t                                          source_ue_index,
                               nr_cell_global_id_t                                       target_nr_cgi,
                               guami_t                                                   guami,
                               amf_ue_id_t                                               source_amf_ue_id,
                               aggregate_maximum_bit_rate_t                              ue_ambr,
                               const security::security_context&                         security_context,
                               const std::map<pdu_session_id_t, up_pdu_session_context>& pdu_sessions,
                               const byte_buffer&                            rrc_handover_preparation_information,
                               const std::optional<location_report_request>& location_report_cfg);

} // namespace ocudu::ocucp
