// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/reference_location.h"
#include <chrono>

namespace ocudu::ocucp {

/// \brief Coarse UE location reported by the UE, TS 38.300 sec. 16.14.8.
///
/// Accurate to roughly 2 km at the instant it is taken.
struct coarse_ue_location {
  reference_location position;
  /// Time the report was received.
  std::chrono::steady_clock::time_point received_at;
};

} // namespace ocudu::ocucp
