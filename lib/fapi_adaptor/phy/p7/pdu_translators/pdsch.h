// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/fapi/p7/messages/dl_tti_request.h"
#include "ocudu/phy/support/re_pattern.h"
#include "ocudu/phy/upper/channel_processors/pdsch/pdsch_processor.h"

namespace ocudu {
namespace fapi_adaptor {

class precoding_codebook_repository;

/// Helper function that converts from a PDSCH FAPI PDU to a PDSCH processor PDU.
void convert_pdsch_fapi_to_phy(pdsch_processor::pdu_t&              proc_pdu,
                               const fapi::dl_pdsch_pdu&            fapi_pdu,
                               slot_point                           slot,
                               span<const re_pattern_list>          csi_re_pattern_list,
                               const precoding_codebook_repository& pm_repo);

} // namespace fapi_adaptor
} // namespace ocudu
