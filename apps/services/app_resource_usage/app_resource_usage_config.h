// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "apps/helpers/metrics/metrics_config.h"

namespace ocudu {
namespace app_services {

/// Application resource usage configuration.
struct app_resource_usage_config {
  app_helpers::metrics_config metrics_consumers_cfg;
  bool                        enable_app_usage = false;
};

} // namespace app_services
} // namespace ocudu
