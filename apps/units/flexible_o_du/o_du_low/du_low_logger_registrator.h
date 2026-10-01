// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "du_low_config.h"

namespace ocudu {

/// Registers the DU low loggers in the logger service.
inline void register_du_low_loggers(const du_low_unit_logger_config& log_cfg)
{
  // Set layer-specific logging options.
  auto& phy_logger = ocudulog::fetch_basic_logger("PHY", true);
  phy_logger.set_level(log_cfg.phy_level);
  phy_logger.set_hex_dump_max_size(log_cfg.hex_max_size);

  ocudulog::basic_logger& hwacc_logger = ocudulog::fetch_basic_logger("HWACC", false);
  hwacc_logger.set_level(log_cfg.hal_level);
}

} // namespace ocudu
