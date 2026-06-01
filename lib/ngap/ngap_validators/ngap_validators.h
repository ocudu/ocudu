// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../ue_context/ngap_ue_logger.h"
#include "ocudu/asn1/ngap/ngap_pdu_contents.h"
#include "ocudu/ngap/ngap_pdu_session.h"

namespace ocudu::ocucp {

struct pdu_session_resource_setup_validation_outcome {
  ngap_pdu_session_resource_setup_request  request;
  ngap_pdu_session_resource_setup_response response;
};

/// \brief Verify a received PduSessionResourceSetupRequest message as per TS 38.413 section 8.2.1.4.
/// \param[in] request The common type PduSessionResourceSetupRequest message.
/// \param[in] asn1_request The received PduSessionResourceSetupRequest message.
/// \param[in] logger The NGAP UE logger.
/// \returns the verified PduSessionResourceSetupRequest and PduSessionResourceSetupResponse messages. The
/// verified request will only contain PDU sessions that passed the verification. The verified response will contain PDU
/// sessions that failed the verification.
pdu_session_resource_setup_validation_outcome
verify_pdu_session_resource_setup_request(const ngap_pdu_session_resource_setup_request&     request,
                                          const asn1::ngap::pdu_session_res_setup_request_s& asn1_request,
                                          const ngap_ue_logger&                              logger);

struct pdu_session_resource_modify_validation_outcome {
  ngap_pdu_session_resource_modify_request  request;
  ngap_pdu_session_resource_modify_response response;
};

/// \brief Verify a received PduSessionResourceModifyRequest message as per TS 38.413 section 8.2.3.4.
/// \param[in] request The common type PduSessionResourceModifyRequest message.
/// \param[in] asn1_request The received PduSessionResourceModifyRequest message.
/// \param[in] logger The NGAP UE logger.
/// \returnsthe verified PduSessionResourceModifyRequest and PduSessionResourceModifyResponse messages. The
/// verified request will only contain PDU sessions that passed the verification. The verified response will contain PDU
/// sessions that failed the verification.
pdu_session_resource_modify_validation_outcome
verify_pdu_session_resource_modify_request(const ngap_pdu_session_resource_modify_request&     request,
                                           const asn1::ngap::pdu_session_res_modify_request_s& asn1_request,
                                           const ngap_ue_logger&                               ue_logger);

} // namespace ocudu::ocucp
