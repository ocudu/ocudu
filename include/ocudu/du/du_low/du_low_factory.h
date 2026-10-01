// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/du/du_low/du_low.h"
#include <memory>

namespace ocudu {
namespace odu {

struct du_low_config;
struct du_low_dependencies;

/// Creates and returns the DU low.
std::unique_ptr<du_low> make_du_low(const du_low_config& config, du_low_dependencies&& deps);

} // namespace odu
} // namespace ocudu
