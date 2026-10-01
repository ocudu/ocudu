// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/fapi/p7/builders/ul_srs_pdu_builder.h"

namespace ocudu {

struct srs_info;

namespace fapi_adaptor {

/// \brief Helper function that converts from a SRS MAC PDU to a SRS FAPI PDU.
///
/// \param[out] fapi_pdu     SRS FAPI builder that helps to fill the PDU.
/// \param[in]  mac_pdu      MAC PDU that contains the SRS parameters.
void convert_srs_mac_to_fapi(fapi::ul_srs_pdu_builder& builder, const srs_info& mac_pdu);

} // namespace fapi_adaptor
} // namespace ocudu
