// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/circular_vector.h"
#include "ocudu/adt/slotted_array.h"
#include "ocudu/scheduler/config/ran_cell_config.h"
#include "ocudu/scheduler/rrm/cg_resource_manager.h"

namespace ocudu {

/// \brief Resource manager for Type-1 Configured Grant.
///
/// This class implements the CG resource allocation assuming Type-1 CG.
class cg_type1_res_mng : public cg_resource_manager
{
public:
  void add_cell(du_cell_index_t cell_idx, const ran_cell_config& cell_cfg) override;

  void rem_cell(du_cell_index_t cell_idx) override;

  bool alloc_resources(ue_cell_config& ue_cell_cfg) override;

  void dealloc_resources(ue_cell_config& ue_cell_cfg) override;

private:
  struct cell_context {
    explicit cell_context(const ran_cell_config& cell_cfg_);

    std::optional<unsigned> find_optimal_cg_offset();

    const ran_cell_config                        cell_cfg;
    const std::optional<tdd_ul_dl_config_common> tdd_ul_dl_cfg_common;
    // Contains the default (per-cell) parameters for the Configured Grant configuration.
    const cg_configuration default_cg_config;
    const unsigned         nof_rbs_per_ue;

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

} // namespace ocudu
