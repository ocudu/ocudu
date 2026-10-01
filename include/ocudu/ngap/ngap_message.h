// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/asn1/ngap/ngap.h"

namespace ocudu::ocucp {

/// \brief NGAP PDU sent and received from the AMF.
struct ngap_message {
  asn1::ngap::ngap_pdu_c pdu;
};

} // namespace ocudu::ocucp
