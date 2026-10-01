// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/fapi/p7/builders/ul_pucch_pdu_builder.h"

namespace ocudu {

struct pucch_info;

namespace fapi_adaptor {

/// \brief Helper function that converts from a PUCCH MAC PDU to a PUCCH FAPI PDU.
///
/// \param[out] builder PUCCH FAPI builder that helps to fill the PDU.
/// \param[in] mac_pdu MAC PDU that contains the PUCCH parameters.
void convert_pucch_mac_to_fapi(fapi::ul_pucch_pdu_builder& builder, const pucch_info& mac_pdu);

} // namespace fapi_adaptor
} // namespace ocudu
