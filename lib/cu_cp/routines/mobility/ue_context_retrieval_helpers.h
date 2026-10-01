// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../../ue_manager/ue_manager_impl.h"
#include "ocudu/xnap/xnap_ue_context_retrieval.h"

namespace ocudu::ocucp {

/// \brief Collect the UE context the old NG-RAN node transfers in a Retrieve UE Context Response.
///
/// Implements the old NG-RAN node side of TS 33.501 section 6.11 and TS 38.423 section 8.2.4: the MAC-I is verified
/// against the AS keys the UE has here, KgNB* is derived for the target cell, and the UE context is packed for
/// transfer. The UE is not released here, that only happens once the new NG-RAN node confirms the retrieval.
///
/// \param[in] request The received Retrieve UE Context Request, with the UE index and target cell already resolved.
/// \param[in] ue The UE holding the context.
/// \param[in] guami The GUAMI serving the UE.
/// \param[in] amf_ue_id The AMF UE NGAP ID of the UE.
/// \param[in] amf_addr The address of the SCTP association with the AMF serving the UE.
/// \param[in] target_ssb_arfcn SSB ARFCN of the target cell, decoded from the MeasurementTimingConfiguration the peer
/// advertised for it. Empty when it could not be decoded, in which case KgNB* cannot be derived.
/// \param[in] logger CU-CP logger.
/// \return The response to send to the new NG-RAN node. On failure, \c success is false and \c cause carries the
/// reason.
xnap_retrieve_ue_context_response collect_ue_context_for_retrieval(const xnap_retrieve_ue_context_request& request,
                                                                   cu_cp_ue&                               ue,
                                                                   const guami_t&                          guami,
                                                                   amf_ue_id_t                             amf_ue_id,
                                                                   const transport_layer_address&          amf_addr,
                                                                   std::optional<arfcn_t>  target_ssb_arfcn,
                                                                   ocudulog::basic_logger& logger);

} // namespace ocudu::ocucp
