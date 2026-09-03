// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "xnu_cli11_schema.h"
#include "apps/helpers/network/udp_cli11_schema.h"
#include "apps/helpers/xnu/xnu_appconfig.h"
#include "ocudu/support/cli11_utils.h"
#include "ocudu/support/config_parsers.h"

using namespace ocudu;

static void configure_cli11_xnu_socket_args(CLI::App& app, xnu_socket_appconfig& xnu_cfg)
{
  add_option(app,
             "--bind_addr",
             xnu_cfg.bind_addr,
             "Default local IP address interfaces bind to, unless a specific bind address is specified")
      ->check(CLI::ValidIPV4);

  add_option(app, "--sst", xnu_cfg.sst, "Slice Service Type")->capture_default_str()->range(0, 255);
  add_option(app, "--sd", xnu_cfg.sd, "Service Differentiator")->capture_default_str()->range(0, 0xfffffe);
  add_option(app, "--five_qi", xnu_cfg.five_qi, "Assign this socket to a specific 5QI")->range(0, 255);

  configure_cli11_with_udp_config_schema(app, xnu_cfg.udp_config);
}

void ocudu::configure_cli11_xnu_sockets_args(CLI::App& app, xnu_sockets_appconfig& xnu_params)
{
  // Add configurable Xn-U bind port. Default port is 2152 as per TS 29.281 Sec. 4.4.2.3.
  add_option(app, "--bind_port", xnu_params.bind_port, "Xn-U bind port")->capture_default_str();
  // Add configurable Xn-U peer port. Default port is 2152 as per TS 29.281 Sec. 4.4.2.3.
  add_option(app, "--peer_port", xnu_params.peer_port, "Xn-U peer port")->capture_default_str();

  // Add option for multiple sockets, for usage with different slices, 5QIs or parallization.
  add_option_object_list<xnu_socket_appconfig>(app,
                                               "--socket",
                                               xnu_params.xnu_socket_cfg,
                                               configure_cli11_xnu_socket_args,
                                               "Configures UDP/IP socket parameters of the Xn-U interface");
}
