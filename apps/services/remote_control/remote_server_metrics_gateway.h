// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include <string>

namespace ocudu {
namespace app_services {

/// Remote server metrics gateway.
class remote_server_metrics_gateway
{
public:
  virtual ~remote_server_metrics_gateway() = default;

  /// Sends the given metrics through the gateway.
  virtual void send(std::string metrics) = 0;
};

} // namespace app_services
} // namespace ocudu
