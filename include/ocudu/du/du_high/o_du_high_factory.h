// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/du/du_high/o_du_high.h"
#include <memory>

namespace ocudu {
namespace odu {

struct o_du_high_config;
struct o_du_high_dependencies;

/// Creates an O-RAN DU high with the given configuration and dependencies.
std::unique_ptr<o_du_high> make_o_du_high(const o_du_high_config& config, o_du_high_dependencies&& dependencies);

} // namespace odu
} // namespace ocudu
