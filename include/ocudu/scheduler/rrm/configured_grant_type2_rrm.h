// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/circular_vector.h"
#include "ocudu/adt/slotted_array.h"
#include "ocudu/scheduler/config/ran_cell_config.h"
#include "ocudu/scheduler/rrm/configured_grant_rrm.h"

namespace ocudu {

/// \brief Resource manager for Type-2 Configured Grant.
///
/// This class implements the CG resource allocation assuming Type-2 CG.
class configured_grant_type2_rrm : public configured_grant_rrm
{
public:
  void add_cell(du_cell_index_t cell_idx, const ran_cell_config& cell_cfg) override;

  void rem_cell(du_cell_index_t cell_idx) override;

  bool build_ue_cg_config(ue_cell_config& ue_cell_cfg) override;

  void reset_ue_cg_config(ue_cell_config& ue_cell_cfg) override;

private:
  struct cell_context {
    explicit cell_context(const ran_cell_config& cell_cfg_);

    // Contains the default (per-cell) parameters for the Configured Grant configuration.
    const cg_configuration default_cg_config;
    // Maximum number of UEs that the cell can serve with a Type-2 CG. It is the size of the resource pool held by the
    // scheduler's cg_type2_resource_manager, i.e. one resource per (usable slot offset, CRB block) pair.
    const unsigned max_nof_ues;
    // Number of UEs currently holding a Type-2 CG configuration in this cell.
    unsigned nof_ues = 0;
  };

  // Contains the resources for the different cells of the DU.
  slotted_id_table<du_cell_index_t, cell_context, MAX_NOF_DU_CELLS> cells;
};

} // namespace ocudu
