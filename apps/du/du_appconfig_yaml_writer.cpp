// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "du_appconfig_yaml_writer.h"
#include "apps/helpers/f1u/f1u_config_yaml_writer.h"
#include "apps/helpers/hal/hal_appconfig_yaml_writer.h"
#include "apps/helpers/logger/logger_appconfig_yaml_writer.h"
#include "apps/helpers/network/sctp_config_yaml_writer.h"
#include "apps/helpers/tracing/tracer_appconfig_yaml_writer.h"
#include "apps/services/app_execution_metrics/executor_metrics_config_yaml_writer.h"
#include "apps/services/app_resource_usage/app_resource_usage_config_yaml_writer.h"
#include "apps/services/buffer_pool/buffer_pool_config_yaml_writer.h"
#include "apps/services/metrics/metrics_config_yaml_writer.h"
#include "du_appconfig.h"

using namespace ocudu;

static void fill_du_appconfig_hal_section(YAML::Node node, const std::optional<hal_appconfig>& config)
{
  if (!config.has_value()) {
    return;
  }
  fill_hal_appconfig_section(node, *config);
}

static void fill_du_appconfig_expert_execution_section(YAML::Node node, const expert_execution_appconfig& config)
{
  {
    YAML::Node affinities_node = node["affinities"];

    if (config.affinities.main_pool_cpu_cfg.mask.any()) {
      affinities_node["main_pool_cpus"] =
          fmt::format("{:n}", span<const size_t>(config.affinities.main_pool_cpu_cfg.mask.get_cpu_ids()));
    }
    affinities_node["main_pool_pinning"] = to_string(config.affinities.main_pool_cpu_cfg.pinning_policy);
  }

  {
    YAML::Node threads_node   = node["threads"];
    YAML::Node main_pool_node = threads_node["main_pool"];
    if (config.threads.main_pool.nof_threads.has_value()) {
      main_pool_node["nof_threads"] = config.threads.main_pool.nof_threads.value();
    }
    main_pool_node["task_queue_size"] = config.threads.main_pool.task_queue_size;
    main_pool_node["backoff_period"]  = config.threads.main_pool.backoff_period;
  }
}

static void fill_du_appconfig_remote_control_section(YAML::Node node, const remote_control_appconfig& config)
{
  node["enabled"]   = config.enabled;
  node["bind_addr"] = config.bind_addr;
  node["port"]      = config.port;
}

static void fill_du_appconfig_f1u_section(YAML::Node node, const odu::f1u_appconfig& config)
{
  node["queue_size"] = config.pdu_queue_size;
  fill_f1u_config_yaml_schema(node, config.f1u_sockets);
}

static void fill_du_appconfig_f1ap_section(YAML::Node node, const odu::f1ap_appconfig& config)
{
  node["addrs"]            = config.cu_cp_addresses;
  node["bind_addrs"]       = config.bind_addresses;
  node["retry_connection"] = config.retry_connection;
  fill_sctp_config_in_yaml_schema(node, config.sctp);
}

void ocudu::fill_du_appconfig_in_yaml_schema(YAML::Node& node, const du_appconfig& config)
{
  app_services::fill_app_resource_usage_config_in_yaml_schema(node, config.metrics_cfg.rusage_config);
  app_services::fill_metrics_appconfig_in_yaml_schema(node, config.metrics_cfg.metrics_service_cfg);
  app_services::fill_app_exec_metrics_config_in_yaml_schema(node, config.metrics_cfg.executors_metrics_cfg);
  node["metrics"]["autostart_stdout_metrics"] = config.metrics_cfg.autostart_stdout_metrics;
  app_services::fill_buffer_pool_config_in_yaml_schema(node, config.buffer_pool_config);
  fill_logger_appconfig_in_yaml_schema(node, config.log_cfg);
  fill_tracer_appconfig_in_yaml_schema(node, config.trace_cfg);
  fill_du_appconfig_hal_section(node, config.hal_config);
  // TODO: move duplicated cu/cu_cp/cu_up/du YAML writers to helpers/app_services
  fill_du_appconfig_expert_execution_section(node["expert_execution"], config.expert_execution_cfg);
  fill_du_appconfig_remote_control_section(node["remote_control"], config.remote_control_config);
  fill_du_appconfig_f1u_section(node["f1u"], config.f1u_cfg);
  fill_du_appconfig_f1ap_section(node["f1ap"], config.f1ap_cfg);
}
