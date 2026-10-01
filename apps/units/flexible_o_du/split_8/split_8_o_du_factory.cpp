// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "split_8_o_du_factory.h"
#include "helpers/ru_sdr_factories.h"

using namespace ocudu;

split8_du_factory::split8_du_factory(const split_8_o_du_unit_config& config_) :
  flexible_o_du_factory({config_.odu_high_cfg,
                         config_.du_low_cfg,
                         {config_.ru_cfg.metrics_cfg.metrics_cfg, config_.ru_cfg.metrics_cfg.enable_ru_metrics}}),
  unit_config(config_)
{
}

std::unique_ptr<radio_unit> split8_du_factory::create_radio_unit(const flexible_o_du_ru_config&       ru_config,
                                                                 const flexible_o_du_ru_dependencies& ru_dependencies)
{
  return create_sdr_radio_unit(unit_config.ru_cfg, ru_config, ru_dependencies);
}
