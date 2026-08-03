// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "apps/helpers/metrics/metrics_config.h"
#include "ocudu/adt/byte_buffer.h"

namespace ocudu {
namespace app_services {

/// Buffer pool metrics configuration.
struct buffer_pool_metrics_config {
  /// Common metrics config.
  app_helpers::metrics_config common_metrics_cfg;
  /// Defines whether to log buffer pool metrics.
  bool enable_metrics = false;
};

/// Buffer pool application configuration.
struct buffer_pool_appconfig {
  std::size_t nof_segments  = 1048576;
  std::size_t segment_size  = byte_buffer_segment_pool_default_segment_size();
  bool        use_hugepages = false;
  /// Metrics config.
  buffer_pool_metrics_config metrics_config;
};

} // namespace app_services
} // namespace ocudu
