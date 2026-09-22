// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "du_high/du_high_config_cli11_schema.h"
#include "e2/o_du_high_e2_config_cli11_schema.h"
#include "fapi/fapi_config_cli11_schema.h"
#include "o_du_high_unit_config.h"
#include "CLI/CLI11.hpp"

namespace ocudu {

/// Configures the given CLI11 application with the O-DU high configuration schema.
inline void configure_cli11_with_o_du_high_config_schema(CLI::App& app, o_du_high_unit_config& unit_cfg)
{
  configure_cli11_with_du_high_config_schema(app, unit_cfg.du_high_cfg);
  configure_cli11_with_fapi_config_schema(app, unit_cfg.fapi_cfg);
  configure_cli11_with_o_du_high_e2_config_schema(app, unit_cfg.e2_cfg);
}

/// Auto derive O-DU high parameters after the parsing.
inline void
autoderive_o_du_high_parameters_after_parsing(CLI::App& app, o_du_high_unit_config& unit_cfg, bool is_rt_mode_enabled)
{
  autoderive_du_high_parameters_after_parsing(unit_cfg.du_high_cfg.config);
  autoderive_o_du_high_e2_parameters_after_parsing(unit_cfg.e2_cfg);
  unit_cfg.fapi_cfg.is_rt_mode_enabled = is_rt_mode_enabled;
}

} // namespace ocudu
