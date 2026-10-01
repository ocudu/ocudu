// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/fapi_adaptor/phy/phy_fapi_adaptor.h"
#include <memory>
#include <string_view>

namespace CLI {
class App;
} // namespace CLI

namespace ocudu {

struct o_du_unit_dependencies;
struct worker_manager_config;

namespace fapi_adaptor {
struct split6_o_du_low_fapi_adaptor_configuration;
}

namespace odu {
struct du_high_ran_config;
}

/// \brief Split 6 plugin interface.
///
/// The plugin interface allows different implementations of the split 6.
class split6_plugin
{
public:
  virtual ~split6_plugin() = default;

  /// Registers the parsing configuration properties that will be used by this application unit.
  virtual void on_parsing_configuration_registration(CLI::App& app) = 0;

  /// Validates the configuration of this application unit. Returns true on success, otherwise false.
  virtual bool on_configuration_validation() const = 0;

  /// Checks if the given RAN configuration is supported by the plugin.
  virtual bool is_ran_config_supported(const odu::du_high_ran_config& configuration) const = 0;

  /// Registers the loggers of this application unit.
  virtual void on_loggers_registration() = 0;

  /// Creates and returns a vector of fapi adaptors, each of them representing a cell.
  virtual std::unique_ptr<fapi_adaptor::phy_fapi_adaptor>
  create_fapi_adaptor(const fapi_adaptor::split6_o_du_low_fapi_adaptor_configuration& fapi_cfg,
                      const o_du_unit_dependencies&                                   dependencies) = 0;

  /// Fills the given worker manager split 6 configuration.
  virtual void fill_worker_manager_config(worker_manager_config& config) = 0;
};

/// Creates the split 6 plugin.
std::unique_ptr<split6_plugin> create_split6_plugin(std::string_view app_name);

} // namespace ocudu
