// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/scheduler/rrm/srs_resource_manager.h"
#include <memory>

namespace ocudu {

/// \brief Creates an SRS resource manager policy matching the SRS configuration of the cell.
/// \param[in] first_cell_cfg Configuration of the first cell to be managed; its SRS type (periodic vs aperiodic)
///            determines which policy gets instantiated.
std::unique_ptr<srs_resource_manager> create_srs_resource_manager(const ran_cell_config& first_cell_cfg);

} // namespace ocudu
