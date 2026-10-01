// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "f1u_cli11_schema.h"
#include "apps/helpers/f1u/f1u_appconfig.h"
#include "apps/helpers/network/udp_cli11_schema.h"
#include "ocudu/support/cli11_utils.h"
#include "ocudu/support/config_parsers.h"

using namespace ocudu;

static void configure_cli11_f1u_socket_args(CLI::App& app, f1u_socket_appconfig& f1u_cfg)
{
  add_option(app,
             "--bind_addr",
             f1u_cfg.bind_addr,
             "Default local IP address interfaces bind to, unless a specific bind address is specified")
      ->check(CLI::ValidIPV4);

  add_option(app, "--sst", f1u_cfg.sst, "Slice Service Type")->capture_default_str()->range(0, 255);
  add_option(app, "--sd", f1u_cfg.sd, "Service Differentiator")->capture_default_str()->range(0, 0xfffffe);
  add_option(app, "--five_qi", f1u_cfg.five_qi, "Assign this socket to a specific 5QI")->range(0, 255);

  configure_cli11_with_udp_config_schema(app, f1u_cfg.udp_config);
}

void ocudu::configure_cli11_f1u_sockets_args(CLI::App& app, f1u_sockets_appconfig& f1u_params)
{
  // Add configurable F1-U bind port. Default port is 2152 as per TS 29.281 Sec. 4.4.2.3.
  add_option(app, "--bind_port", f1u_params.bind_port, "F1-U bind port")->capture_default_str();
  // Add configurable F1-U peer port. Default port is 2152 as per TS 29.281 Sec. 4.4.2.3.
  add_option(app, "--peer_port", f1u_params.peer_port, "F1-U peer port")->capture_default_str();

  // Add option for multiple sockets, for usage with different slices, 5QIs or parallization.
  add_option_object_list<f1u_socket_appconfig>(app,
                                               "--socket",
                                               f1u_params.f1u_socket_cfg,
                                               configure_cli11_f1u_socket_args,
                                               "Configures UDP/IP socket parameters of the F1-U interface");
}
