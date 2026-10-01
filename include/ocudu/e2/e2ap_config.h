// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ocudulog/logger.h"
#include "ocudu/ran/gnb_cu_up_id.h"
#include "ocudu/ran/gnb_du_id.h"
#include "ocudu/ran/gnb_id.h"
#include "ocudu/support/timers.h"
#include <chrono>
#include <memory>
#include <optional>
#include <string>

namespace ocudu {

class e2_connection_client;
class e2_du_metrics_interface;
class task_executor;
class e2_node_component_config_provider;

namespace odu {

class f1ap_ue_id_translator;
class du_configurator;

} // namespace odu

/// E2AP configuration.
struct e2ap_config {
  gnb_id_t gnb_id = {0, 22};
  /// Full PLMN as string (without possible filler digit) e.g. "00101"
  std::string                   plmn;
  std::optional<gnb_du_id_t>    gnb_du_id;
  std::optional<gnb_cu_up_id_t> gnb_cu_up_id;
  unsigned                      max_setup_retries           = 5;
  std::chrono::milliseconds     ric_reconnection_retry_time = std::chrono::milliseconds{1000};
  bool                          e2sm_kpm_enabled            = false;
  bool                          e2sm_rc_enabled             = false;
  bool                          e2sm_ccc_enabled            = false;
};

/// E2AP dependencies.
struct e2ap_dependencies {
  e2_connection_client&                              e2_client;
  e2_du_metrics_interface*                           e2_metrics_var;
  odu::f1ap_ue_id_translator*                        f1ap_ue_id_translator;
  odu::du_configurator*                              du_configurator;
  timer_factory                                      timers;
  task_executor&                                     e2_exec;
  std::unique_ptr<e2_node_component_config_provider> node_component_config_provider;
  ocudulog::basic_logger&                            logger;
};

} // namespace ocudu
