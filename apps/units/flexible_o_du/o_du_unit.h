// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "apps/helpers/e2/e2_metric_connector_manager.h"
#include "apps/services/metrics/metrics_channel.h"
#include "apps/units/application_unit_commands.h"
#include "ocudu/du/du.h"
#include "ocudu/gtpu/gtpu_teid_pool.h"
#include "ocudu/ocudulog/ocudulog.h"
#include <memory>
#include <vector>

namespace ocudu {

namespace app_services {
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
  std::unique_ptr<odu::du>                 unit;
  app_services::metrics_channel_collection metrics;
  application_unit_commands                commands;
  std::unique_ptr<e2_metric_connector_manager<e2_du_metrics_connector, e2_du_metrics_notifier, e2_du_metrics_interface>>
      e2_metric_connectors;
};

/// O-RAN DU unit dependencies.
struct o_du_unit_dependencies {
  worker_manager&                              workers;
  odu::f1c_connection_client&                  f1c_client_handler;
  gtpu_teid_pool&                              f1u_teid_allocator;
  odu::f1u_du_gateway&                         f1u_gw;
  mac_clock_controller&                        timer_ctrl;
  mac_pcap&                                    mac_p;
  rlc_pcap&                                    rlc_p;
  e2_connection_client&                        e2_client_handler;
  ocudulog::basic_logger&                      fapi_logger;
  ocudulog::basic_logger&                      du_logger;
  app_services::remote_server_metrics_gateway* remote_metrics_gateway = nullptr;
};

} // namespace ocudu
