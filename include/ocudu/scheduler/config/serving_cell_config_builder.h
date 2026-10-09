// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/pucch/pucch_configuration.h"
#include "ocudu/scheduler/config/cell_bwp_res_config.h"
#include "ocudu/scheduler/config/ran_cell_config.h"
#include "ocudu/scheduler/config/ue_bwp_config.h"

namespace ocudu::config_helpers {

/// \brief Build the PUCCH-Config for a given UE.
///
/// [Implementation-defined] The UE PUCCH resource list contains the resources in the following order:
///   | HARQ Resource Set ID 0 | SR | HARQ Resource Set ID 1 | CSI | (SR_F2) | (CSI_F0) |
/// Where SR_F2 and CSI_F0 only exist when using Format 0 and Format 2 together. The UE resource ID of each resource,
/// i.e. its \c PUCCH-ResourceId, is its position in the list.
pucch_config build_pucch_config(const ran_cell_config&        cell_cfg,
                                const cell_ul_bwp_res_config& cell_ul_bwp_res,
                                const ue_bwp_config&          ue_bwp_cfg);

/// Gets the UE resource ID of the CSI resource in the PUCCH-Config built by \c build_pucch_config.
unsigned ue_csi_res_id(const pucch_resource_builder_params& res_params);

/// Build the CSI-MeasConfig for a given UE.
std::optional<csi_meas_config> build_csi_meas_config(const ran_cell_config&        cell_cfg,
                                                     const cell_ul_bwp_res_config& cell_ul_bwp_res,
                                                     const ue_bwp_config&          ue_bwp_cfg);

} // namespace ocudu::config_helpers
