// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/e1ap/common/e1_setup_messages.h"
#include "ocudu/e1ap/common/e1ap_message.h"
#include "ocudu/e1ap/common/e1ap_types.h"

namespace ocudu::ocuup {

/// \brief Generate a dummy CU-UP E1 Setup Request.
cu_up_e1_setup_request generate_cu_up_e1_setup_request();

/// \brief Generate a dummy CU-UP E1 Setup Response.
e1ap_message generate_cu_up_e1_setup_response(unsigned transaction_id);

/// \brief Generate a dummy bearer context setup request.
e1ap_message generate_bearer_context_setup_request(unsigned cu_cp_ue_e1ap_id);

/// \brief Generate an invalid dummy bearer context setup request.
e1ap_message generate_invalid_bearer_context_setup_request(unsigned cu_cp_ue_e1ap_id);

/// \brief Generate a dummy bearer context setup request, with an invalid inactivity timer.
e1ap_message generate_invalid_bearer_context_setup_request_inactivity_timer(unsigned cu_cp_ue_e1ap_id);

/// \brief Generate a dummy bearer context modification request.
e1ap_message generate_bearer_context_modification_request(unsigned cu_cp_ue_e1ap_id, unsigned cu_up_ue_e1ap_id);

/// \brief Generate a bearer context modification request that sets ng_ul_up_tnl_info on the PDU session to be modified.
/// Used to test the Xn path switch N3 tunnel update path.
e1ap_message generate_bearer_context_modification_request_with_ng_ul_tnl(unsigned    cu_cp_ue_e1ap_id,
                                                                         uint32_t    upf_teid,
                                                                         const char* upf_addr);

/// \brief Generate an invalid dummy bearer context modification request.
e1ap_message generate_invalid_bearer_context_modification_request(unsigned cu_cp_ue_e1ap_id, unsigned cu_up_ue_e1ap_id);

/// \brief Generate a dummy bearer context release command.
e1ap_message generate_bearer_context_release_command(unsigned cu_cp_ue_e1ap_id, unsigned cu_up_ue_e1ap_id);

/// \brief Generate a dummy E1 Reset message.
e1ap_message generate_e1_reset(const std::vector<std::pair<gnb_cu_cp_ue_e1ap_id_t, gnb_cu_up_ue_e1ap_id_t>>& ues);

} // namespace ocudu::ocuup
