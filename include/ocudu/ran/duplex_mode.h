// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include <algorithm>

namespace ocudu {

/// NR Duplex mode.
enum class duplex_mode {
  /// Paired FDD.
  FDD = 0,
  /// Unpaired TDD.
  TDD,
  /// Supplementary DownLink (Unpaired).
  SDL,
  /// Supplementary UpLink (Unpaired).
  SUL,
  INVALID
};

inline const char* to_string(duplex_mode mode)
{
  static constexpr const char* names[] = {"FDD", "TDD", "SDL", "SUL", "invalid"};
  return names[std::min((unsigned)mode, (unsigned)duplex_mode::INVALID)];
}

} // namespace ocudu
