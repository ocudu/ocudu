// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "apps/units/flexible_o_du/split_helpers/flexible_o_du_factory.h"
#include "split_8_o_du_unit_config.h"

namespace ocudu {

/// Split 8 O-RAN DU factory.
class split8_du_factory : public flexible_o_du_factory
{
  const split_8_o_du_unit_config& unit_config;

public:
  explicit split8_du_factory(const split_8_o_du_unit_config& config_);

private:
  std::unique_ptr<radio_unit> create_radio_unit(const flexible_o_du_ru_config&       ru_config,
                                                const flexible_o_du_ru_dependencies& ru_dependencies) override;
};

} // namespace ocudu
