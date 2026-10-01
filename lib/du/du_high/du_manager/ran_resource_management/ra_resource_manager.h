// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "du_ue_resource_config.h"
#include "ocudu/du/du_cell_config.h"

namespace ocudu::odu {

/// Handler of allocation of RA preambles for Contention-free Random Access (CFRA).
class ra_resource_manager
{
public:
  ra_resource_manager(span<const du_cell_config> cell_cfg_list);

  /// Allocate RA resources to a given UE.
  void allocate_cfra_resources(du_ue_resource_config& ue_res_cfg);

  /// Deallocate RA resources of a given UE.
  void deallocate_cfra_resources(du_ue_resource_config& ue_res_cfg);

private:
  struct cell_ra_context {
    const du_cell_config* cell_cfg;
    std::vector<unsigned> free_preamble_idx_list;
  };

  std::vector<cell_ra_context> cells;
};

} // namespace ocudu::odu
