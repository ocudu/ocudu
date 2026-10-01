// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include <array>

namespace ocudu {

/// TS 38.321, 6.1.3.1 - Buffer Status Report MAC CEs
enum class bsr_format { SHORT_BSR, LONG_BSR, SHORT_TRUNC_BSR, LONG_TRUNC_BSR };

inline const char* to_string(bsr_format bsr)
{
  static constexpr std::array<const char*, 4> names = {
      "Short BSR", "Long BSR", "Short Truncated BSR", "Long Truncated BSR"};
  auto idx = static_cast<std::size_t>(bsr);
  return idx < names.size() ? names[idx] : "Invalid BSR format";
}

} // namespace ocudu
