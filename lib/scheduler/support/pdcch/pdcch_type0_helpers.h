// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief Type0-PDCCH CSS n0/nc slot, SFNc set derivation as per TS 38.213, clause 13.
#pragma once

#include "../../config/cell_configuration.h"
#include "ocudu/ran/slot_point.h"

namespace ocudu {

/// \brief Computes the Type0-PDCCH CSS n0 slot for each beam [TS 38.213, Section 13].
///
/// \param[in] searchspace0 Search Space Zero index.
/// \param[in] coreset0 CORESET 0 index.
/// \param[in] cell_cfg Cell configuration.
/// \param[in] scs_common SCS corresponding to subCarrierSpacingCommon, which must coincide with SCS if initial DL BWP.
/// \param[in] ssb_index SSB Beam index.
/// \return Returns slot n0 where UE should monitor Type0-PDCCH CSS.
slot_point precompute_type0_pdcch_css_n0(search_space0_index       searchspace0,
                                         uint8_t                   coreset0,
                                         const cell_configuration& cell_cfg,
                                         subcarrier_spacing        scs_common,
                                         unsigned                  ssb_index);

/// \brief Computes the Type0-PDCCH CSS n0 + 1 slot for each beam [TS 38.213, Section 13].
///
/// \param[in] searchspace0 Search Space Zero index.
/// \param[in] coreset0 CORESET 0 index.
/// \param[in] cell_cfg Cell configuration.
/// \param[in] scs_common SCS corresponding to subCarrierSpacingCommon, which must coincide with SCS if initial DL BWP.
/// \param[in] ssb_index SSB Beam index.
/// \return Returns slot n0 + 1 where UE should monitor Type0-PDCCH CSS.
slot_point precompute_type0_pdcch_css_n0_plus_1(search_space0_index       searchspace0,
                                                uint8_t                   coreset0,
                                                const cell_configuration& cell_cfg,
                                                subcarrier_spacing        scs_common,
                                                unsigned                  ssb_index);

} // namespace ocudu
