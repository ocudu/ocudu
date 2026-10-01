// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "fapi_adaptor/mac_fapi_p7_sector_adaptor_factory.h"
#include "split6_flexible_o_du_low_session_factory.h"
#include "ocudu/fapi_adaptor/mac/p5/mac_fapi_p5_sector_adaptor.h"
#include <memory>

namespace CLI {
class App;
} // namespace CLI

namespace ocudu {

struct worker_manager_config;

/// \brief Split 6 O-DU low plugin interface.
///
/// The plugin interface allows different implementations of the split 6 O-DU low.
class split6_o_du_low_plugin
{
public:
  virtual ~split6_o_du_low_plugin() = default;

  /// Registers the parsing configuration properties that will be used by this application unit.
  virtual void on_parsing_configuration_registration(CLI::App& app) = 0;

  /// Auto derive configuration parameters.
  virtual void on_configuration_parameters_autoderivation(CLI::App& app) = 0;

  /// Validates the configuration of this application unit. Returns true on success, otherwise false.
  virtual bool on_configuration_validation() const = 0;

  /// Registers the loggers of this application unit.
  virtual void on_loggers_registration() = 0;

  /// Creates and returns a MAC-FAPI P5 sector adaptor using the given arguments.
  virtual std::unique_ptr<fapi_adaptor::mac_fapi_p5_sector_adaptor>
  create_fapi_p5_sector_adaptor(fapi::p5_requests_gateway& p5_gateway,
                                task_executor&             executor,
                                task_executor&             control_executor) = 0;

  /// Creates and returns a MAC-FAPI P7 sector adaptor factory using the given arguments.
  virtual std::unique_ptr<fapi_adaptor::mac_fapi_p7_sector_adaptor_factory>
  create_fapi_p7_sector_adaptor_factory(task_executor& executor, task_executor& control_executor) = 0;

  /// Fills the given worker manager split 6 configuration.
  virtual void fill_worker_manager_config(worker_manager_config& config) = 0;
};

/// Creates the split 6 O-DU low plugin.
std::unique_ptr<split6_o_du_low_plugin> create_split6_o_du_low_plugin(std::string_view app_name);

} // namespace ocudu
