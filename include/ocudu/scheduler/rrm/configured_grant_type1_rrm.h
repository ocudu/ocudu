// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/slotted_array.h"
#include "ocudu/scheduler/config/ran_cell_config.h"
#include "ocudu/scheduler/rrm/configured_grant_rrm.h"

namespace ocudu {

/// \brief Resource manager for Type-1 Configured Grant.
///
/// This class implements the CG resource allocation assuming Type-1 CG.
class configured_grant_type1_rrm : public configured_grant_rrm
{
public:
  void add_cell(du_cell_index_t cell_idx, const ran_cell_config& cell_cfg) override;

  void rem_cell(du_cell_index_t cell_idx) override;

  bool build_ue_cg_config(ue_cell_config& ue_cell_cfg) override;

  void reset_ue_cg_config(ue_cell_config& ue_cell_cfg) override;

private:
  struct cell_context {
    explicit cell_context(const ran_cell_config& cell_cfg_);

    std::optional<unsigned> find_optimal_cg_offset() const;

    const ran_cell_config cell_cfg;
    // Contains the default (per-cell) parameters for the Configured Grant configuration.
    const cg_configuration default_cg_config;
    const unsigned         nof_rbs_per_ue;
    // Slot offsets, within the CG period, that CG resources can be placed at: full-UL slots (in TDD) that carry no
    // PRACH occasion on any of their occurrences. The offsets outside this list are never allocated, so the two
    // vectors below hold no meaningful state at their indices.
    const std::vector<unsigned> usable_cg_offsets;

    // Vector that keeps track of the RB usage (for CG and PUCCH) at a given slot offset within the CG period.
    std::vector<crb_bitmap> cg_alloc_grid;
    // Vector that keeps track of how many RBs have been used for CG at a given slot offset within the CG period.
    std::vector<unsigned> nof_rbs_allocated;
  };

  // Contains the resources for the different cells of the DU.
  slotted_id_table<du_cell_index_t, cell_context, MAX_NOF_DU_CELLS> cells;
};

} // namespace ocudu
