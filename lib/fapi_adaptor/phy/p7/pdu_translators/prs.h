// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/fapi/p7/messages/dl_prs_pdu.h"
#include "ocudu/ran/slot_point.h"

namespace ocudu {

struct prs_generator_configuration;

namespace fapi_adaptor {

class precoding_codebook_repository;

/// \brief Helper function that converts from a PRS FAPI PDU to a PRS generator configuration.
///
/// \param[out] generator_config PRS generator configuration.
/// \param[in] fapi_pdu FAPI PDU received that contains all the PRS parameters.
/// \param[in] sfn SFN when the FAPI PDU was received.
/// \param[in] slot Slot when the FAPI PDU was received.
/// \param[in] pm_repo Precoding weight matrix repository.
void convert_prs_fapi_to_phy(prs_generator_configuration&         generator_config,
                             const fapi::dl_prs_pdu&              fapi_pdu,
                             slot_point                           slot,
                             const precoding_codebook_repository& pm_repo);

} // namespace fapi_adaptor
} // namespace ocudu
