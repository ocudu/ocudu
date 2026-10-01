// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/asn1/e1ap/e1ap.h"

namespace ocudu {

/// \brief E1AP message transferred between a CU-CP and a CU-UP.
struct e1ap_message {
  asn1::e1ap::e1ap_pdu_c pdu;
};

} // namespace ocudu
