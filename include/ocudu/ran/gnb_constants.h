// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include <cstdint>

namespace ocudu {

/// \brief Defines the maximum number of cells that can be instantiated per OCUDU DU (implementation-defined).
/// \remark TS 38.473 defines an upper limit of maxCellingNBDU=512 for this value.
constexpr uint16_t MAX_CELLS_PER_DU = 32;

} // namespace ocudu
