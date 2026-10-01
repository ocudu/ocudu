// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "apps/services/metrics/metrics_config.h"
#include "apps/units/application_unit.h"
#include "split6_o_du_low_plugin.h"
#include "split6_o_du_low_unit_config.h"
#include "ocudu/du/du_operation_controller.h"
#include <memory>

namespace ocudu {

class timer_manager;
struct worker_manager;

namespace app_services {
class metrics_notifier;
class remote_server_metrics_gateway;
} // namespace app_services

struct split6_o_du_low_unit {
  std::unique_ptr<du_operation_controller>  odu_low;
  std::vector<app_services::metrics_config> metrics;
};

/// O-RAN DU low Split6 application unit implementation.
class split6_o_du_low_application_unit_impl : public application_unit
{
public:
  split6_o_du_low_application_unit_impl(std::string_view app_name, std::unique_ptr<split6_o_du_low_plugin> plugin_);

  // See interface for documentation.
  void on_parsing_configuration_registration(CLI::App& app) override;

  // See interface for documentation.
  void on_configuration_parameters_autoderivation(CLI::App& app) override;

  /// Validates the configuration of this application unit. Returns true on success, otherwise false.
  bool on_configuration_validation() const;

  // See interface for documentation.
  void on_loggers_registration() override;

  // See interface for documentation.
  bool are_metrics_enabled() const override;

  // See interface for documentation.
  void dump_config(YAML::Node& node) const override;

  // See interface for documentation.
  void fill_worker_manager_config(worker_manager_config& config) override;

  /// Creates and returns the low FAPI control adaptor of this application unit.
  split6_o_du_low_unit create_flexible_o_du_low(worker_manager&                              workers,
                                                app_services::metrics_notifier&              metrics_notifier,
                                                app_services::remote_server_metrics_gateway* remote_metrics_gateway,
                                                timer_manager&                               timers,
                                                ocudulog::basic_logger&                      logger);

private:
  split6_o_du_low_unit_config             unit_cfg;
  std::unique_ptr<split6_o_du_low_plugin> plugin;
};

std::unique_ptr<split6_o_du_low_application_unit_impl>
create_flexible_o_du_low_application_unit(std::string_view app_name);

} // namespace ocudu
