// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/fapi/p7/builders/dl_tti_request_builder.h"
#include "ocudu/fapi_adaptor/precoding_codebook_mapper.h"

namespace ocudu {

struct prs_info;

namespace fapi_adaptor {

/// \brief Helper function that converts from a DL-PRS MAC PDU to a DL-PRS FAPI PDU.
///
/// \param[out] builder       DL_TTI.Request builder where to add the DL-PRS PDU.
/// \param[in] prs_pdu        DL-PRS MAC PDU to convert to DL-PRS FAPI PDU.
/// \param[in] pm_mapper      Precoding codebook mapper.
/// \param[in] cell_nof_prbs  Number of PRBs of the cell.
void convert_prs_mac_to_fapi(fapi::dl_tti_request_builder&    builder,
                             const prs_info&                  prs_pdu,
                             const precoding_codebook_mapper& pm_mapper,
                             unsigned                         cell_nof_prbs);

} // namespace fapi_adaptor
} // namespace ocudu
