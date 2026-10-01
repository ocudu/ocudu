// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

namespace ocudu {

/// Channel equalizer algorithm type enumeration.
enum class channel_equalizer_algorithm_type {
  /// Zero Forcing.
  zf = 0,
  /// Minimum Mean Square Error.
  mmse
};

/// Convert channel equalizer algorithm type to string.
inline const char* to_string(channel_equalizer_algorithm_type type)
{
  switch (type) {
    case channel_equalizer_algorithm_type::zf:
      return "ZF";
    case channel_equalizer_algorithm_type::mmse:
    default:
      return "MMSE";
  }
}

} // namespace ocudu
