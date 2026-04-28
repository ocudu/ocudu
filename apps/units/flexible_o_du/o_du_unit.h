// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "apps/helpers/e2/e2_metric_connector_manager.h"
#include "apps/services/metrics/metrics_config.h"
#include "apps/units/application_unit_commands.h"
#include "ocudu/du/du.h"
#include "ocudu/gtpu/gtpu_teid_pool.h"
#include "ocudu/ntn/ntn_configuration_manager.h"
#include <memory>
#include <vector>

namespace ocudu {

namespace app_services {
class metrics_notifier;
class remote_server_metrics_gateway;
} // namespace app_services

namespace odu {
class f1u_du_gateway;
class f1c_connection_client;
} // namespace odu

class e2_connection_client;
class e2_du_metrics_connector;
class e2_du_metrics_notifier;
class e2_du_metrics_interface;
class mac_pcap;
class rlc_pcap;
class timer_manager;
struct worker_manager;
class mac_clock_controller;

/// O-DU unit.
struct o_du_unit {
  std::unique_ptr<odu::du>                  unit;
  std::vector<app_services::metrics_config> metrics;
  application_unit_commands                 commands;
  std::unique_ptr<e2_metric_connector_manager<e2_du_metrics_connector, e2_du_metrics_notifier, e2_du_metrics_interface>>
                                                        e2_metric_connectors;
  std::unique_ptr<ocudu_ntn::ntn_configuration_manager> ntn_configurator_manager;
};

/// O-RAN DU unit dependencies.
struct o_du_unit_dependencies {
  worker_manager*                              workers                = nullptr;
  odu::f1c_connection_client*                  f1c_client_handler     = nullptr;
  gtpu_teid_pool*                              f1u_teid_allocator     = nullptr;
  odu::f1u_du_gateway*                         f1u_gw                 = nullptr;
  mac_clock_controller*                        timer_ctrl             = nullptr;
  mac_pcap*                                    mac_p                  = nullptr;
  rlc_pcap*                                    rlc_p                  = nullptr;
  e2_connection_client*                        e2_client_handler      = nullptr;
  app_services::metrics_notifier*              metrics_notifier       = nullptr;
  app_services::remote_server_metrics_gateway* remote_metrics_gateway = nullptr;
  ocudulog::basic_logger*                      fapi_logger            = nullptr;
};

} // namespace ocudu
