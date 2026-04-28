// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "apps/services/metrics/metrics_config.h"
#include "apps/units/application_unit_commands.h"
#include "ocudu/du/du_high/o_du_high.h"
#include "ocudu/du/du_high/o_du_high_config.h"

namespace ocudu {
struct o_du_high_unit_config;

namespace app_services {
class metrics_notifier;
class remote_server_metrics_gateway;
} // namespace app_services

namespace odu {
class du_high_executor_mapper;
struct o_du_high_config;
struct o_du_high_dependencies;
class f1c_connection_client;
class f1u_du_gateway;
} // namespace odu

class du_high_executor_mapper;
template <typename ConnectorType, typename NotifierType, typename InterfaceType>
class e2_metric_connector_manager;
class e2_du_metrics_connector;
class e2_du_metrics_notifier;
class e2_du_metrics_interface;
class e2_connection_client;
class mac_pcap;
class rlc_pcap;
class timer_manager;
struct du_high_unit_config;
struct du_high_wrapper_config;
struct du_high_wrapper_dependencies;
struct o_du_high_unit_config;
using e2_du_metrics_connector_manager =
    e2_metric_connector_manager<e2_du_metrics_connector, e2_du_metrics_notifier, e2_du_metrics_interface>;

/// Prints basic DU info in the stdout and in the GNB logs.
void announce_du_high_cells(const du_high_unit_config& du_high_unit_cfg);

/// O-RAN DU high unit.
struct o_du_high_unit {
  std::unique_ptr<odu::o_du_high>           o_du_hi;
  application_unit_commands                 commands;
  std::vector<app_services::metrics_config> metrics;
};

/// O-RAN DU high unit dependencies.
struct o_du_high_unit_dependencies {
  odu::du_high_executor_mapper&                execution_mapper;
  odu::f1c_connection_client&                  f1c_client_handler;
  gtpu_teid_pool&                              f1u_teid_allocator;
  odu::f1u_du_gateway&                         f1u_gw;
  mac_clock_controller&                        timer_ctrl;
  mac_pcap&                                    mac_p;
  rlc_pcap&                                    rlc_p;
  e2_connection_client&                        e2_client_handler;
  e2_du_metrics_connector_manager&             e2_metric_connectors;
  app_services::metrics_notifier&              metrics_notifier;
  app_services::remote_server_metrics_gateway* remote_metrics_gateway = nullptr;
  odu::o_du_high_dependencies                  o_du_hi_dependencies;
};

/// Creates the O-RAN DU high unit with the given configuration.
o_du_high_unit make_o_du_high_unit(const o_du_high_unit_config&  o_du_high_unit_cfg,
                                   o_du_high_unit_dependencies&& dependencies);

} // namespace ocudu
