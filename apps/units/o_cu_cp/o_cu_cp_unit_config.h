// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "cu_cp/cu_cp_unit_config.h"
#include "e2/o_cu_cp_e2_config.h"

namespace ocudu {

/// O-RAN CU-CP application unit configuration.
struct o_cu_cp_unit_config {
  cu_cp_unit_config cucp_cfg;
  o_cu_cp_e2_config e2_cfg;
};

} // namespace ocudu
