// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "o_cu_up_builder.h"
#include "apps/helpers/metrics/metrics_helpers.h"
#include "apps/services/worker_manager/worker_manager.h"
#include "cu_up/cu_up_unit_config_translators.h"
#include "cu_up/metrics/cu_up_f1u_metrics_consumers.h"
#include "cu_up/metrics/cu_up_f1u_metrics_producer.h"
#include "cu_up/metrics/cu_up_pdcp_metrics_consumers.h"
#include "cu_up/metrics/cu_up_pdcp_metrics_producer.h"
#include "e2/o_cu_up_e2_config_translators.h"
#include "o_cu_up_unit_config.h"
#include "ocudu/cu_up/o_cu_up_factory.h"

using namespace ocudu;

static pdcp_metrics_notifier*
build_pdcp_metrics_config(std::vector<app_services::metrics_config>&   cu_up_services_cfg,
                          app_services::metrics_notifier&              metrics_notifier,
                          bool                                         e2_enabled,
                          e2_cu_metrics_notifier&                      e2_notifier,
                          const cu_up_unit_metrics_config&             cu_up_metrics_cfg,
                          worker_manager&                              workers,
                          timer_manager&                               timers,
                          app_services::remote_server_metrics_gateway* remote_metrics_gateway)
{
  if (!cu_up_metrics_cfg.layers_cfg.enable_pdcp) {
    return nullptr;
  }

  auto                   metrics_generator  = std::make_unique<cu_up_pdcp_metrics_producer_impl>(metrics_notifier);
  pdcp_metrics_notifier* out                = metrics_generator.get();
  app_services::metrics_config& metrics_cfg = cu_up_services_cfg.emplace_back();
  metrics_cfg.metric_name                   = cu_up_pdcp_metrics_properties_impl().name();
  metrics_cfg.callback                      = cu_up_pdcp_metrics_callback;
  metrics_cfg.producers.push_back(std::move(metrics_generator));

  const app_helpers::metrics_config& unit_metrics_cfg = cu_up_metrics_cfg.common_metrics_cfg;
  if (unit_metrics_cfg.enable_json_metrics) {
    metrics_cfg.consumers.push_back(std::make_unique<cu_up_pdcp_metrics_consumer_json>(
        cu_up_pdcp_metrics_consumer_json_config{.report_period = cu_up_metrics_cfg.cu_up_report_period},
        cu_up_pdcp_metrics_consumer_json_dependencies{.logger   = ocudulog::fetch_basic_logger("APP"),
                                                      .gateway  = *remote_metrics_gateway,
                                                      .executor = workers.get_metrics_executor(),
                                                      .timer =
                                                          timers.create_unique_timer(workers.get_metrics_executor())}));
  }

  if (unit_metrics_cfg.enable_log_metrics) {
    metrics_cfg.consumers.push_back(
        std::make_unique<cu_up_pdcp_metrics_consumer_log>(app_helpers::fetch_logger_metrics_log_channel()));
  }

  if (e2_enabled) {
    metrics_cfg.consumers.push_back(std::make_unique<cu_up_pdcp_metrics_consumer_e2>(e2_notifier));
  }

  return out;
}

static ocuup::f1u_metrics_notifier*
build_f1u_metrics_config(std::vector<app_services::metrics_config>& cu_up_services_cfg,
                         app_services::metrics_notifier&            metrics_notifier,
                         bool                                       e2_enabled,
                         e2_cu_metrics_notifier&                    e2_notifier,
                         const cu_up_unit_metrics_config&           cu_up_metrics_cfg,
                         worker_manager&                            workers,
                         timer_manager&                             timers)
{
  if (!cu_up_metrics_cfg.layers_cfg.enable_nrup) {
    return nullptr;
  }

  auto                          metrics_generator = std::make_unique<cu_up_f1u_metrics_producer_impl>(metrics_notifier);
  ocuup::f1u_metrics_notifier*  out               = metrics_generator.get();
  app_services::metrics_config& metrics_cfg       = cu_up_services_cfg.emplace_back();
  metrics_cfg.metric_name                         = cu_up_f1u_metrics_properties_impl().name();
  metrics_cfg.callback                            = cu_up_f1u_metrics_callback;
  metrics_cfg.producers.push_back(std::move(metrics_generator));

  if (const app_helpers::metrics_config& unit_metrics_cfg = cu_up_metrics_cfg.common_metrics_cfg;
      unit_metrics_cfg.enable_json_metrics) {
    metrics_cfg.consumers.push_back(std::make_unique<cu_up_f1u_metrics_consumer_json>(
        cu_up_f1u_metrics_consumer_json_config{.report_period = cu_up_metrics_cfg.cu_up_report_period},
        cu_up_f1u_metrics_consumer_json_dependencies{.logger   = ocudulog::fetch_basic_logger("APP"),
                                                     .log_chan = app_helpers::fetch_json_metrics_log_channel(),
                                                     .executor = workers.get_metrics_executor(),
                                                     .timer =
                                                         timers.create_unique_timer(workers.get_metrics_executor())}));

    if (unit_metrics_cfg.enable_log_metrics) {
      metrics_cfg.consumers.push_back(
          std::make_unique<cu_up_f1u_metrics_consumer_log>(app_helpers::fetch_logger_metrics_log_channel()));
    }

    if (e2_enabled) {
      metrics_cfg.consumers.push_back(std::make_unique<cu_up_f1u_metrics_consumer_e2>(e2_notifier));
    }
  }

  return out;
}

o_cu_up_unit ocudu::build_o_cu_up(const o_cu_up_unit_config& unit_cfg, const o_cu_up_unit_dependencies& dependencies)
{
  o_cu_up_unit          ocu_unit = {};
  ocuup::o_cu_up_config config;
  config.cu_up_cfg     = generate_cu_up_config(unit_cfg.cu_up_cfg);
  config.cu_up_cfg.qos = generate_cu_up_qos_config(unit_cfg.cu_up_cfg);

  // Create NG-U gateway(s).
  std::vector<std::unique_ptr<gtpu_gateway>> ngu_gws;
  if (!unit_cfg.cu_up_cfg.ngu_cfg.no_core) {
    for (const cu_up_unit_ngu_socket_config& sock_cfg : unit_cfg.cu_up_cfg.ngu_cfg.ngu_socket_cfg) {
      udp_network_gateway_config ngu_udp_cfg = {};
      ngu_udp_cfg.if_name                    = "NG-U";
      ngu_udp_cfg.bind_address               = sock_cfg.bind_addr;
      ngu_udp_cfg.bind_interface             = sock_cfg.bind_interface;
      ngu_udp_cfg.ext_bind_addr              = sock_cfg.ext_addr;
      ngu_udp_cfg.pool_occupancy_threshold   = sock_cfg.udp_config.pool_threshold;
      ngu_udp_cfg.bind_port                  = GTPU_PORT;
      ngu_udp_cfg.rx_max_mmsg                = sock_cfg.udp_config.rx_max_msgs;
      ngu_udp_cfg.tx_qsize                   = sock_cfg.udp_config.tx_qsize;
      ngu_udp_cfg.tx_max_mmsg                = sock_cfg.udp_config.tx_max_msgs;
      ngu_udp_cfg.tx_max_segments            = sock_cfg.udp_config.tx_max_segments;
      ngu_udp_cfg.pool_occupancy_threshold   = sock_cfg.udp_config.pool_threshold;
      ngu_udp_cfg.reuse_addr                 = sock_cfg.udp_config.reuse_addr;
      ngu_udp_cfg.dscp                       = sock_cfg.udp_config.dscp;
      ngu_udp_cfg.warn_on_drop               = unit_cfg.cu_up_cfg.warn_on_drop;

      std::unique_ptr<gtpu_gateway> ngu_gw =
          create_udp_gtpu_gateway(ngu_udp_cfg,
                                  dependencies.io_brk,
                                  dependencies.workers.get_cu_up_executor_mapper().io_ul_executor(),
                                  dependencies.workers.get_cu_up_executor_mapper().ngu_rx_executor());
      ngu_gws.push_back(std::move(ngu_gw));
    }
  } else {
    ngu_gws.push_back(create_no_core_gtpu_gateway());
  }

  // Create Xn-U gateway(s).
  std::vector<std::unique_ptr<gtpu_gateway>> xnu_gws;
  for (const xnu_socket_appconfig& sock_cfg : unit_cfg.cu_up_cfg.xnu_cfg.sockets_cfg.xnu_socket_cfg) {
    udp_network_gateway_config xnu_udp_cfg = {};
    xnu_udp_cfg.if_name                    = "Xn-U";
    xnu_udp_cfg.bind_address               = sock_cfg.bind_addr;
    xnu_udp_cfg.ext_bind_addr              = sock_cfg.udp_config.ext_addr;
    xnu_udp_cfg.bind_port                  = unit_cfg.cu_up_cfg.xnu_cfg.sockets_cfg.bind_port;
    xnu_udp_cfg.pool_occupancy_threshold   = sock_cfg.udp_config.pool_threshold;
    xnu_udp_cfg.rx_max_mmsg                = sock_cfg.udp_config.rx_max_msgs;
    xnu_udp_cfg.tx_qsize                   = sock_cfg.udp_config.tx_qsize;
    xnu_udp_cfg.tx_max_mmsg                = sock_cfg.udp_config.tx_max_msgs;
    xnu_udp_cfg.tx_max_segments            = sock_cfg.udp_config.tx_max_segments;
    xnu_udp_cfg.reuse_addr                 = sock_cfg.udp_config.reuse_addr;
    xnu_udp_cfg.dscp                       = sock_cfg.udp_config.dscp;
    xnu_udp_cfg.warn_on_drop               = unit_cfg.cu_up_cfg.warn_on_drop;

    xnu_gws.push_back(create_udp_gtpu_gateway(xnu_udp_cfg,
                                              dependencies.io_brk,
                                              dependencies.workers.get_cu_up_executor_mapper().io_ul_executor(),
                                              dependencies.workers.get_cu_up_executor_mapper().xnu_rx_executor()));
  }

  auto e2_metric_connectors = std::make_unique<e2_cu_metrics_connector_manager>();

  e2_cu_metrics_interface* e2_cu_metric_iface = nullptr;
  e2_connection_client*    e2_client          = nullptr;

  if (unit_cfg.e2_cfg.base_config.enable_unit_e2) {
    ocudu_assert(!config.cu_up_cfg.plmns.empty(), "CU-UP PLMN list must not be empty");
    config.e2ap_cfg    = generate_e2_config(unit_cfg.e2_cfg.base_config,
                                         unit_cfg.cu_up_cfg.gnb_id,
                                         config.cu_up_cfg.plmns.front(),
                                         config.cu_up_cfg.cu_up_id);
    e2_client          = &dependencies.e2_gw;
    e2_cu_metric_iface = &e2_metric_connectors->get_e2_metrics_interface(0);
  }

  auto pdcp_metric_notifier = build_pdcp_metrics_config(ocu_unit.metrics,
                                                        dependencies.metrics_notifier,
                                                        unit_cfg.e2_cfg.base_config.enable_unit_e2,
                                                        e2_metric_connectors->get_e2_metric_notifier(0),
                                                        unit_cfg.cu_up_cfg.metrics,
                                                        dependencies.workers,
                                                        dependencies.timers,
                                                        dependencies.remote_metrics_gateway);

  auto f1u_metric_notifier = build_f1u_metrics_config(ocu_unit.metrics,
                                                      dependencies.metrics_notifier,
                                                      unit_cfg.e2_cfg.base_config.enable_unit_e2,
                                                      e2_metric_connectors->get_e2_metric_notifier(0),
                                                      unit_cfg.cu_up_cfg.metrics,
                                                      dependencies.workers,
                                                      dependencies.timers);

  for (auto& qos_ : config.cu_up_cfg.qos) {
    qos_.second.pdcp_custom_cfg.metrics_notifier = pdcp_metric_notifier;
    if (!pdcp_metric_notifier) {
      qos_.second.pdcp_custom_cfg.metrics_period = std::chrono::milliseconds(0);
    }
    qos_.second.f1u_cfg.metrics_notifier = f1u_metric_notifier;
    if (!f1u_metric_notifier) {
      qos_.second.f1u_cfg.metrics_period = std::chrono::milliseconds(0);
    }
  }

  ocuup::o_cu_up_dependencies ocu_up_dependencies{
      .cu_dependencies    = ocuup::cu_up_dependencies{.exec_mapper = dependencies.workers.get_cu_up_executor_mapper(),
                                                      .f1u_teid_allocator   = dependencies.f1u_teid_allocator,
                                                      .f1u_gateway          = dependencies.f1u_gateway,
                                                      .timers               = dependencies.timers,
                                                      .gtpu_pcap            = dependencies.gtpu_pcap,
                                                      .logger               = ocudulog::fetch_basic_logger("CU-UP", false),
                                                      .pdcp_metric_notifier = pdcp_metric_notifier,
                                                   // TODO support multiple E1APs.
                                                      .e1_conn_clients   = {dependencies.e1ap_conn_client},
                                                      .ngu_gws           = std::move(ngu_gws),
                                                      .xnu_gws           = std::move(xnu_gws),
                                                      .e1_setup_notifier = nullptr

      },
      .e2_cu_metric_iface = e2_cu_metric_iface,
      .e2_client          = e2_client};

  ocu_unit.unit = std::make_unique<o_cu_up_unit_impl>(std::move(e2_metric_connectors),
                                                      ocuup::create_o_cu_up(config, std::move(ocu_up_dependencies)));

  return ocu_unit;
}
