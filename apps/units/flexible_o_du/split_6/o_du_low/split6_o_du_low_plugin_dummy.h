// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "split6_o_du_low_plugin.h"

namespace ocudu {

struct worker_manager_config;

/// Dummy split 6 O-DU low plugin implementation.
class split6_o_du_low_plugin_dummy : public split6_o_du_low_plugin
{
public:
  // See interface for documentation.
  void on_parsing_configuration_registration(CLI::App& app) override {}

  // See interface for documentation.
  void on_configuration_parameters_autoderivation(CLI::App& app) override {}

  // See interface for documentation.
  bool on_configuration_validation() const override { return false; }

  // See interface for documentation.
  void on_loggers_registration() override {}

  // See interface for documentation.
  std::unique_ptr<fapi_adaptor::mac_fapi_p5_sector_adaptor>
  create_fapi_p5_sector_adaptor(fapi::p5_requests_gateway& p5_gateway,
                                task_executor&             executor,
                                task_executor&             control_executor) override;

  // See interface for documentation.
  std::unique_ptr<fapi_adaptor::mac_fapi_p7_sector_adaptor_factory>
  create_fapi_p7_sector_adaptor_factory(task_executor& executor, task_executor& control_executor) override;

  // See interface for documentation.
  void fill_worker_manager_config(worker_manager_config& config) override {}
};

} // namespace ocudu
