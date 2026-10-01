// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/csi_report/csi_report_data.h"
#include "ocudu/ran/pdcch/aggregation_level.h"
#include "ocudu/ran/pdcch/dci_packing.h"

namespace ocudu {

enum class cqi_table_t;

/// \brief Computes PDCCH aggregation level to use based on the input parameters.
///
/// \param[in] cqi CQI value to consider for the mapping.
/// \param[in] cqi_table CQI table to be used for the mapping.
/// \param[in] pdcch_candidates PDCCH candidates per aggregation level, where aggregation level for the array element
/// with index "x" is L=1U << x.
/// \param[in] nof_dci_bits PDCCH DCI size in nof. bits.
/// \return PDCCH aggregation level if a valid candidate is found. Else, returns highest aggregation level with PDCCH
/// candidate configured.
aggregation_level map_cqi_to_aggregation_level(float               cqi,
                                               cqi_table_t         cqi_table,
                                               span<const uint8_t> pdcch_candidates,
                                               unsigned            nof_dci_bits);

} // namespace ocudu
