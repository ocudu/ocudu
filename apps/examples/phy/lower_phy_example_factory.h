// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/lower/lower_phy.h"
#include "ocudu/phy/lower/lower_phy_configuration.h"
#include <memory>

namespace ocudu {

/// Creates a generic lower PHY.
std::unique_ptr<lower_phy> create_lower_phy(const lower_phy_configuration& config, const lower_phy_dependencies& deps);

} // namespace ocudu
