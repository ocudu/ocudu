// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ru_ofh_config.h"

namespace ocudu {

/// Registers the Open Fronthaul Radio Unit loggers in the logger service.
inline void register_ru_ofh_loggers(const ru_ofh_unit_logger_config& log_cfg)
{
  ocudulog::basic_logger& ofh_logger = ocudulog::fetch_basic_logger("OFH", false);
  ofh_logger.set_level(log_cfg.ofh_level);
}

} // namespace ocudu
