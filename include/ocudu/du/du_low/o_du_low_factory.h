// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/du/du_low/o_du_low.h"
#include <memory>

namespace ocudu {
namespace odu {

struct o_du_low_config;
struct o_du_low_dependencies;

/// Creates and returns an O-RAN Distributed Unit (O-DU) low.
std::unique_ptr<o_du_low> make_o_du_low(const o_du_low_config& config, o_du_low_dependencies& o_du_low_deps);

} // namespace odu
} // namespace ocudu
