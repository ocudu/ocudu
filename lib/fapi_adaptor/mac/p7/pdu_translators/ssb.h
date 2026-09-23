// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/fapi/p7/builders/dl_ssb_pdu_builder.h"
#include "ocudu/mac/mac_cell_result.h"

namespace ocudu {
namespace fapi_adaptor {

/// \brief Helper function that converts from a SSB MAC PDU to a SSB FAPI PDU.
///
/// \param[out] builder   SSB FAPI builder that helps to fill the PDU.
/// \param[in] mac_pdu    SSB MAC PDU to convert to SSB FAPI PDU.
/// \param[in] slot       Slot point associated to this PDU.
void convert_ssb_mac_to_fapi(fapi::dl_ssb_pdu_builder& builder,
                             const ocudu::dl_ssb_pdu&  mac_pdu,
                             slot_point                slot,
                             unsigned                  cell_nof_prbs);

} // namespace fapi_adaptor
} // namespace ocudu
