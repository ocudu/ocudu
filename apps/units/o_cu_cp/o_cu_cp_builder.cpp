// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "o_cu_cp_builder.h"
#include "apps/helpers/metrics/metrics_helpers.h"
#include "apps/services/metrics/metrics_config.h"
#include "apps/units/o_cu_cp/cu_cp/metrics/cu_cp_metrics.h"
#include "apps/units/o_cu_cp/cu_cp/metrics/cu_cp_metrics_consumers.h"
#include "apps/units/o_cu_cp/cu_cp/metrics/cu_cp_metrics_producer.h"
#include "cu_cp/commands/cu_cp_remote_commands.h"
#include "cu_cp/cu_cp_cmdline_commands.h"
#include "cu_cp/cu_cp_config_translators.h"
#include "e2/o_cu_cp_e2_config_translators.h"
#include "o_cu_cp_unit_config.h"
#include "o_cu_cp_unit_impl.h"
#include "ocudu/adt/format.h"
#include "ocudu/cu_cp/cu_cp_command_handler.h"
#include "ocudu/cu_cp/cu_cp_executor_mapper.h"
#include "ocudu/cu_cp/o_cu_cp_config.h"
#include "ocudu/cu_cp/o_cu_cp_factory.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/ran/i_rnti.h"

using namespace ocudu;

static cu_cp_metrics_report_notifier*
build_cu_cp_metrics_config(std::vector<app_services::metrics_config>&   cu_cp_services_cfg,
                           app_services::metrics_notifier&              metrics_notifier,
                           app_services::remote_server_metrics_gateway* remote_metrics_gateway,
                           const cu_cp_unit_metrics_config&             cu_cp_metrics_cfg,
                           bool                                         e2_enabled,
                           e2_cu_metrics_notifier&                      e2_notifier)
{
  cu_cp_metrics_report_notifier* out = nullptr;

  auto metrics_generator                            = std::make_unique<cu_cp_metrics_producer_impl>(metrics_notifier);
  out                                               = &(*metrics_generator);
  app_services::metrics_config& metrics_service_cfg = cu_cp_services_cfg.emplace_back();
  metrics_service_cfg.metric_name                   = cu_cp_metrics_properties_impl().name();
  metrics_service_cfg.callback                      = cu_cp_metrics_callback;
  metrics_service_cfg.producers.push_back(std::move(metrics_generator));

  if (const app_helpers::metrics_config& unit_metrics_cfg = cu_cp_metrics_cfg.common_metrics_cfg;
      unit_metrics_cfg.enable_json_metrics) {
    report_error_if_not(remote_metrics_gateway,
                        "Invalid remote server gateway for sending JSON metrics. Check that remote server is enabled");
    metrics_service_cfg.consumers.push_back(std::make_unique<cu_cp_metrics_consumer_json>(*remote_metrics_gateway));
  }
  if (cu_cp_metrics_cfg.common_metrics_cfg.enable_log_metrics) {
    metrics_service_cfg.consumers.push_back(
        std::make_unique<cu_cp_metrics_consumer_log>(app_helpers::fetch_logger_metrics_log_channel()));
  }
  if (e2_enabled) {
    metrics_service_cfg.consumers.push_back(std::make_unique<cu_cp_metrics_consumer_e2>(e2_notifier));
  }

  return out;
}

/// Logs the Local NG-RAN Node Identifier that the I-RNTIs of this node carry, and which gNB IDs a neighbour can be
/// given while staying distinguishable from it.
///
/// The I-RNTI identifies the allocating node by a Local NG-RAN Node Identifier (TS 38.300 Annex F) that is narrower
/// than a gNB ID. [Implementation-defined] The CU-CP takes it from the least significant bits of its gNB ID
/// (TS 38.300 Annex C). Peers resolve an I-RNTI at RRC Resume by matching that identifier against the gNB IDs they are
/// connected to, so gNB IDs sharing those bits address the same node.
static void
log_i_rnti_node_identifiers(gnb_id_t gnb_id, full_i_rnti_profile full_profile, short_i_rnti_profile short_profile)
{
  const uint32_t full_mask  = (1U << full_i_rnti_t::nof_node_id_bits(full_profile)) - 1;
  const uint32_t short_mask = (1U << short_i_rnti_t::nof_node_id_bits(short_profile)) - 1;
  const uint32_t full_id    = full_i_rnti_t::to_local_node_id(full_profile, gnb_id.id);
  const uint32_t short_id   = short_i_rnti_t::to_local_node_id(short_profile, gnb_id.id);

  // A gNB ID that fits in the node identifier is carried whole, and the I-RNTIs of this node address it alone.
  const bool full_truncates  = gnb_id.id > full_mask;
  const bool short_truncates = gnb_id.id > short_mask;
  if (!full_truncates && !short_truncates) {
    return;
  }

  auto is_available = [&](uint32_t candidate) {
    return (!full_truncates || (candidate & full_mask) != full_id) &&
           (!short_truncates || (candidate & short_mask) != short_id);
  };

  // Walk up from the configured gNB ID, which shows that adjacent gNB IDs are usable while those a whole node
  // identifier apart are not.
  std::vector<std::string> available;
  std::vector<std::string> unavailable;
  const uint64_t           gnb_id_range = uint64_t{1} << gnb_id.bit_length;
  for (uint64_t candidate = gnb_id.id + 1; candidate < gnb_id_range && (available.size() < 3 || unavailable.size() < 3);
       ++candidate) {
    std::vector<std::string>& examples = is_available(candidate) ? available : unavailable;
    if (examples.size() < 3) {
      examples.push_back(fmt::format("{:#x}", candidate));
    }
  }

  std::vector<std::string> conditions;
  if (full_truncates) {
    conditions.push_back(fmt::format("gnb_id & {:#x} != {:#x}", full_mask, full_id));
  }
  if (short_truncates) {
    conditions.push_back(fmt::format("gnb_id & {:#x} != {:#x}", short_mask, short_id));
  }

  ocudulog::basic_logger& logger = ocudulog::fetch_basic_logger("CU-CP", false);
  logger.info("I-RNTI node identifiers for gnb_id={:#x}: full={:#x}, short={:#x}. A neighbour gNB is told apart from "
              "this node only if {} (available gnb_ids: {}; unavailable gnb_ids: {})",
              gnb_id.id,
              full_id,
              short_id,
              fmt::join(conditions, " and "),
              fmt::join(available, ", "),
              fmt::join(unavailable, ", "));
}

o_cu_cp_unit ocudu::build_o_cu_cp(const o_cu_cp_unit_config& unit_cfg, const o_cu_cp_unit_dependencies& dependencies)
{
  ocudu_assert(dependencies.executor_mapper, "Invalid CU-CP executor mapper");
  ocudu_assert(dependencies.ngap_pcap, "Invalid NGAP PCAP");
  ocudu_assert(dependencies.broker, "Invalid IO broker");
  ocudu_assert(dependencies.metrics_notifier, "Invalid metrics notifier");

  ocucp::o_cu_cp_config       o_cu_cp_cfg;
  const cu_cp_unit_config&    cucp_unit_cfg = unit_cfg.cucp_cfg;
  ocucp::cu_cp_configuration& cu_cp_cfg     = o_cu_cp_cfg.cu_cp_config;
  cu_cp_cfg                                 = generate_cu_cp_config(cucp_unit_cfg);
  cu_cp_cfg.services.cu_cp_executor         = &dependencies.executor_mapper->ctrl_executor();
  cu_cp_cfg.services.cu_cp_e2_exec          = &dependencies.executor_mapper->e2_executor();
  cu_cp_cfg.services.timers                 = dependencies.timers;
  cu_cp_cfg.xnap.xnc_gws                    = dependencies.xnc_gws;

  // Only a peer over Xn resolves the I-RNTIs this node allocates.
  if (!cucp_unit_cfg.xnap_config.gateways.empty()) {
    log_i_rnti_node_identifiers(cu_cp_cfg.node.gnb_id, cu_cp_cfg.ue.full_i_rnti_prof, cu_cp_cfg.ue.short_i_rnti_prof);
  }

  o_cu_cp_unit ocucp;
  auto         e2_metric_connectors = std::make_unique<e2_cu_metrics_connector_manager>();

  cu_cp_cfg.metrics_notifier = build_cu_cp_metrics_config(ocucp.metrics,
                                                          *dependencies.metrics_notifier,
                                                          dependencies.remote_metrics_gateway,
                                                          unit_cfg.cucp_cfg.metrics,
                                                          unit_cfg.e2_cfg.base_config.enable_unit_e2,
                                                          e2_metric_connectors->get_e2_metric_notifier(0));

  // Create N2 Client Gateways.
  std::vector<std::unique_ptr<ocucp::n2_connection_client>> n2_clients;

  n2_clients.push_back(
      ocucp::create_n2_connection_client(generate_n2_client_config(cucp_unit_cfg.amf_config.no_core,
                                                                   cucp_unit_cfg.amf_config.amf,
                                                                   *dependencies.ngap_pcap,
                                                                   *dependencies.broker,
                                                                   dependencies.executor_mapper->n2_rx_executor())));

  for (const auto& amf : cucp_unit_cfg.extra_amfs) {
    n2_clients.push_back(
        ocucp::create_n2_connection_client(generate_n2_client_config(cucp_unit_cfg.amf_config.no_core,
                                                                     amf,
                                                                     *dependencies.ngap_pcap,
                                                                     *dependencies.broker,
                                                                     dependencies.executor_mapper->n2_rx_executor())));
  }

  cu_cp_cfg.ngap.n2_gws.reserve(n2_clients.size());
  for (const auto& n2_client : n2_clients) {
    cu_cp_cfg.ngap.n2_gws.push_back(n2_client.get());
  }

  ocucp::o_cu_cp_dependencies ocu_dependencies;
  if (unit_cfg.e2_cfg.base_config.enable_unit_e2) {
    ocudu_assert(!cucp_unit_cfg.amf_config.amf.supported_tas.empty() &&
                     !cucp_unit_cfg.amf_config.amf.supported_tas.front().plmn_list.empty(),
                 "CU-CP AMF config must have at least one supported TA with a PLMN");
    o_cu_cp_cfg.e2ap_cfg =
        generate_e2_config(unit_cfg.e2_cfg,
                           cucp_unit_cfg.gnb_id,
                           cucp_unit_cfg.amf_config.amf.supported_tas.front().plmn_list.front().plmn_id);
    ocu_dependencies.e2_client          = dependencies.e2_gw;
    ocu_dependencies.e2_cu_metric_iface = &e2_metric_connectors->get_e2_metrics_interface(0);
  }

  ocucp.unit = std::make_unique<o_cu_cp_unit_impl>(
      std::move(n2_clients), std::move(e2_metric_connectors), ocucp::create_o_cu_cp(o_cu_cp_cfg, ocu_dependencies));

  // Add the commands;
  ocucp.commands.cmdline.commands.push_back(
      std::make_unique<handover_app_command>(ocucp.unit->get_cu_cp().get_command_handler()));
  ocucp.commands.cmdline.commands.push_back(
      std::make_unique<cho_app_command>(ocucp.unit->get_cu_cp().get_command_handler(),
                                        std::chrono::milliseconds{cucp_unit_cfg.mobility_config.cho_timeout_ms}));
  ocucp.commands.cmdline.commands.push_back(
      std::make_unique<release_app_command>(ocucp.unit->get_cu_cp().get_command_handler()));

  // Add the remote WS commands.
  ocucp.commands.remote.push_back(
      std::make_unique<cell_lock_remote_command>(ocucp.unit->get_cu_cp().get_command_handler()));
  ocucp.commands.remote.push_back(
      std::make_unique<cell_unlock_remote_command>(ocucp.unit->get_cu_cp().get_command_handler()));
  ocucp.commands.remote.push_back(
      std::make_unique<cell_bar_remote_command>(ocucp.unit->get_cu_cp().get_command_handler()));
  ocucp.commands.remote.push_back(
      std::make_unique<cell_unbar_remote_command>(ocucp.unit->get_cu_cp().get_command_handler()));

  return ocucp;
}
