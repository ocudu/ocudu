// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../cell/resource_grid.h"
#include "../config/cell_configuration.h"

namespace ocudu {

class csi_rs_scheduler
{
public:
  explicit csi_rs_scheduler(const cell_configuration& cell_cfg);

  void run_slot(cell_slot_resource_allocator& res_grid);

private:
  const cell_configuration& cell_cfg;

  std::vector<csi_rs_info> cached_csi_rs;
};

} // namespace ocudu
