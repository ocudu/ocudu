// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/scheduler/config/ran_cell_config.h"
#include "ocudu/scheduler/rrm/du_srs_resource.h"

namespace ocudu {

/// \brief Generates the list of orthogonal SRS resources available in a cell.
/// The resources of this cells are meant to be used by the UEs; the same resources can be reused by different UEs over
/// different slots. Note that this function does not allocate the resources to the UEs, it only creates the cell
/// resource list.
/// \param[in] cell_cfg Cell configuration parameters.
/// \param[in] use_special_slot_only If true, use the special slot only to derive the symbols for SRS resources. Only
/// applicable for aperiodic SRS.
/// \return List of orthogonal SRS resources.
std::vector<du_srs_resource> generate_cell_srs_list(const ran_cell_config& cell_cfg,
                                                    bool                   use_special_slot_only = false);

} // namespace ocudu
