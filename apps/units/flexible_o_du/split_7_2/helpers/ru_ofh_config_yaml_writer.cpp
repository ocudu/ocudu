// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ru_ofh_config_yaml_writer.h"
#include "apps/helpers/metrics/metrics_config_yaml_writer.h"
#include "ru_ofh_config.h"
#include "ocudu/support/math/math_utils.h"

using namespace ocudu;

static void fill_ru_ofh_log_section(YAML::Node node, const ru_ofh_unit_logger_config& config)
{
  node["ofh_level"] = ocudulog::basic_level_to_string(config.ofh_level);
}

static void fill_ru_ofh_expert_execution_section(YAML::Node node, const ru_ofh_unit_expert_execution_config& config)
{
  YAML::Node affinities_node = node["affinities"];
  YAML::Node ofh_node        = affinities_node["ofh"];
  if (!config.ru_timing_cpu.get_cpu_ids().empty()) {
    ofh_node["timing_cpu"] = fmt::format("{:,}", span<const size_t>(config.ru_timing_cpu.get_cpu_ids()));
  }

  if (config.txrx_affinities.size() > 0) {
    YAML::Node txrx_node = ofh_node["txrx_cpus"];
    for (const auto& affinity : config.txrx_affinities) {
      txrx_node.push_back(fmt::format("{:,}", span<const size_t>(affinity.get_cpu_ids())));
    }
  }

  auto cell_affinities_node = node["cell_affinities"];
  while (config.cell_affinities.size() > cell_affinities_node.size()) {
    cell_affinities_node.push_back(YAML::Node());
  }

  unsigned index = 0;
  for (auto cell : cell_affinities_node) {
    const auto& expert = config.cell_affinities[index];

    if (expert.ru_cpu_cfg.mask.any()) {
      cell["ru_cpus"] = fmt::format("{:,}", span<const size_t>(expert.ru_cpu_cfg.mask.get_cpu_ids()));
    }
    cell["ru_pinning"] = to_string(expert.ru_cpu_cfg.pinning_policy);

    ++index;
  }

  YAML::Node threads_node            = node["threads"];
  YAML::Node ofh_threads             = threads_node["ofh"];
  ofh_threads["enable_busy_waiting"] = config.enable_busy_waiting;
}

static void fill_ru_ofh_hal_section(YAML::Node node, const std::optional<ru_ofh_unit_hal_config>& config)
{
  if (!config.has_value()) {
    return;
  }
  YAML::Node hal_node  = node["hal"];
  hal_node["eal_args"] = config.value().eal_args;
}

static unsigned translate_c_plane_prach_fft_len(ofh::cplane_fft_size c_plane_prach_fft_len)
{
  switch (c_plane_prach_fft_len) {
    case ofh::cplane_fft_size::fft_noop:
      return 0;
    case ofh::cplane_fft_size::fft_1536:
      return 1536;
    case ofh::cplane_fft_size::fft_3072:
      return 3072;
    default:
      return pow2(static_cast<unsigned>(c_plane_prach_fft_len));
  }
}

/// Fills only the base-cell options. These are accepted by the CLI schema both at the ru_ofh top level
/// (as the common template) and inside each cells[] entry.
static void fill_ru_ofh_base_cell_section(YAML::Node node, const ru_ofh_unit_base_cell_config& config)
{
  if (config.ru_operating_bw.has_value()) {
    node["ru_bandwidth_MHz"] = bs_channel_bandwidth_to_MHz(*config.ru_operating_bw);
  }
  node["t1a_max_cp_dl"]              = config.T1a_max_cp_dl.count();
  node["t1a_min_cp_dl"]              = config.T1a_min_cp_dl.count();
  node["t1a_max_cp_ul"]              = config.T1a_max_cp_ul.count();
  node["t1a_min_cp_ul"]              = config.T1a_min_cp_ul.count();
  node["t1a_max_up"]                 = config.T1a_max_up.count();
  node["t1a_min_up"]                 = config.T1a_min_up.count();
  node["ta4_max"]                    = config.Ta4_max.count();
  node["ta4_min"]                    = config.Ta4_min.count();
  node["is_prach_cp_enabled"]        = config.is_prach_control_plane_enabled;
  node["ignore_prach_start_symbol"]  = config.ignore_prach_start_symbol;
  node["ignore_ecpri_seq_id"]        = config.ignore_ecpri_seq_id_field;
  node["ignore_ecpri_payload_size"]  = config.ignore_ecpri_payload_size_field;
  node["log_lates_as_warnings"]      = config.enable_log_warnings_for_lates;
  node["warn_unreceived_ru_frames"]  = to_string(config.log_unreceived_ru_frames);
  node["compr_method_ul"]            = config.compression_method_ul;
  node["compr_bitwidth_ul"]          = config.compression_bitwidth_ul;
  node["compr_method_dl"]            = config.compression_method_dl;
  node["compr_bitwidth_dl"]          = config.compression_bitwidth_dl;
  node["compr_method_prach"]         = config.compression_method_prach;
  node["compr_bitwidth_prach"]       = config.compression_bitwidth_prach;
  node["enable_ul_static_compr_hdr"] = config.is_uplink_static_comp_hdr_enabled;
  node["enable_dl_static_compr_hdr"] = config.is_downlink_static_comp_hdr_enabled;
  node["cplane_prach_fft_len"]       = translate_c_plane_prach_fft_len(config.c_plane_prach_fft_len);
  if (const auto* scaling_params = std::get_if<ru_ofh_scaling_config>(&config.iq_scaling_config)) {
    node["ru_reference_level_dBFS"] = scaling_params->ru_reference_level_dBFS;

    if (scaling_params->subcarrier_rms_backoff_dB) {
      node["subcarrier_rms_backoff_dB"] = *scaling_params->subcarrier_rms_backoff_dB;
    } else {
      node["subcarrier_rms_backoff_dB"] = "auto";
    }
  } else if (const auto* legacy_scaling_params = std::get_if<ru_ofh_legacy_scaling_config>(&config.iq_scaling_config)) {
    node["iq_scaling"] = legacy_scaling_params->iq_scaling;
  }
}

static YAML::Node build_ru_ofh_cell_section(const ru_ofh_unit_cell_config& config)
{
  YAML::Node node;

  fill_ru_ofh_base_cell_section(node, config.cell);
  node["network_interface"]  = config.network_interface;
  node["enable_promiscuous"] = config.enable_promiscuous_mode;
  node["mtu"]                = config.mtu_size.value();
  node["ru_mac_addr"]        = config.ru_mac_address;
  node["du_mac_addr"]        = config.du_mac_address;
  node["check_link_status"]  = config.check_link_status;

  if (config.vlan_tag_cp.has_value()) {
    node["vlan_tag_cp"] = *config.vlan_tag_cp;
  }
  if (config.vlan_tag_up.has_value()) {
    node["vlan_tag_up"] = *config.vlan_tag_up;
  }
  if (config.vlan_pcp_cp.has_value()) {
    node["vlan_pcp_cp"] = static_cast<unsigned>(*config.vlan_pcp_cp);
  }
  if (config.vlan_pcp_up.has_value()) {
    node["vlan_pcp_up"] = static_cast<unsigned>(*config.vlan_pcp_up);
  }

  node["prach_port_id"] = config.ru_prach_port_id;
  node["prach_port_id"].SetStyle(YAML::EmitterStyle::Flow);

  node["dl_port_id"] = config.ru_dl_port_id;
  node["dl_port_id"].SetStyle(YAML::EmitterStyle::Flow);

  node["ul_port_id"] = config.ru_ul_port_id;
  node["ul_port_id"].SetStyle(YAML::EmitterStyle::Flow);

  return node;
}

static void fill_ru_ofh_section(YAML::Node node, const ru_ofh_unit_config& config)
{
  node["gps_alpha"] = config.gps_Alpha;
  node["gps_beta"]  = config.gps_Beta;

  // Mirror the first cell's base configuration at the ru_ofh top level. The CLI schema treats those
  // fields as the common template that propagates to every cell during parsing, so emitting them here
  // preserves the user-facing keys the schema documents.
  if (!config.cells.empty()) {
    fill_ru_ofh_base_cell_section(node, config.cells.front().cell);
  }

  for (const auto& cell : config.cells) {
    node["cells"].push_back(build_ru_ofh_cell_section(cell));
  }
}

static void fill_ru_ofh_metrics_section(YAML::Node node, const ru_ofh_unit_metrics_config& config)
{
  app_helpers::fill_metrics_appconfig_in_yaml_schema(node, config.metrics_cfg);

  auto metrics_node        = node["metrics"];
  auto layers_node         = metrics_node["layers"];
  layers_node["enable_ru"] = config.enable_ru_metrics;
}

void ocudu::fill_ru_ofh_config_in_yaml_schema(YAML::Node& node, const ru_ofh_unit_config& config)
{
  fill_ru_ofh_metrics_section(node, config.metrics_cfg);
  fill_ru_ofh_log_section(node["log"], config.loggers);
  fill_ru_ofh_expert_execution_section(node["expert_execution"], config.expert_execution_cfg);
  fill_ru_ofh_section(node["ru_ofh"], config);
  fill_ru_ofh_hal_section(node, config.hal_config);
}
