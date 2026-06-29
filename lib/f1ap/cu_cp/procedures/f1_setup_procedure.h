// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/asn1/f1ap/f1ap.h"
#include "ocudu/f1ap/cu_cp/du_setup_notifier.h"
#include "ocudu/f1ap/f1ap_message_notifier.h"

namespace ocudu {
namespace ocucp {

struct f1ap_du_context;

/// \brief Convert the F1 Setup Request from ASN.1 to a request to be sent to the CU-CP.
/// \param[in] asn1_request The ASN.1 type F1 Setup Request.
/// \return Request to setup DU if the ASN.1 request is valid, std::nullopt otherwise.
std::optional<du_setup_request> create_du_setup_request(const asn1::f1ap::f1_setup_request_s& asn1_request);

/// \brief Handle the F1 Setup Request from the DU as per TS 38.473 section 8.2.
///
/// This function will ensure the input message is valid, request the CU-CP to setup the DU and send the F1 Setup
/// Response/Failure back to the DU via the F1-C interface.
/// \param[in] request The F1 Setup Request.
/// \param[out] du_ctxt The DU context to store the DU information.
/// \param[in] pdu_notifier The notifier to send F1AP messages to the DU.
/// \param[in] du_setup_notif The notifier to send the DU setup request to the CU-CP.
/// \param[in] logger The logger.
/// \return True if the DU was accepted; false if the setup was rejected.
bool handle_f1_setup_procedure(const asn1::f1ap::f1_setup_request_s& request,
                               f1ap_du_context&                      du_ctxt,
                               f1ap_message_notifier&                pdu_notifier,
                               du_setup_notifier&                    du_setup_notif,
                               ocudulog::basic_logger&               logger);

} // namespace ocucp
} // namespace ocudu
