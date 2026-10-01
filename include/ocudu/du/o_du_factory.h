// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/du/o_du.h"
#include "ocudu/du/o_du_config.h"
#include <memory>

namespace ocudu {
namespace odu {

/// Instantiates an O-RAN Distributed Unit (O-DU) object with the given configuration and dependencies.
std::unique_ptr<o_du> make_o_du(o_du_dependencies&& dependencies);

} // namespace odu
} // namespace ocudu
