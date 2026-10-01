// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "fapi_config_cli11_schema.h"
#include "apps/helpers/logger/logger_appconfig_cli11_utils.h"
#include "fapi_config.h"
#include "ocudu/support/cli11_utils.h"

using namespace ocudu;

static void configure_cli11_log_args(CLI::App& app, fapi_unit_config& log_params)
{
  app_helpers::add_log_option(app, log_params.fapi_level, "--fapi_level", "FAPI log level");
}

void ocudu::configure_cli11_with_fapi_config_schema(CLI::App& app, fapi_unit_config& parsed_cfg)
{
  // Loggers section.
  CLI::App* log_subcmd = add_subcommand(app, "log", "Logging configuration")->configurable();
  configure_cli11_log_args(*log_subcmd, parsed_cfg);
}
