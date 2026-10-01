// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "external/nlohmann/json.hpp"

namespace ocudu {

struct mac_dl_metric_report;

namespace app_helpers {
namespace json_generators {

/// Generates a nlohmann JSON object that codifies the given downlink MAC metrics.
nlohmann::json generate(const mac_dl_metric_report& metrics);

} // namespace json_generators
} // namespace app_helpers
} // namespace ocudu
