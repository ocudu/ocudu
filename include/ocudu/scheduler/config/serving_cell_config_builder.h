// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/pucch/pucch_configuration.h"
#include "ocudu/scheduler/config/ran_cell_config.h"
#include "ocudu/scheduler/config/ue_bwp_config.h"
#include "ocudu/scheduler/rrm/cell_rrm_config.h"
#include "ocudu/scheduler/rrm/ue_rrm_config.h"

namespace ocudu::config_helpers {

/// \brief Layout of the PUCCH resources of a UE.
///
/// [Implementation-defined] The UE PUCCH resource list contains the resources in the following order:
///   | HARQ Resource Set ID 0 | SR | HARQ Resource Set ID 1 | CSI | (SR_F2) | (CSI_F0) |
/// Where SR_F2 and CSI_F0 only exist when using Format 0 and Format 2 together. The UE resource ID of each resource,
/// i.e. its \c PUCCH-ResourceId, is its position in the list.
struct ue_pucch_res_layout {
  /// Cell resource of each position of the UE resource list.
  static_vector<pucch_res_id_t, MAX_NOF_UE_PUCCH_RESOURCES> res_list;
  /// UE resource IDs of the resources of the HARQ-ACK Resource Set ID 0.
  static_vector<unsigned, MAX_NOF_PUCCH_RESOURCES_PER_PUCCH_RESOURCE_SET> res_set_0;
  /// UE resource IDs of the resources of the HARQ-ACK Resource Set ID 1.
  static_vector<unsigned, MAX_NOF_PUCCH_RESOURCES_PER_PUCCH_RESOURCE_SET> res_set_1;
  /// UE resource ID of the SR resource.
  unsigned sr_res_id = 0;
  /// UE resource ID of the CSI resource. Present only if the UE has a periodic CSI report.
  std::optional<unsigned> csi_res_id;
};

/// \brief Makes the layout of the PUCCH resources of a UE from the resources that the RRM allocates to it.
///
/// [Implementation-defined] The UE PUCCH resource list contains the resources in the following order:
///   | HARQ Resource Set ID 0 | SR | HARQ Resource Set ID 1 | CSI | (SR_F2) | (CSI_F0) |
/// Where SR_F2 and CSI_F0 only exist when using Format 0 and Format 2 together. The UE resource ID of each resource,
/// i.e. its \c PUCCH-ResourceId, is its position in the list.
ue_pucch_res_layout make_ue_pucch_res_layout(const cell_pucch_resources& cell_resources,
                                             const ue_rrm_config&        ue_rrm_cfg);

/// Gets the UE resource ID of the CSI resource in the layout built by \c make_ue_pucch_res_layout.
unsigned ue_csi_res_id(unsigned res_set_size);

/// Build the CSI-MeasConfig for a given UE.
std::optional<csi_meas_config> build_csi_meas_config(const ran_cell_config&      cell_cfg,
                                                     const cell_pucch_resources& cell_pucch_res,
                                                     const ue_rrm_config&        ue_rrm_cfg);

} // namespace ocudu::config_helpers
