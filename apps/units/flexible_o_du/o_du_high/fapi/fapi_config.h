// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ocudulog/logger.h"

namespace ocudu {

/// FAPI configuration.
struct fapi_unit_config {
  /// FAPI log level.
  ocudulog::basic_levels fapi_level = ocudulog::basic_levels::warning;
  /// Flags that indicates if realtime mode is enabled or not.
  bool is_rt_mode_enabled;
};

} // namespace ocudu
