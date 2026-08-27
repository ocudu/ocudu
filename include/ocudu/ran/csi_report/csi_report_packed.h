// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/bounded_bitset.h"
#include "ocudu/support/units.h"

namespace ocudu {

/// \brief Maximum Channel State Information (CSI) report size in bits.
///
/// Largest CSI report part produced by any supported configuration, i.e. the CSI Part 2 of the report quantity \e
/// cri-RI-LI-PMI-CQI with the Type II codebook.
///
/// \remark The maximum is given by eight CSI-RS ports with \f$(N_1, N_2) = (2, 2)\f$, four beams, an 8-PSK phase
/// alphabet, subband amplitude reporting enabled, rank two, and the maximum number of non-zero wideband amplitude
/// coefficients in both layers. It adds up the Layer Indicator as per TS38.212 Table 6.3.1.1.2-5 and the Type II PMI
/// fields as per TS38.212 Table 6.3.2.1.2-1.
constexpr units::bits csi_report_max_size(101U);

/// Packed Channel State Information (CSI) report data type.
using csi_report_packed = bounded_bitset<csi_report_max_size.value(), false>;

} // namespace ocudu
