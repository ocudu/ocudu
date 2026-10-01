// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "apps/units/flexible_o_du/o_du_high/o_du_high_unit_config.h"

namespace ocudu {
/// O-RAN DU Split 6 unit configuration.
struct split6_o_du_unit_config {
  /// O-DU high configuration.
  o_du_high_unit_config odu_high_cfg;
};

} // namespace ocudu
