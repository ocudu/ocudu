// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/flat_map.h"
#include "ocudu/mac/mac_cell_group_config.h"
#include "ocudu/ran/physical_cell_group.h"
#include "ocudu/scheduler/scheduler_configurator.h"

namespace ocudu {
namespace odu {

/// This struct stores the accumulated CellGroupConfig.
struct cell_group_config {
  mac_cell_group_config                       mcg_cfg;
  physical_cell_group_config                  pcg_cfg;
  flat_map<serv_cell_index_t, ue_cell_config> cells;
};

} // namespace odu
} // namespace ocudu
