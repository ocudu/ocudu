// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "split_7_2_o_du_unit_config_yaml_writer.h"
#include "apps/units/flexible_o_du/o_du_high/o_du_high_unit_config_yaml_writer.h"
#include "apps/units/flexible_o_du/o_du_low/du_low_config_yaml_writer.h"
#include "apps/units/flexible_o_du/split_7_2/helpers/ru_ofh_config_yaml_writer.h"
#include "split_7_2_o_du_unit_config.h"

using namespace ocudu;

void ocudu::fill_split_7_2_o_du_unit_config_in_yaml_schema(YAML::Node& node, const split_7_2_o_du_unit_config& config)
{
  fill_o_du_high_config_in_yaml_schema(node, config.odu_high_cfg);
  fill_du_low_config_in_yaml_schema(node, config.du_low_cfg);
  fill_ru_ofh_config_in_yaml_schema(node, config.ru_cfg.config);
}
