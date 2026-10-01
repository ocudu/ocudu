// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/upper/upper_phy_factories.h"

namespace ocudu {
namespace odu {

/// DU low cell configuration.
struct du_low_cell_config {
  /// Upper PHY configuration.
  upper_phy_configuration upper_phy_cfg;
};

/// DU low cell dependencies.
struct du_low_cell_dependencies {
  /// Upper PHY dependencies.
  upper_phy_dependencies upper_phy_deps;
};

/// Parameters used to instantiate the DU-low.
struct du_low_config {
  /// Upper physical layer common configuration for all cells.
  upper_phy_factory_configuration upper_phy_common_config;
  /// Specific cell configuration.
  std::vector<du_low_cell_config> cells;
};

/// Dependencies necessary to instantiate a DU-low.
struct du_low_dependencies {
  /// Logger used to report state of the DU-low.
  ocudulog::basic_logger* logger;
  /// Upper physical layer common dependencies for all cells.
  upper_phy_factory_dependencies upper_phy_common_deps;
  /// Specific cell dependencies.
  std::vector<du_low_cell_dependencies> cells;
};

} // namespace odu
} // namespace ocudu
