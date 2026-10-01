// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/asn1/f1ap/f1ap.h"
#include "ocudu/f1ap/cu_cp/du_setup_notifier.h"
#include "ocudu/f1ap/f1ap_message_notifier.h"

namespace ocudu {
namespace ocucp {

struct f1ap_du_context;

/// \brief Converts the gNB-DU Configuration Update from ASN.1 to a request to be sent to the CU-CP.
///
/// A cell whose ASN.1 information the CU-CP cannot read is left out of the request.
/// \param[in] asn1_request The ASN.1 type gNB-DU Configuration Update.
/// \param[in] du_ctxt The context of the DU that sent the update.
/// \param[in] logger The logger.
/// \return The request to update the DU configuration.
du_config_update_request create_du_config_update_request(const asn1::f1ap::gnb_du_cfg_upd_s& asn1_request,
                                                         const f1ap_du_context&              du_ctxt,
                                                         ocudulog::basic_logger&             logger);

/// \brief Handles the gNB-DU Configuration Update from the DU as per TS 38.473, Section 8.2.4.
///
/// Passes the updated configuration to the CU-CP and sends the acknowledge or the failure back to the DU.
/// \param[in] request The gNB-DU Configuration Update.
/// \param[out] du_ctxt The DU context, which takes the identities the update carries.
/// \param[in] pdu_notifier The notifier to send F1AP messages to the DU.
/// \param[in] du_setup_notif The notifier to send the update to the CU-CP.
/// \param[in] logger The logger.
void handle_du_config_update_procedure(const asn1::f1ap::gnb_du_cfg_upd_s& request,
                                       f1ap_du_context&                    du_ctxt,
                                       f1ap_message_notifier&              pdu_notifier,
                                       du_setup_notifier&                  du_setup_notif,
                                       ocudulog::basic_logger&             logger);

} // namespace ocucp
} // namespace ocudu
