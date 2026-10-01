// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/du/du_high/du_high.h"
#include "ocudu/du/du_high/du_high_configuration.h"

namespace ocudu {
namespace odu {

/// Create a DU-high instance, which comprises MAC, RLC and F1 layers.
std::unique_ptr<du_high> make_du_high(const du_high_configuration& du_hi_cfg, const du_high_dependencies& dependencies);

} // namespace odu
} // namespace ocudu
