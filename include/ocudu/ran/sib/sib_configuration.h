// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include <cstdint>

namespace ocudu {

/// \brief SIB retransmission periodicity in milliseconds as per TS38.331 Section 5.2.1.
/// \remark This is used for retransmission periodicity only; SIB1 periodicity is given as 160ms.
enum class sib1_rtx_periodicity : uint8_t { ms5 = 5, ms10 = 10, ms20 = 20, ms40 = 40, ms80 = 80, ms160 = 160 };

} // namespace ocudu
