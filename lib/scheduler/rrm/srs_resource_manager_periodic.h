// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/slotted_array.h"
#include "ocudu/scheduler/config/ran_cell_config.h"
#include "ocudu/scheduler/config/serving_cell_config.h"
#include "ocudu/scheduler/rrm/du_srs_resource.h"
#include "ocudu/scheduler/rrm/srs_resource_manager.h"

namespace ocudu {

/// This class implements the MAX UL throughput policy for the SRS allocation. The SRS resources are allocated with the
/// objective to minimize the number of slots and symbols that contain the SRS resources, to reduce as much as possible
/// the slots and symbols resources taken from the PUSCH. The drawback of this policy is that it can increase the
/// inter-slot SRS interference among different UEs.
class srs_resource_manager_periodic : public srs_resource_manager
{
public:
  void add_cell(du_cell_index_t cell_idx, const ran_cell_config& cell_cfg) override;

  /// The SRS resources are allocate according to the following policy:
  /// - Give priority to resources that are placed on partially-UL slots, first.
  /// - Then, give priority to SRS resources that is placed on the symbol interval (i.e, the symbols interval used by an
  /// SRS resource) closest to the end of the slot.
  /// - If a symbol interval on a particular slot is already used and not completely full, then give priority to this
  /// symbol interval over any other symbol intervals on the same or on different slots.
  bool alloc_resources(ue_cell_config& ue_cell_cfg) override;

  void dealloc_resources(ue_cell_config& ue_cell_cfg) override;

  unsigned get_nof_srs_free_res_offsets(du_cell_index_t cell_idx) const override
  {
    return cells[cell_idx].srs_res_offset_free_list.size();
  }

private:
  struct cell_context {
    cell_context(const ran_cell_config& cfg);

    // Returns the DU SRS resource with the given cell resource ID from the cell list of resources.
    std::vector<du_srs_resource>::const_iterator get_du_srs_res_cfg(unsigned cell_res_id);

    using pair_res_id_offset = std::pair<unsigned, unsigned>;

    // Returns the best SRS resource ID and offset for this UE, according to the policy defined in this class.
    std::vector<pair_res_id_offset>::const_iterator find_optimal_ue_srs_resource();

    // Check if this SRS offset has already some SRS resources allocated at a given symbol interval, but can still host
    // more.
    bool offset_interval_used_not_full(unsigned offset) const
    {
      // The counter of SRS resources counts how many resources have been allocated in the current slot (offset).
      // If the counter is 0, then the offset is free.
      // If the counter is a non-zero multiple (i.e, N) of nof_res_per_symb_interval, then the symbol interval of index
      // N-1 has been filled completely and the next symbol interval of index N is free. In any other case, the symbol
      // interval N-1 is partially filled.
      ocudu_assert(offset < slot_resource_cnt.size(), "Offset out of range");
      return slot_resource_cnt[offset] % nof_res_per_symb_interval != 0U;
    }

    // Parameters that are common to all cell SRS resources.
    struct srs_cell_common {
      unsigned c_srs;
      unsigned freq_shift;
      int      p0;
    };

    const ran_cell_config                        cell_cfg;
    const std::optional<tdd_ul_dl_config_common> tdd_ul_dl_cfg_common;
    // Default SRS configuration for the cell.
    const srs_config default_srs_cfg;
    srs_cell_common  srs_common_params;
    // List of all SRS resources available to the cell; these resources can be allocated over to different UEs over
    // different offsets.
    std::vector<du_srs_resource> cell_srs_res_list;
    // List of SRS resource ID and offset that can be allocated to the cell's UEs.
    std::vector<pair_res_id_offset> srs_res_offset_free_list;
    unsigned                        nof_res_per_symb_interval = 0;
    // Counter of how many SRS resources that are allocated per slot (offset).
    std::vector<unsigned> slot_resource_cnt;
  };

  // Contains the resources for the different cells of the DU.
  slotted_id_table<du_cell_index_t, cell_context, MAX_NOF_DU_CELLS> cells;
};

} // namespace ocudu
