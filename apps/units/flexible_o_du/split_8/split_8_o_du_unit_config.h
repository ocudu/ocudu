// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "apps/units/flexible_o_du/o_du_high/o_du_high_unit_config.h"
#include "apps/units/flexible_o_du/o_du_low/du_low_config.h"
#include "apps/units/flexible_o_du/split_8/helpers/ru_sdr_config.h"

namespace ocudu {

/// Split 8 O-RAN DU unit configuration.
struct split_8_o_du_unit_config {
  /// O-DU high configuration.
  o_du_high_unit_config odu_high_cfg;
  /// DU low configuration.
  du_low_unit_config du_low_cfg;
  /// Radio Unit configuration.
  ru_sdr_unit_config ru_cfg;
};

} // namespace ocudu
