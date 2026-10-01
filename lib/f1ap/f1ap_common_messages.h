// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/asn1/f1ap/f1ap_ies.h"
#include "ocudu/f1ap/f1ap_message.h"
#include "ocudu/f1ap/f1ap_ue_id_types.h"

namespace ocudu {

/// Generate an F1AP Error Indication message.
f1ap_message generate_error_indication(uint8_t                                   transaction_id,
                                       const std::optional<gnb_du_ue_f1ap_id_t>& du_ue_id,
                                       const std::optional<gnb_cu_ue_f1ap_id_t>& cu_ue_id,
                                       const std::optional<asn1::f1ap::cause_c>& cause);

} // namespace ocudu
