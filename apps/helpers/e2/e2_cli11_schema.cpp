// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "e2_cli11_schema.h"
#include "apps/helpers/e2/e2_appconfig.h"
#include "apps/helpers/network/sctp_cli11_schema.h"
#include "ocudu/support/cli11_utils.h"

using namespace ocudu;

static void configure_cli11_e2_args(CLI::App&          app,
                                    e2_config&         e2_params,
                                    const std::string& option_name,
                                    const std::string& option_description)
{
  add_option(app, option_name, e2_params.enable_unit_e2, option_description)->capture_default_str();
  add_option(app,
             "--addrs,--addr",
             e2_params.ip_addrs,
             "RIC addresses to be used for E2 interface. Multiple addresses can be specified for SCTP multi-homing. "
             "The '--addr' name is a deprecated alias and should not be used.")
      ->capture_default_str();
  add_option(app, "--port", e2_params.port, "RIC port")->range(20000, 40000)->capture_default_str();
  add_option(
      app,
      "--bind_addrs,--bind_addr",
      e2_params.bind_addrs,
      "Local bind addresses to be used for E2 interface. Multiple addresses can be specified for SCTP "
      "multi-homing. If left empty, implicit bind is performed. The '--bind_addr' name is a deprecated alias and "
      "should not be used.")
      ->capture_default_str();
  configure_cli11_sctp_socket_args(app, e2_params.sctp);
  add_option(app, "--e2sm_kpm_enabled", e2_params.e2sm_kpm_enabled, "Enable KPM service module")->capture_default_str();
  add_option(app, "--e2sm_rc_enabled", e2_params.e2sm_rc_enabled, "Enable RC service module")->capture_default_str();
  add_option(app, "--e2sm_ccc_enabled", e2_params.e2sm_ccc_enabled, "Enable CCC service module")->capture_default_str();
}

void ocudu::configure_cli11_with_e2_config_schema(CLI::App&          app,
                                                  e2_config&         config,
                                                  const std::string& option_name,
                                                  const std::string& option_description)
{
  CLI::App* e2_subcmd = add_subcommand(app, "e2", "E2 parameters")->configurable();
  configure_cli11_e2_args(*e2_subcmd, config, option_name, option_description);
}
