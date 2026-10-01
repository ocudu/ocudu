// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ru_sdr_config.h"

namespace ocudu {

/// Registers the SDR Radio Unit loggers in the logger service.
inline void register_ru_sdr_logs(const ru_sdr_unit_logger_config& log_cfg)
{
  ocudulog::basic_logger& phy_logger = ocudulog::fetch_basic_logger("PHY", true);
  phy_logger.set_level(log_cfg.phy_level);

  ocudulog::basic_logger& rf_logger = ocudulog::fetch_basic_logger("RF", false);
  rf_logger.set_level(log_cfg.radio_level);

  ocudulog::basic_logger& ru_logger = ocudulog::fetch_basic_logger("RU", true);
  ru_logger.set_level(log_cfg.radio_level);
}

} // namespace ocudu
