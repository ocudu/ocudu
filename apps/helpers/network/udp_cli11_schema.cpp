// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "udp_cli11_schema.h"
#include "udp_appconfig.h"
#include "ocudu/support/cli11_utils.h"

using namespace ocudu;

static void configure_cli11_udp_args(CLI::App& app, udp_appconfig& udp_params)
{
  add_option(app, "--max_rx_msgs", udp_params.rx_max_msgs, "Maximum amount of messages RX in a single syscall")
      ->capture_default_str();
  add_option(app, "--tx_qsize", udp_params.tx_qsize, "Size of TX queue used for batching SDUs.")->capture_default_str();
  add_option(app, "--max_tx_msgs", udp_params.tx_max_msgs, "Maximum amount of messages TX in a single syscall")
      ->capture_default_str();
  add_option(app, "--max_tx_segments", udp_params.tx_max_segments, "Maximum amount of segments TX in a single SDU")
      ->capture_default_str();
  add_option(
      app, "--pool_threshold", udp_params.pool_threshold, "Pool accupancy threshold after which packets are dropped")
      ->capture_default_str();
  add_option(app, "--reuse_addr", udp_params.reuse_addr, "Allow multiple sockets to bind to the same port.")
      ->capture_default_str();
  add_option(app, "--dscp", udp_params.dscp, "Differentiated Services Code Point value.")
      ->capture_default_str()
      ->range(0, 63);
}

void ocudu::configure_cli11_with_udp_config_schema(CLI::App& app, udp_appconfig& config)
{
  add_option(app, "--ext_addr", config.ext_addr, "External IP address that is advertised for receiving UDP packets.")
      ->check(CLI::ValidIPV4 | CLI::IsMember({"auto"}));

  CLI::App* udp_subcmd = add_subcommand(app, "udp", "UDP parameters")->configurable();
  configure_cli11_udp_args(*udp_subcmd, config);
}
