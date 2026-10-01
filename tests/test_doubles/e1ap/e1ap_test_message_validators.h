// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/cu_types.h"

namespace ocudu {

struct e1ap_message;
class byte_buffer;

namespace test_helpers {

/// \brief Check if an E1AP message is a valid Bearer Context Setup Request message.
bool is_valid_bearer_context_setup_request(const e1ap_message& msg);

/// \brief Check if an E1AP message is a valid Bearer Context Setup Request message,
/// with a valid security indication.
bool is_valid_security_indication_with_bearer_context_setup_request(const e1ap_message&   msg,
                                                                    pdu_session_id_t      psi,
                                                                    security_indication_t sec_ind);

/// \brief Check if an E1AP message is a valid Bearer Context Modification Request message.
bool is_valid_bearer_context_modification_request(const e1ap_message& msg);

/// \brief Check if an E1AP message is a valid Bearer Context Modification Request message.
bool is_valid_bearer_context_modification_request_with_ue_security_info(const e1ap_message& msg);

/// \brief Check if an E1AP message is a valid Bearer Context Release Command message.
bool is_valid_bearer_context_release_command(const e1ap_message& msg);

/// \brief Check if an E1AP message is a valid E1 Reset message.
bool is_valid_e1_reset(const e1ap_message& msg);

} // namespace test_helpers
} // namespace ocudu
