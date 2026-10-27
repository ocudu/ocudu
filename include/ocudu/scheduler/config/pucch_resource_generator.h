// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/expected.h"
#include "ocudu/ran/pucch/pucch_constants.h"
#include "ocudu/scheduler/config/pucch_resource_builder_params.h"
#include "ocudu/scheduler/rrm/cell_pucch_resources.h"

namespace ocudu {

struct serving_cell_config;

namespace config_helpers {

/// \brief Returns the number of PRBs at each BWP edge reserved before the dedicated PUCCH resources.
/// \param[in] params PUCCH resource builder parameters.
/// \param[in] pucch_res_common PUCCH default row index (pucch-ResourceCommon in TS 38.331). Range {0, ..., 15}.
/// \param[in] bwp_size_rbs size of the BWP in RBs.
/// \return \c params.nof_edge_prbs if set. Otherwise, the number of PRBs at each BWP edge up to the last common PUCCH
///         PRB, or zero if \c pucch_res_common is 15.
unsigned get_dedicated_pucch_rb_start(const pucch_resource_builder_params& params,
                                      unsigned                             pucch_res_common,
                                      unsigned                             bwp_size_rbs);

/// \brief Validates the user-defined parameters for building the cell PUCCH resource list.
/// \param[in] params PUCCH resource builder parameters.
/// \param[in] bwp_size_rbs size of the BWP in RBs.
/// \param[in] dedicated_pucch_rb_start Number of PRBs at each BWP edge reserved before the dedicated PUCCH resources.
/// \return An error message if the parameters are not valid. Otherwise, success.
error_type<const char*> pucch_parameters_validator(const pucch_resource_builder_params& params,
                                                   unsigned                             bwp_size_rbs,
                                                   unsigned                             dedicated_pucch_rb_start);

/// \brief Generates the list of cell common PUCCH resources from TS 38.213 Table 9.2.1-1.
///
/// \param[in] pucch_res_common PUCCH default row index (pucch-ResourceCommon in TS 38.331). Range {0, ..., 15}.
/// \param[in] bwp_size_rbs size of the BWP in RBs.
/// \return The 16 PUCCH resources for the default common table row.
std::array<pucch_resource, pucch_constants::MAX_NOF_CELL_COMMON_PUCCH_RESOURCES>
generate_cell_common_pucch_res_list(unsigned pucch_res_common, unsigned bwp_size_rbs);

/// \brief Generates the cell PUCCH resources from the PUCCH resource builder parameters.
///
/// The generated resources are packed on both ends of the BWP as tightly as possible (using multiplexing if
/// configured), while ensuring they don't collide with each other nor with the PRBs reserved at the BWP edges.
/// The returned tables map the IDs that the RRM allocates to a UE (resource set configuration, SR and CSI resource) to
/// the resources of the list.
///
/// \param[in] params PUCCH resource builder parameters.
/// \param[in] bwp_size_rbs size of the BWP in RBs.
/// \param[in] dedicated_pucch_rb_start Number of PRBs at each BWP edge reserved before the dedicated PUCCH resources.
/// \return The PUCCH resources for a cell.
/// \remark The function returns empty resources in the following cases:
///         (i) If overall the RBs occupancy is larger than the BWP size.
///         (ii) If F2 intra-slot frequency hopping is enabled with only 1 symbol.
cell_pucch_resources generate_cell_pucch_res_list(const pucch_resource_builder_params& params,
                                                  unsigned                             bwp_size_rbs,
                                                  unsigned                             dedicated_pucch_rb_start);

} // namespace config_helpers
} // namespace ocudu
