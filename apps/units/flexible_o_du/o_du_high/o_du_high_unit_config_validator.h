// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "du_high/du_high_config_validator.h"
#include "o_du_high_unit_config.h"

namespace ocudu {

/// Validates the given O-DU high configuration. Returns true on success, false otherwise.
inline bool validate_o_du_high_config(const o_du_high_unit_config& config)
{
  return validate_du_high_config(config.du_high_cfg.config);
}

} // namespace ocudu
