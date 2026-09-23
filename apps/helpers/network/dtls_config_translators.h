// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "apps/helpers/network/dtls_appconfig.h"
#include "ocudu/gateways/dtls_context_config.h"
#include <chrono>

namespace ocudu {

/// Applies DTLS options from the application DTLS config to the gateway config.
/// If parameter was configured as -1, std::optional won't be initialized and system default will be used instead.
inline void fill_dtls_network_gateway_config_params(dtls_context_config& dtls_cfg, const dtls_appconfig& app_cfg)
{
  dtls_cfg.mode             = app_cfg.mode == dtls_appconfig_mode::server ? dtls_mode::server : dtls_mode::client;
  dtls_cfg.session_id       = app_cfg.mode == dtls_appconfig_mode::server ? "1" : "2";
  dtls_cfg.cert_filename    = app_cfg.cert_filename;
  dtls_cfg.key_filename     = app_cfg.key_filename;
  dtls_cfg.ca_cert_filename = app_cfg.ca_cert_filename;
}

} // namespace ocudu
