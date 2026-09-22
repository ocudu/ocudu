// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/fapi/cell_config.h"
#include "ocudu/ocudulog/logger.h"
#include "ocudu/support/timers.h"

namespace ocudu {

namespace fapi {
class p5_requests_gateway;
} // namespace fapi

namespace fapi_adaptor {

/// MAC-FAPI P5 sector fastpath adaptor configuration.
struct mac_fapi_p5_sector_fastpath_adaptor_config {
  bool                     is_rt_mode_enabled;
  fapi::cell_configuration cell_cfg;
};

/// MAC-FAPI P5 sector fastpath adaptor dependencies.
struct mac_fapi_p5_sector_fastpath_adaptor_dependencies {
  /// Logger.
  ocudulog::basic_logger& logger;
  /// FAPI P5 requests gateway.
  fapi::p5_requests_gateway& p5_gateway;
  /// Timer manager.
  timer_manager& timers;
  /// FAPI control task executor.
  task_executor& fapi_ctrl_executor;
  /// MAC control task executor.
  task_executor& mac_ctrl_executor;
};

} // namespace fapi_adaptor
} // namespace ocudu
