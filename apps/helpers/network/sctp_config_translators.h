// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "apps/helpers/network/sctp_appconfig.h"
#include "ocudu/gateways/sctp_network_gateway.h"
#include <chrono>

namespace ocudu {

/// Applies SCTP socket options from the application SCTP config to the gateway config.
/// If parameter was configured as -1, std::optional won't be initialized and system default will be used instead.
inline void fill_sctp_network_gateway_config_socket_params(sctp_network_gateway_config& gw_cfg,
                                                           const sctp_appconfig&        app_cfg)
{
  if (app_cfg.rto_initial_ms >= 0) {
    gw_cfg.rto_initial = std::chrono::milliseconds(app_cfg.rto_initial_ms);
  }
  if (app_cfg.rto_min_ms >= 0) {
    gw_cfg.rto_min = std::chrono::milliseconds(app_cfg.rto_min_ms);
  }
  if (app_cfg.rto_max_ms >= 0) {
    gw_cfg.rto_max = std::chrono::milliseconds(app_cfg.rto_max_ms);
  }
  if (app_cfg.init_max_attempts >= 0) {
    gw_cfg.init_max_attempts = app_cfg.init_max_attempts;
  }
  if (app_cfg.max_init_timeo_ms >= 0) {
    gw_cfg.max_init_timeo = std::chrono::milliseconds(app_cfg.max_init_timeo_ms);
  }
  if (app_cfg.hb_interval_ms >= 0) {
    gw_cfg.hb_interval = std::chrono::milliseconds(app_cfg.hb_interval_ms);
  }
  if (app_cfg.assoc_max_retx >= 0) {
    gw_cfg.assoc_max_rxt = app_cfg.assoc_max_retx;
  }
  gw_cfg.nodelay = app_cfg.nodelay;
}

} // namespace ocudu
