// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/fapi_adaptor/mac/p5/mac_fapi_p5_sector_fastpath_adaptor_config.h"
#include "ocudu/fapi_adaptor/mac/p7/mac_fapi_p7_sector_fastpath_adaptor_config.h"

namespace ocudu {
namespace fapi_adaptor {

/// MAC-FAPI sector fastpath adaptor configuration.
struct mac_fapi_sector_fastpath_adaptor_config {
  mac_fapi_p5_sector_fastpath_adaptor_config p5_config;
  mac_fapi_p7_sector_fastpath_adaptor_config p7_config;
};

/// MAC-FAPI sector fastpath adaptor dependencies.
struct mac_fapi_sector_fastpath_adaptor_dependencies {
  mac_fapi_p5_sector_fastpath_adaptor_dependencies p5_dependencies;
  mac_fapi_p7_sector_fastpath_adaptor_dependencies p7_dependencies;
};

/// MAC-FAPI fastpath adaptor configuration.
struct mac_fapi_fastpath_adaptor_config {
  std::vector<mac_fapi_sector_fastpath_adaptor_config> sectors;
};

/// MAC-FAPI fastpath adaptor dependencies.
struct mac_fapi_fastpath_adaptor_dependencies {
  std::vector<mac_fapi_sector_fastpath_adaptor_dependencies> sectors;
};

} // namespace fapi_adaptor
} // namespace ocudu
