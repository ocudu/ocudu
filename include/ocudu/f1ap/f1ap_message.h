// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/asn1/f1ap/f1ap.h"

namespace ocudu {

/// F1AP message definition which is transferred between a CU-CP and a DU.
struct f1ap_message {
  asn1::f1ap::f1ap_pdu_c pdu;
};

} // namespace ocudu
