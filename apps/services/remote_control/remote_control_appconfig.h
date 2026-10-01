// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include <cstdint>
#include <string>

namespace ocudu {

/// Remote control application configuration.
struct remote_control_appconfig {
  bool        enabled                     = false;
  std::string bind_addr                   = "127.0.0.1";
  uint16_t    port                        = 8001;
  bool        enable_metrics_subscription = false;
};

} // namespace ocudu
