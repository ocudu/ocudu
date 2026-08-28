// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

namespace ocudu {

namespace app_services {
class remote_server_metrics_gateway;
class metrics_notifier;
} // namespace app_services

namespace ocuup {
class e1_connection_client;
} // namespace ocuup

class dlt_pcap;
class e2_connection_client;
class f1u_cu_up_gateway;
class gtpu_teid_pool;
class io_broker;
class timer_manager;

struct o_cu_up_unit_config;
struct worker_manager;

/// O-RAN CU-UP unit dependencies.
struct o_cu_up_unit_dependencies {
  worker_manager&                              workers;
  e2_connection_client&                        e2_gw;
  app_services::metrics_notifier&              metrics_notifier;
  app_services::remote_server_metrics_gateway* remote_metrics_gateway = nullptr;
  std::vector<ocuup::e1_connection_client*>    e1ap_conn_client;
  gtpu_teid_pool&                              f1u_teid_allocator;
  f1u_cu_up_gateway&                           f1u_gateway;
  dlt_pcap&                                    gtpu_pcap;
  timer_manager&                               timers;
  io_broker&                                   io_brk;
};

} // namespace ocudu
