// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once
#include "ocudu/adt/span.h"
#include "ocudu/ntn/ntn_configuration_manager_config.h"
#include "ocudu/ran/gnb_id.h"
#include "ocudu/ran/qos/five_qi.h"
#include "ocudu/ran/rb_id.h"
#include <map>
#include <vector>

namespace ocudu {

namespace odu {
struct du_qos_config;
struct du_srb_config;
} // namespace odu

struct du_high_unit_cell_config;
struct du_high_unit_cell_ntn_config;
struct ntn_satellite_config;

/// Applies NTN-specific overrides to DU SRB configuration based on \c ntn_cfg.
void ntn_augment_du_srb_config(const du_high_unit_cell_ntn_config&     ntn_cfg,
                               std::map<srb_id_t, odu::du_srb_config>& srb_cfgs);

/// Applies NTN-specific overrides to DU QoS configuration based on \c ntn_cfg.
void ntn_augment_du_qos_config(const du_high_unit_cell_ntn_config&      ntn_cfg,
                               std::map<five_qi_t, odu::du_qos_config>& qos_cfgs);

/// \brief Converts the app-level NTN cell configurations into the NTN configuration manager config.
///
/// The returned config carries no cell when none of the cells configures NTN.
ocudu_ntn::ntn_configuration_manager_config
generate_ntn_configuration_manager_config(const gnb_id_t&                          gnb_id,
                                          span<const du_high_unit_cell_config>     du_hi_cells,
                                          const std::vector<ntn_satellite_config>& ntn_satellites);

} // namespace ocudu
