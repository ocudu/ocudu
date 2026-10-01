// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../../ue_manager/ue_manager_impl.h"
#include "ocudu/cu_cp/ue_configuration.h"
#include "ocudu/e1ap/cu_cp/e1ap_cu_cp_bearer_context_update.h"
#include "ocudu/ngap/ngap.h"
#include "ocudu/ngap/ngap_types.h"

namespace ocudu::ocucp {

/// \brief Fill the request that sets up the bearers of a UE whose context was retrieved from a peer NG-RAN node.
///
/// The PDU sessions come from the retrieval, the keys from the security context the peer derived.
///
/// \return True if the request could be filled, false otherwise.
bool fill_retrieved_context_bearer_setup_request(e1ap_bearer_context_setup_request& request,
                                                 cu_cp_ue&                          ue,
                                                 const up_config_update&            next_config,
                                                 const ue_configuration&            ue_cfg,
                                                 const security_indication_t&       default_security_indication,
                                                 ocudulog::basic_logger&            logger);

/// \brief Fill the request that asks the AMF to move the DL path of a retrieved UE to this node
/// (TS 38.413 section 8.4.4).
/// \param[in] setup_response Response of the bearer context setup, which carries the DL NG-U endpoints the CU-UP
/// allocated and the PDU sessions it rejected.
cu_cp_path_switch_request
fill_retrieved_context_path_switch_request(cu_cp_ue& ue, const e1ap_bearer_context_setup_response& setup_response);

/// \brief Fill the request that informs the CU-UP of the UL NG-U tunnels the AMF returned, and of any PDU session the
/// AMF released.
void fill_retrieved_context_tunnel_update_request(e1ap_bearer_context_modification_request& request,
                                                  cu_cp_ue_index_t                          ue_index,
                                                  const cu_cp_path_switch_request_ack&      ack);

/// \brief Commit the PDU sessions established for a retrieved UE to its UP resource manager.
void apply_retrieved_context_up_config_update(cu_cp_ue& ue, const up_config_update& next_config);

/// \brief Start the location reporting the old NG-RAN node requested for a retrieved UE.
///
/// TS 38.423 section 8.2.4.2: if the Location Reporting Information IE is included in the RETRIEVE UE CONTEXT RESPONSE
/// message, the new NG-RAN node initiates the requested location reporting as defined in TS 38.413.
///
/// \param[in] loc_report_handler NGAP handler that carries the reports to the AMF.
void start_retrieved_context_location_reporting(cu_cp_ue&                        ue,
                                                ngap_location_reporting_handler& loc_report_handler,
                                                ocudulog::basic_logger&          logger);

} // namespace ocudu::ocucp
