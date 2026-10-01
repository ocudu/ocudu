// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "apps/services/metrics/metrics_config.h"
#include "apps/units/application_unit_commands.h"
#include "o_cu_cp_unit_impl.h"
#include "ocudu/cu_cp/cu_cp.h"
#include "ocudu/e2/e2_cu_metrics_connector.h"
#include "ocudu/xnap/gateways/xnc_connection_gateway.h"

namespace ocudu {

namespace app_services {
class metrics_notifier;
class remote_server_metrics_gateway;
} // namespace app_services

namespace ocucp {
class cu_cp_executor_mapper;
} // namespace ocucp

class dlt_pcap;
class io_broker;
struct o_cu_cp_unit_config;

template <typename ConnectorType, typename NotifierType, typename InterfaceType>
class e2_metric_connector_manager;
class e2_connection_client;

/// O-RAN CU-CP build dependencies.
struct o_cu_cp_unit_dependencies {
  ocucp::cu_cp_executor_mapper*                executor_mapper        = nullptr;
  timer_manager*                               timers                 = nullptr;
  dlt_pcap*                                    ngap_pcap              = nullptr;
  io_broker*                                   broker                 = nullptr;
  e2_connection_client*                        e2_gw                  = nullptr;
  app_services::metrics_notifier*              metrics_notifier       = nullptr;
  app_services::remote_server_metrics_gateway* remote_metrics_gateway = nullptr;
  std::vector<ocucp::xnc_connection_gateway*>  xnc_gws;
};

/// O-RAN CU-CP unit.
struct o_cu_cp_unit {
  std::unique_ptr<ocucp::o_cu_cp>           unit;
  application_unit_commands                 commands;
  std::vector<app_services::metrics_config> metrics;
};

/// Builds an O-RAN CU-CP unit with the given configuration and dependencies.
o_cu_cp_unit build_o_cu_cp(const o_cu_cp_unit_config& unit_cfg, const o_cu_cp_unit_dependencies& dependencies);

} // namespace ocudu
