// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/du/du_low/du_low_config.h"
#include "ocudu/fapi_adaptor/phy/phy_fapi_fastpath_adaptor_config.h"

namespace ocudu {
namespace odu {

using cell_prach_ports_entry = std::vector<uint8_t>;

/// O-RAN DU low configuration.
struct o_du_low_config {
  du_low_config du_low_cfg;
  /// FAPI adaptor configuration.
  fapi_adaptor::phy_fapi_fastpath_adaptor_config fapi_cfg;
  /// Metrics configuration. Set to \c true to enable the DU low metrics.
  bool enable_metrics;
};

/// O-RAN DU low dependencies.
struct o_du_low_dependencies {
  /// DU Low dependencies.
  du_low_dependencies du_low_deps;
  /// FAPI P5 executor.
  task_executor& fapi_p5_executor;
};

} // namespace odu
} // namespace ocudu
