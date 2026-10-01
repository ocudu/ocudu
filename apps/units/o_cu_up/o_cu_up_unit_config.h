// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "apps/units/o_cu_up/cu_up/cu_up_unit_config.h"
#include "apps/units/o_cu_up/e2/o_cu_up_e2_config.h"

namespace ocudu {

/// O-RAN CU-UP application unit configuration.
struct o_cu_up_unit_config {
  cu_up_unit_config cu_up_cfg;
  o_cu_up_e2_config e2_cfg;
};

} // namespace ocudu
