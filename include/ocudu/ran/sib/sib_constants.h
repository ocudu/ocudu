// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

namespace ocudu {

/// Maximum number of entries in \c intra_freq_neigh_cell_list and \c intra_freq_excluded_cell_list of SIB3. See
/// TS 38.331, \c maxCellIntra.
constexpr unsigned MAX_NOF_SIB3_INTRA_FREQ_CELLS = 16;

/// Maximum number of entries in \c inter_freq_carrier_freq_list of SIB4. See TS 38.331, \c maxFreq.
constexpr unsigned MAX_NOF_SIB4_INTER_FREQ_CARRIERS = 8;

} // namespace ocudu
