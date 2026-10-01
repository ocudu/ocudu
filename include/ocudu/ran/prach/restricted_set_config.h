// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include <cstdint>

namespace ocudu {

/// \brief PRACH Restricted set configuration.
///
/// PRACH \f$N_{CS}\f$ restricted set configuration. Used for interpreting TS38.211 Tables 6.3.3.1-5 and 6.3.3.1-6.
enum class restricted_set_config : uint8_t { UNRESTRICTED, TYPE_A, TYPE_B };

inline const char* to_string(restricted_set_config config)
{
  switch (config) {
    case restricted_set_config::TYPE_A:
      return "type-A";
    case restricted_set_config::TYPE_B:
      return "type-B";
    case restricted_set_config::UNRESTRICTED:
    default:
      return "unrestricted";
  }
}

} // namespace ocudu
