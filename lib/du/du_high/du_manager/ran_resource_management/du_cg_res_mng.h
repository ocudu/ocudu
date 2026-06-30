// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "du_cg_resource_manager.h"
#include "ocudu/adt/circular_vector.h"
#include "ocudu/adt/slotted_array.h"
#include "ocudu/du/du_cell_config.h"

namespace ocudu {
namespace odu {

struct cell_group_config;

/// \brief DU resource manager for Type-1 Configured Grant.
///
/// This class implements the CG resource allocation assuming Type-1 CG.
class du_cg_type1_res_mng : public du_cg_resource_manager
{
public:
  void add_cell(du_cell_index_t cell_idx, const du_cell_config& cell_cfg) override;

  void rem_cell(du_cell_index_t cell_idx) override;

  bool alloc_resources(cell_group_config& cell_grp_cfg) override;

  void dealloc_resources(cell_group_config& cell_grp_cfg) override;

private:
  struct cell_context {
    cell_context(const du_cell_config& cell_cfg_);

    std::optional<unsigned> find_optimal_cg_offset();

    const du_cell_config&                        cell_cfg;
    const std::optional<tdd_ul_dl_config_common> tdd_ul_dl_cfg_common;
    // Ring vector that keeps track of the RB usage (for CG, PRACH and PUCCH) at a given slot within the "CG period".
    // NOTE: more precisely, we use the LCM of CG period and PRACH period as length of the ring.
    circular_vector<crb_bitmap> cg_alloc_grid;
    // Ring vector that keeps track of how many RBs have been used for CG at a given slot within the "CG period".
    // NOTE: more precisely, we use the LCM of CG period and PRACH period as length of the ring.
    circular_vector<unsigned> nof_rbs_allocated;
  };

  // Contains the resources for the different cells of the DU.
  slotted_id_table<du_cell_index_t, cell_context, MAX_NOF_DU_CELLS> cells;
};

} // namespace odu
} // namespace ocudu
