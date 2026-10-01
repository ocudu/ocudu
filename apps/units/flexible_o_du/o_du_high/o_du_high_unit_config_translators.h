// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "du_high/du_high_config_translators.h"
#include "e2/o_du_high_e2_config_translators.h"
#include "o_du_high_unit_config.h"

namespace ocudu {

/// Fills the O-RAN DU high worker manager parameters of the given worker manager configuration.
inline void fill_o_du_high_worker_manager_config(worker_manager_config&       config,
                                                 const o_du_high_unit_config& unit_cfg,
                                                 bool                         is_blocking_mode_enabled = false)
{
  fill_du_high_worker_manager_config(config, unit_cfg.du_high_cfg.config, is_blocking_mode_enabled);
  fill_o_du_high_e2_worker_manager_config(config, unit_cfg.e2_cfg);
}

} // namespace ocudu
