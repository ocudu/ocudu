// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "apps/units/flexible_o_du/o_du_unit.h"
#include "ocudu/fapi_adaptor/phy/phy_fapi_adaptor.h"
#include <memory>

namespace ocudu {

struct split6_o_du_unit_config;

/// Helper function to build the O-RAN DU Split 6 with the given arguments.
o_du_unit create_o_du_split6(const split6_o_du_unit_config&                  du_unit_cfg,
                             const o_du_unit_dependencies&                   du_dependencies,
                             std::unique_ptr<fapi_adaptor::phy_fapi_adaptor> fapi_adaptor);

} // namespace ocudu
