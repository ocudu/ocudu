// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "du_high/du_high_config.h"
#include "e2/o_du_high_e2_config.h"
#include "fapi/fapi_config.h"

namespace ocudu {

/// O-DU high unit configuration.
struct o_du_high_unit_config {
  du_high_parsed_config du_high_cfg;
  o_du_high_e2_config   e2_cfg;
  fapi_unit_config      fapi_cfg;
};

} // namespace ocudu
