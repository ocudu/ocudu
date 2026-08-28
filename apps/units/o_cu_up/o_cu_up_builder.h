// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "apps/services/metrics/metrics_config.h"
#include "o_cu_up_unit_dependencies.h"
#include "o_cu_up_unit_impl.h"

namespace ocudu {

/// O-RAN CU-CP unit.
struct o_cu_up_unit {
  std::unique_ptr<ocuup::o_cu_up>           unit;
  std::vector<app_services::metrics_config> metrics;
};

/// Builds the O-RAN CU-UP unit using the given arguments.
o_cu_up_unit build_o_cu_up(const o_cu_up_unit_config& unit_cfg, const o_cu_up_unit_dependencies& dependencies);

} // namespace ocudu
