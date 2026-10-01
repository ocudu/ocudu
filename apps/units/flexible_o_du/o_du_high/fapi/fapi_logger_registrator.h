// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "fapi_config.h"

namespace ocudu {

/// Registers the fapi loggers in the logger service.
inline void register_fapi_loggers(const fapi_unit_config& log_cfg)
{
  auto& fapi_logger = ocudulog::fetch_basic_logger("FAPI", true);
  fapi_logger.set_level(log_cfg.fapi_level);
}

} // namespace ocudu
