// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/du/du_high/du_high_configuration.h"
#include "ocudu/e2/e2ap_config.h"

namespace ocudu {

class e2_du_metrics_interface;
class e2_connection_client;

namespace fapi {
class p5_requests_gateway;
class p7_last_request_notifier;
class p7_requests_gateway;
} // namespace fapi

namespace odu {

/// Base O-DU high configuration.
struct o_du_high_config {
  /// Configuration of the DU-high that comprises the MAC, RLC and F1 layers.
  du_high_configuration du_hi;
  /// E2AP configuration.
  e2ap_config e2ap_cfg;
  /// Flag that indicates if realtime mode is enabled or disabled.
  bool is_rt_mode_enabled;
};

/// O-RAN DU high sector dependencies. Contains the dependencies of one sector.
struct o_du_high_sector_dependencies {
  fapi::p5_requests_gateway&      p5_gateway;
  fapi::p7_requests_gateway&      p7_gateway;
  fapi::p7_last_request_notifier& p7_last_req_notifier;
  /// Timer manager.
  timer_manager& timer_mng;
  /// FAPI control executor.
  task_executor& fapi_ctrl_executor;
  /// MAC control executor.
  task_executor& mac_ctrl_executor;
  /// FAPI logger.
  ocudulog::basic_logger& fapi_logger;
};

/// O-RAN DU high dependencies.
struct o_du_high_dependencies {
  std::vector<o_du_high_sector_dependencies> sectors;
  du_high_dependencies                       du_hi;
  e2_connection_client*                      e2_client          = nullptr;
  e2_du_metrics_interface*                   e2_du_metric_iface = nullptr;
};

} // namespace odu
} // namespace ocudu
