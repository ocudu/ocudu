// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/fapi/p7/builders/ul_prach_pdu_builder.h"

namespace ocudu {

struct prach_occasion_info;

namespace fapi_adaptor {

/// \brief Helper function that converts from a PRACH MAC PDU to a PRACH FAPI PDU.
///
/// \param[out] builder PRACH FAPI builder that helps to fill the PDU.
/// \param[in] mac_pdu MAC PDU that contains the PRACH parameters.
void convert_prach_mac_to_fapi(fapi::ul_prach_pdu_builder& builder, const prach_occasion_info& mac_pdu);

} // namespace fapi_adaptor
} // namespace ocudu
