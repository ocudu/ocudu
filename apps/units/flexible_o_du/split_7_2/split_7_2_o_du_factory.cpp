// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "split_7_2_o_du_factory.h"
#include "helpers/ru_ofh_factories.h"

using namespace ocudu;

std::unique_ptr<radio_unit>
split_7_2_o_du_factory::create_radio_unit(const flexible_o_du_ru_config&       ru_config,
                                          const flexible_o_du_ru_dependencies& ru_dependencies)
{
  return create_ofh_radio_unit(
      unit_config.ru_cfg.config, ru_config, ru_dependencies, unit_config.du_low_cfg.hal_config.has_value());
}
