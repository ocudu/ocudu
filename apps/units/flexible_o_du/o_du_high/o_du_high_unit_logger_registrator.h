// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "du_high/du_high_logger_registrator.h"
#include "du_high/ntn/ntn_logger_registrator.h"
#include "fapi/fapi_logger_registrator.h"
#include "o_du_high_unit_config.h"

namespace ocudu {

/// Checks if NTN is enabled in any cell.
static bool is_ntn_enabled(span<const du_high_unit_cell_config> cells_cfg)
{
  for (const auto& cell : cells_cfg) {
    if (cell.cell.ntn_cfg.has_value()) {
      return true;
    }
  }
  return false;
}

/// Registers the O-RAN DU high loggers in the logger service.
inline void register_o_du_high_loggers(const o_du_high_unit_config& unit_config)
{
  register_du_high_loggers(unit_config.du_high_cfg.config.loggers);

  // Register NTN loggers only if at least one cell has NTN config
  if (is_ntn_enabled(unit_config.du_high_cfg.config.cells_cfg)) {
    register_ntn_loggers(unit_config.du_high_cfg.config.loggers.ntn_level);
  }

  register_fapi_loggers(unit_config.fapi_cfg);
}

} // namespace ocudu
