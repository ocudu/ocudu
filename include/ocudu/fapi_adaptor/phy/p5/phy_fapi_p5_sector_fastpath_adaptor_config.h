// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once
#include "ocudu/ocudulog/logger.h"

namespace ocudu {

class upper_phy_operation_controller;
class task_executor;

namespace fapi_adaptor {

/// PHY-FAPI P5 sector fastpath adaptor configuration.
struct phy_fapi_p5_sector_fastpath_adaptor_config {
  /// Sector identifier.
  unsigned sector_id;
};

/// PHY-FAPI P5 sector fastpath adaptor dependencies.
struct phy_fapi_p5_sector_fastpath_adaptor_dependencies {
  /// Logger.
  ocudulog::basic_logger& logger;
  /// Executor.
  task_executor& executor;
  /// Upper PHY operation controller.
  upper_phy_operation_controller& upper_phy_controller;
};

} // namespace fapi_adaptor
} // namespace ocudu
