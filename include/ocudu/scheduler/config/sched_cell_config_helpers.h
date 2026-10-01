// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

namespace ocudu {

struct pucch_resource_builder_params;
struct bwp_downlink_dedicated;
struct serving_cell_config;
struct dl_config_common;
struct pdcch_config;

namespace config_helpers {

unsigned compute_tot_nof_monitored_pdcch_candidates_per_slot(const serving_cell_config& ue_cell_cfg,
                                                             const dl_config_common&    dl_cfg_common);
unsigned compute_tot_nof_monitored_pdcch_candidates_per_slot(const pdcch_config&     ue_pdcch_cfg,
                                                             const dl_config_common& dl_cfg_common);
} // namespace config_helpers
} // namespace ocudu
