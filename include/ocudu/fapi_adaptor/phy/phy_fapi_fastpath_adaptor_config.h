// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/fapi_adaptor/phy/p5/phy_fapi_p5_sector_fastpath_adaptor_config.h"
#include "ocudu/fapi_adaptor/phy/p7/phy_fapi_p7_sector_fastpath_adaptor_config.h"

namespace ocudu {
namespace fapi_adaptor {

/// PHY-FAPI sector fastpath adaptor configuration.
struct phy_fapi_sector_fastpath_adaptor_config {
  phy_fapi_p5_sector_fastpath_adaptor_config p5_config;
  phy_fapi_p7_sector_fastpath_adaptor_config p7_config;
};

/// PHY-FAPI sector fastpath adaptor dependencies.
struct phy_fapi_sector_fastpath_adaptor_dependencies {
  phy_fapi_p5_sector_fastpath_adaptor_dependencies p5_dependencies;
  phy_fapi_p7_sector_fastpath_adaptor_dependencies p7_dependencies;
};

/// PHY-FAPI fastpath adaptor configuration.
struct phy_fapi_fastpath_adaptor_config {
  std::vector<phy_fapi_sector_fastpath_adaptor_config> sectors;
};

/// PHY-FAPI fastpath adaptor dependencies.
struct phy_fapi_fastpath_adaptor_dependencies {
  std::vector<phy_fapi_sector_fastpath_adaptor_dependencies> sectors;
};

} // namespace fapi_adaptor
} // namespace ocudu
