// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "sctp_cli11_schema.h"
#include "sctp_appconfig.h"
#include "ocudu/support/cli11_utils.h"

using namespace ocudu;

void ocudu::configure_cli11_sctp_socket_args(CLI::App& app, sctp_appconfig& config)
{
  add_option(app,
             "--sctp_rto_initial",
             config.rto_initial_ms,
             "SCTP initial RTO value in milliseconds (-1 to use system default)");
  add_option(app, "--sctp_rto_min", config.rto_min_ms, "SCTP RTO min in milliseconds (-1 to use system default)");
  add_option(app, "--sctp_rto_max", config.rto_max_ms, "SCTP RTO max in milliseconds (-1 to use system default)");
  add_option(
      app, "--sctp_init_max_attempts", config.init_max_attempts, "SCTP init max attempts (-1 to use system default)");
  add_option(app,
             "--sctp_max_init_timeo",
             config.max_init_timeo_ms,
             "SCTP max init timeout in milliseconds (-1 to use system default)");
  add_option(app,
             "--sctp_hb_interval",
             config.hb_interval_ms,
             "SCTP heartbeat interval in milliseconds (-1 to use system default)")
      ->capture_default_str();
  add_option(app,
             "--sctp_assoc_max_retx",
             config.assoc_max_retx,
             "SCTP association max retransmissions (-1 to use system default)")
      ->capture_default_str();
  add_option(
      app, "--sctp_nodelay", config.nodelay, "Send SCTP messages as soon as possible without any Nagle-like algorithm");
}
