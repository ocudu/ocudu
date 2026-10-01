// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "apps/units/flexible_o_du/split_helpers/flexible_o_du_factory.h"
#include "split_7_2_o_du_unit_config.h"

namespace ocudu {

/// Split 7.2 O-RAN DU factory.
class split_7_2_o_du_factory : public flexible_o_du_factory
{
  const split_7_2_o_du_unit_config& unit_config;

public:
  explicit split_7_2_o_du_factory(const split_7_2_o_du_unit_config& config_) :
    flexible_o_du_factory(
        {config_.odu_high_cfg,
         config_.du_low_cfg,
         {config_.ru_cfg.config.metrics_cfg.metrics_cfg, config_.ru_cfg.config.metrics_cfg.enable_ru_metrics}}),
    unit_config(config_)
  {
  }

private:
  std::unique_ptr<radio_unit> create_radio_unit(const flexible_o_du_ru_config&       ru_config,
                                                const flexible_o_du_ru_dependencies& ru_dependencies) override;
};

} // namespace ocudu
