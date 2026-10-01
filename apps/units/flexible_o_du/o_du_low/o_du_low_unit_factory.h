// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "apps/services/metrics/metrics_config.h"
#include "o_du_low_unit_factory_config.h"
#include "ocudu/du/du_low/o_du_low.h"

namespace ocudu {

/// O-RAN DU low unit.
struct o_du_low_unit {
  std::unique_ptr<odu::o_du_low>            o_du_lo;
  std::vector<app_services::metrics_config> metrics;
};

/// O-RAN DU low unit factory.
class o_du_low_unit_factory
{
  o_du_low_hal_dependencies hal_dependencies;

public:
  o_du_low_unit_factory(const std::optional<du_low_unit_hal_config>& hal_config);

  /// Creates an O-RAN DU low unit with the given parameters.
  o_du_low_unit create(const o_du_low_unit_config& params, const o_du_low_unit_dependencies& dependencies);
};

} // namespace ocudu
