// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "apps/units/flexible_o_du/o_du_low/du_low_logger_registrator.h"
#include "apps/units/flexible_o_du/split_7_2/helpers/ru_ofh_logger_registrator.h"
#include "apps/units/flexible_o_du/split_8/helpers/ru_sdr_logger_registrator.h"
#include "split6_o_du_low_unit_config.h"
#include <variant>

namespace ocudu {

/// Registers all the loggers for the O-RAN DU low split 6.
inline void register_split6_o_du_low_loggers(const split6_o_du_low_unit_config& config)
{
  auto& fapi = ocudulog::fetch_basic_logger("FAPI", true);
  fapi.set_level(config.fapi_level);

  register_du_low_loggers(config.du_low_cfg.loggers);

  if (const auto* ru_sdr = std::get_if<ru_sdr_unit_config>(&config.ru_cfg)) {
    register_ru_sdr_logs(ru_sdr->loggers);
  } else if (const auto* ru_ofh = std::get_if<ru_ofh_unit_parsed_config>(&config.ru_cfg)) {
    register_ru_ofh_loggers(ru_ofh->config.loggers);
  }
}

} // namespace ocudu
