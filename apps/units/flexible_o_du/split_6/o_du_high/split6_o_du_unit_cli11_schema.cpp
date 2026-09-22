// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "split6_o_du_unit_cli11_schema.h"
#include "apps/units/flexible_o_du/o_du_high/o_du_high_unit_config_cli11_schema.h"
#include "split6_o_du_unit_config.h"

using namespace ocudu;

void ocudu::configure_cli11_with_split6_o_du_unit_config_schema(CLI::App& app, split6_o_du_unit_config& config)
{
  configure_cli11_with_o_du_high_config_schema(app, config.odu_high_cfg);
}

void ocudu::autoderive_split6_o_du_parameters_after_parsing(CLI::App& app, split6_o_du_unit_config& config)
{
  autoderive_o_du_high_parameters_after_parsing(app, config.odu_high_cfg, true);
}
