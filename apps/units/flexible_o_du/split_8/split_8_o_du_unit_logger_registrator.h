// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "apps/units/flexible_o_du/o_du_high/o_du_high_unit_logger_registrator.h"
#include "apps/units/flexible_o_du/o_du_low/du_low_logger_registrator.h"
#include "apps/units/flexible_o_du/split_8/helpers/ru_sdr_logger_registrator.h"
#include "split_8_o_du_unit_config.h"

namespace ocudu {

/// Registers all the loggers for the O-RAN DU split 8.
inline void register_split_8_o_du_loggers(const split_8_o_du_unit_config& config)
{
  register_o_du_high_loggers(config.odu_high_cfg);
  register_du_low_loggers(config.du_low_cfg.loggers);
  register_ru_sdr_logs(config.ru_cfg.loggers);
}

} // namespace ocudu
