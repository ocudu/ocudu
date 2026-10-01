// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "du_high/du_high_config_yaml_writer.h"
#include "e2/o_du_high_e2_config_yaml_writer.h"
#include "fapi/fapi_config_yaml_writer.h"
#include "o_du_high_unit_config.h"
#include <yaml-cpp/yaml.h>

namespace ocudu {

/// Fills the given node with the O-DU high configuration values.
inline void fill_o_du_high_config_in_yaml_schema(YAML::Node& node, const o_du_high_unit_config& config)
{
  fill_du_high_config_in_yaml_schema(node, config.du_high_cfg);
  fill_fapi_config_in_yaml_schema(node, config.fapi_cfg);
  fill_o_du_high_e2_config_in_yaml_schema(node, config.e2_cfg);
}

} // namespace ocudu
