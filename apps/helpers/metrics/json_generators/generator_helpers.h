// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "external/nlohmann/json.hpp"
#include "ocudu/ran/slot_point.h"

namespace ocudu {

/// [Implementation defined] Default indentation size for the JSON metrics.
constexpr unsigned DEFAULT_JSON_INDENT = 2U;

inline void to_json(nlohmann::json& json, slot_point slot)
{
  json = fmt::format("{}", slot);
}

} // namespace ocudu
