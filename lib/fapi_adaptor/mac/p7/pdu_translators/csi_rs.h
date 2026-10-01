// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/fapi/p7/builders/dl_tti_request_builder.h"
#include "ocudu/scheduler/result/csi_rs_info.h"

namespace ocudu {
namespace fapi_adaptor {

/// \brief Helper function that converts from a CSI-RS MAC PDU to a CSI-RS FAPI PDU.
///
/// \param[out] builder   DL_TTI.Request builder where to add the CSI-RSI PDU.
/// \param[in] csi_rs_pdu CSI-RS MAC PDU to convert to CSI-RS FAPI PDU.
void convert_csi_rs_mac_to_fapi(fapi::dl_tti_request_builder& builder, const csi_rs_info& csi_rs_pdu);

} // namespace fapi_adaptor
} // namespace ocudu
