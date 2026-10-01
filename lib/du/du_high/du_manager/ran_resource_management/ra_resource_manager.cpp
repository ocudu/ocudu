// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ra_resource_manager.h"
#include "ocudu/adt/format.h"

using namespace ocudu;
using namespace odu;

namespace {

/// We use bit reversal as a mechanism to move preambles close to each other, in terms of index, far apart in the free
/// list.
struct bit_reverse_compare {
  bool operator()(unsigned lhs, unsigned rhs)
  {
    auto lhs_reversed = bit_reverse(lhs);
    auto rhs_reversed = bit_reverse(rhs);
    return lhs_reversed < rhs_reversed;
  }
};

} // namespace

ra_resource_manager::ra_resource_manager(span<const du_cell_config> cell_cfg_list)
{
  cells.resize(cell_cfg_list.size());
  for (unsigned i = 0, e = cells.size(); i != e; ++i) {
    cells[i].cell_cfg = &cell_cfg_list[i];
    const auto& cfg   = *cells[i].cell_cfg;

    if (cfg.ran.ul_cfg_common.init_ul_bwp.rach_cfg_common.has_value() and cfg.ran.init_bwp.rach.cfra_enabled) {
      const auto& rach_common = cfg.ran.ul_cfg_common.init_ul_bwp.rach_cfg_common.value();

      // Store the preambles used for CFRA.
      const unsigned nof_cf_preambles = rach_common.total_nof_ra_preambles - rach_common.nof_cb_preambles_per_ssb;
      cells[i].free_preamble_idx_list.reserve(nof_cf_preambles);
      for (unsigned j = 0; j != nof_cf_preambles; ++j) {
        cells[i].free_preamble_idx_list.push_back(rach_common.nof_cb_preambles_per_ssb + j);
      }

      // Sort preambles so close ones are far apart in the list.
      std::sort(cells[i].free_preamble_idx_list.begin(), cells[i].free_preamble_idx_list.end(), bit_reverse_compare{});
    }
  }
}

void ra_resource_manager::allocate_cfra_resources(du_ue_resource_config& ue_res_cfg)
{
  if (ue_res_cfg.cfra.has_value()) {
    // UE has CFRA already configured.
    return;
  }

  du_cell_index_t  pcell_index = ue_res_cfg.cell_group.cells.at(SERVING_PCELL_IDX).serv_cell_cfg.cell_index;
  cell_ra_context& pcell_ra    = cells[pcell_index];

  if (pcell_ra.free_preamble_idx_list.empty()) {
    // CFRA is either not enabled for this cell or there are no free RA preambles.
    return;
  }

  // Allocate a CFRA preamble.
  const unsigned selected_preamble = pcell_ra.free_preamble_idx_list.back();
  pcell_ra.free_preamble_idx_list.pop_back();

  // Store allocated CFRA preambled in UE resources.
  ue_res_cfg.cfra.emplace();
  ue_res_cfg.cfra.value().preamble_id = selected_preamble;
}

void ra_resource_manager::deallocate_cfra_resources(du_ue_resource_config& ue_res_cfg)
{
  if (ue_res_cfg.cfra.has_value()) {
    // Return allocated CFRA preamble to the pool.
    cell_ra_context& cell = cells[ue_res_cfg.cell_group.cells.at(SERVING_PCELL_IDX).serv_cell_cfg.cell_index];

    // Do sorted insertion (close preamble IDs are far apart).
    auto id  = ue_res_cfg.cfra.value().preamble_id;
    auto pos = std::upper_bound(
        cell.free_preamble_idx_list.begin(), cell.free_preamble_idx_list.end(), id, bit_reverse_compare{});
    cell.free_preamble_idx_list.insert(pos, id);

    // Reset CFRA resources.
    ue_res_cfg.cfra.reset();
  }
}
