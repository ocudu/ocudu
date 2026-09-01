// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "du_high_config.h"
#include "ocudu/du/du_cell_config.h"

namespace ocudu {

namespace odu {
struct du_high_configuration;
} // namespace odu

struct du_high_unit_config;
struct worker_manager_config;

/// Converts and returns the given gNB application configuration to a DU cell configuration.
std::vector<odu::du_cell_config> generate_du_cell_config(const du_high_unit_config& config);

/// Generates and fills the given DU high configuration from the given DU high unit configuration.
void generate_du_high_config(odu::du_high_configuration& du_hi_cfg, const du_high_unit_config& du_high_unit_cfg);

/// Fills the DU high worker manager parameters of the given worker manager configuration.
void fill_du_high_worker_manager_config(worker_manager_config&     config,
                                        const du_high_unit_config& unit_cfg,
                                        bool                       is_blocking_mode_enabled = false);

} // namespace ocudu
