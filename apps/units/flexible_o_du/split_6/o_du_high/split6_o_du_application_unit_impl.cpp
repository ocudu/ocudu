// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "split6_o_du_application_unit_impl.h"
#include "apps/helpers/e2/e2_metric_connector_manager.h"
#include "apps/units/flexible_o_du/o_du_high/du_high/du_high_config_translators.h"
#include "apps/units/flexible_o_du/o_du_high/o_du_high_unit_config_translators.h"
#include "apps/units/flexible_o_du/o_du_high/o_du_high_unit_config_yaml_writer.h"
#include "apps/units/flexible_o_du/split_6/o_du_high/split6_o_du_low_fapi_adaptor_configuration.h"
#include "split6_o_du_factory.h"
#include "split6_o_du_unit_cli11_schema.h"
#include "split6_o_du_unit_config_validator.h"
#include "split6_o_du_unit_logger_registrator.h"
#include "ocudu/du/du_high/du_high_configuration.h"

using namespace ocudu;

split6_o_du_application_unit_impl::split6_o_du_application_unit_impl(std::string_view               app_name,
                                                                     std::unique_ptr<split6_plugin> plugin_) :
  plugin(std::move(plugin_))
{
  ocudu_assert(plugin, "Invalid split 6 plugin");

  unit_cfg.odu_high_cfg.du_high_cfg.config.pcaps.f1ap.filename = fmt::format("/tmp/{}_f1ap.pcap", app_name);
  unit_cfg.odu_high_cfg.du_high_cfg.config.pcaps.f1u.filename  = fmt::format("/tmp/{}_f1u.pcap", app_name);
  unit_cfg.odu_high_cfg.du_high_cfg.config.pcaps.rlc.filename  = fmt::format("/tmp/{}_rlc.pcap", app_name);
  unit_cfg.odu_high_cfg.du_high_cfg.config.pcaps.mac.filename  = fmt::format("/tmp/{}_mac.pcap", app_name);
  // Note: do not update the default e2ap pcap filename.
}

void split6_o_du_application_unit_impl::on_loggers_registration()
{
  register_split6_o_du_loggers(unit_cfg);
  plugin->on_loggers_registration();
}

bool split6_o_du_application_unit_impl::are_metrics_enabled() const
{
  return unit_cfg.odu_high_cfg.du_high_cfg.config.metrics.layers_cfg.are_metrics_enabled();
}

void split6_o_du_application_unit_impl::on_configuration_parameters_autoderivation(CLI::App& app)
{
  autoderive_split6_o_du_parameters_after_parsing(app, unit_cfg);
}

bool split6_o_du_application_unit_impl::on_configuration_validation() const
{
  if (!plugin->on_configuration_validation()) {
    return false;
  }

  if (!validate_split6_o_du_unit_config(unit_cfg)) {
    return false;
  }

  // Get the du_high configuration.
  odu::du_high_configuration du_hi_cfg;
  generate_du_high_config(du_hi_cfg, unit_cfg.odu_high_cfg.du_high_cfg.config);

  return plugin->is_ran_config_supported(du_hi_cfg.ran);
}

void split6_o_du_application_unit_impl::on_parsing_configuration_registration(CLI::App& app)
{
  configure_cli11_with_split6_o_du_unit_config_schema(app, unit_cfg);
  plugin->on_parsing_configuration_registration(app);
}

o_du_unit split6_o_du_application_unit_impl::create_flexible_o_du_unit(const o_du_unit_dependencies& dependencies)
{
  // Get the du_high configuration.
  odu::du_high_configuration du_hi_cfg;
  generate_du_high_config(du_hi_cfg, unit_cfg.odu_high_cfg.du_high_cfg.config);

  // Get the du_high FAPI specific configuration.
  fapi_adaptor::split6_o_du_low_fapi_adaptor_configuration fapi_cfg;
  for (const auto& cell : du_hi_cfg.ran.cells) {
    fapi_cfg.cells.emplace_back(fapi_adaptor::split6_o_du_low_fapi_adaptor_cell_config{
        .scs_common = cell.ran.ul_cfg_common.init_ul_bwp.generic_params.scs,
        .num_tx_ant = static_cast<uint16_t>(cell.ran.ul_carrier.nof_ant)});
  }

  // Create the adaptors.
  auto fapi_ctrl = plugin->create_fapi_adaptor(fapi_cfg, dependencies);
  report_error_if_not(fapi_ctrl, "Could not create PHY-FAPI adaptor");
  auto du_impl = create_o_du_split6(unit_cfg, dependencies, std::move(fapi_ctrl));
  report_error_if_not(du_impl.unit, "Could not create split 6 DU");

  return du_impl;
}

void split6_o_du_application_unit_impl::dump_config(YAML::Node& node) const
{
  fill_o_du_high_config_in_yaml_schema(node, unit_cfg.odu_high_cfg);
}

void split6_o_du_application_unit_impl::fill_worker_manager_config(worker_manager_config& config)
{
  // Split 6 always runs in non blocking mode.
  bool is_blocking_mode_enable = false;
  fill_o_du_high_worker_manager_config(config, unit_cfg.odu_high_cfg, is_blocking_mode_enable);
  plugin->fill_worker_manager_config(config);
}

std::unique_ptr<flexible_o_du_application_unit> ocudu::create_flexible_o_du_application_unit(std::string_view app_name)
{
  return std::make_unique<split6_o_du_application_unit_impl>(app_name, create_split6_plugin(app_name));
}
