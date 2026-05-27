// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/bounded_bitset.h"
#include "ocudu/support/units.h"

namespace ocudu {

/// Maximum Channel State Information (CSI) report size in bits.
constexpr units::bits csi_report_max_size(64U);

/// Packed Channel State Information (CSI) report data type.
using csi_report_packed = bounded_bitset<csi_report_max_size.value(), false>;

} // namespace ocudu
