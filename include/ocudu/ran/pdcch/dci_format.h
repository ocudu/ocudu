// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include <array>
#include <cstdint>

namespace ocudu {

/// Supported DL DCI formats.
enum class dci_dl_format : uint8_t { f1_0, f1_1, f2_0 };

/// Supported UL DCI formats.
enum class dci_ul_format : uint8_t { f0_0, f0_1 };

inline const char* dci_format_to_string(dci_dl_format type)
{
  static constexpr std::array<const char*, 4> names = {"1_0", "1_1", "2_0", "invalid"};
  unsigned                                    idx   = static_cast<unsigned>(type);
  return names[idx < names.size() ? idx : names.size() - 1];
}

inline const char* dci_format_to_string(dci_ul_format type)
{
  static constexpr std::array<const char*, 3> names = {"0_0", "0_1", "invalid"};
  unsigned                                    idx   = static_cast<unsigned>(type);
  return names[idx < names.size() ? idx : names.size() - 1];
}

} // namespace ocudu
