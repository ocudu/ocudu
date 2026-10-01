// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "apps/helpers/network/sctp_appconfig.h"
#include <cstdint>
#include <string>
#include <vector>

namespace ocudu {

/// E2 Agent configuration.
struct e2_config {
  /// Whether to enable E2 agent.
  bool enable_unit_e2 = false;
  /// RIC IP addresses.
  std::vector<std::string> ip_addrs = {"127.0.0.1"};
  /// RIC port.
  uint16_t port = 36421;
  /// Local IP addresses to bind for RIC connection.
  std::vector<std::string> bind_addrs = {"127.0.0.1"};
  /// Configuration for SCTP socket parameters.
  sctp_appconfig sctp;
  /// Whether to enable KPM service module.
  bool e2sm_kpm_enabled = false;
  /// Whether to enable RC service module.
  bool e2sm_rc_enabled = false;
  /// Whether to enable CCC service module.
  bool e2sm_ccc_enabled = false;
};

} // namespace ocudu
