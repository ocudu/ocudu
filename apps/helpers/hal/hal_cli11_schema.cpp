// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "hal_cli11_schema.h"
#include "hal_appconfig.h"
#include "ocudu/support/cli11_utils.h"

using namespace ocudu;

static void configure_cli11_hal_args(CLI::App& app, hal_appconfig& config)
{
  add_option(app, "--eal_args", config.eal_args, "EAL configuration parameters used to initialize DPDK");
  add_option(app, "--enable_pdump_init", config.enable_pdump_init, "Enable DPDK pdump initialization")
      ->always_capture_default();
}

void ocudu::configure_cli11_with_hal_appconfig_schema(CLI::App& app, hal_appconfig& config)
{
  CLI::App* hal_subcmd = add_subcommand(app, "hal", "HAL configuration")->configurable();
  configure_cli11_hal_args(*hal_subcmd, config);
}

bool ocudu::is_hal_section_present(CLI::App& app)
{
  auto subcmd = app.get_subcommand("hal");
  return subcmd->count_all() != 0;
}
