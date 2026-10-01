// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/asn1/xnap/xnap.h"

namespace ocudu::ocucp {

/// \brief Log Received/Transmitted NGAP PDU.
void log_xnap_pdu(ocudulog::basic_logger& logger, bool json_log, bool is_rx, const asn1::xnap::xn_ap_pdu_c& pdu);

} // namespace ocudu::ocucp
