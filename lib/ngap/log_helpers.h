// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/asn1/ngap/ngap.h"
#include "ocudu/ran/cu_cp_types.h"

namespace ocudu::ocucp {

/// \brief Log Received/Transmitted NGAP PDU.
void log_ngap_pdu(ocudulog::basic_logger&                logger,
                  bool                                   json_log,
                  bool                                   is_rx,
                  const std::optional<cu_cp_ue_index_t>& ue_idx,
                  const asn1::ngap::ngap_pdu_c&          pdu);

} // namespace ocudu::ocucp
