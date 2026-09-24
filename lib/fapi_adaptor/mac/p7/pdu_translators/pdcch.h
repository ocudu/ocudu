// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/fapi/p7/builders/dl_pdcch_pdu_builder.h"
#include "ocudu/scheduler/result/pdcch_info.h"

namespace ocudu {
namespace fapi_adaptor {

class precoding_codebook_mapper;

/// \brief Helper function that converts from a PDCCH MAC PDU to a PDCCH FAPI PDU.
///
/// \param[out] builder PDCCH FAPI builder that helps to fill the PDU.
/// \param[in] context_information MACX DCI context information.
/// \param[in] payload MACX DCI payload.
/// \param[in] pm_mapper Precoding codebook mapper.
/// \param[in] cell_nof_prbs Cell number of PRBs.
void convert_pdcch_mac_to_fapi(fapi::dl_pdcch_pdu_builder&      builder,
                               const dci_context_information&   context_information,
                               const dci_payload&               payload,
                               const precoding_codebook_mapper& pm_mapper,
                               unsigned                         cell_nof_prbs);

} // namespace fapi_adaptor
} // namespace ocudu
