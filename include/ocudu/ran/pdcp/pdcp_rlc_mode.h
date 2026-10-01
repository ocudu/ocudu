// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

namespace ocudu {

/// PDCP NR RLC mode information.
enum class pdcp_rlc_mode { um, am };

inline const char* format_as(pdcp_rlc_mode mode)
{
  static constexpr const char* options[] = {"UM", "AM"};
  return options[static_cast<unsigned>(mode)];
}

} // namespace ocudu
