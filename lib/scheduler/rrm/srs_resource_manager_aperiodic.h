// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/slotted_array.h"
#include "ocudu/ran/srs/srs_bandwidth_configuration.h"
#include "ocudu/scheduler/config/ran_cell_config.h"
#include "ocudu/scheduler/config/serving_cell_config.h"
#include "ocudu/scheduler/rrm/du_srs_resource.h"
#include "ocudu/scheduler/rrm/srs_resource_manager.h"

namespace ocudu {

/// \brief This class implements the SRS resource manager for aperiodic SRS-resources.
/// It computes:
/// - The list of cell SRS resources.
/// - The \c slot_offset (as per \c slotOffset, SRS-Config, TS 38.331) values that are used to build the SRS-Config for
///   a given UE.
/// and it allocates an SRS resource to any new UE that are added to the DU.
class srs_resource_manager_aperiodic : public srs_resource_manager
{
public:
  void add_cell(du_cell_index_t cell_idx, const ran_cell_config& cell_cfg) override;

  /// The SRS resources are allocate according to the following policy:
  /// - Allocate an SRS resource that has not been allocated to any UE.
  /// - If all SRS resources have been allocated to at least a UE, allocates the one that has been assigned to the
  /// smaller number of UEs.
  bool alloc_resources(ue_cell_config& ue_cell_cfg) override;

  void dealloc_resources(ue_cell_config& ue_cell_cfg) override;

  unsigned get_nof_srs_free_res_offsets(du_cell_index_t cell_idx) const override
  {
    // Return max value, as there is no hard limitation in case of aperiodic SRS.
    return cell_context::max_nof_srs_res;
  }

private:
  struct cell_context {
    cell_context(const ran_cell_config& cfg);

    // Returns the DU SRS resource with the given cell resource ID from the cell list of resources.
    std::vector<du_srs_resource>::const_iterator get_du_srs_res_cfg(unsigned cell_res_id);

    // Fills the unique SRS resource in SRS resource list for a UE SRS-Config.
    void fill_srs_res_parameters(srs_config::srs_resource& res_out, const du_srs_resource& res_in) const;

    using srs_set_t = static_vector<srs_config::srs_resource_set, srs_config::srs_res_set_id::MAX_NOF_SRS_RES_SETS>;

    // Fills the SRS resource set list for a UE SRS-Config.
    void fill_srs_res_set(srs_set_t& srs_res_set_list, srs_config::srs_res_id res_id) const;

    // Parameters that are common to all cell SRS resources.
    struct srs_cell_common {
      unsigned c_srs;
      unsigned freq_shift;
      int      p0;
    };

    // Maximum number of SRS resources that can be generated in a cell.
    // [Implementation-defined] We assume each UE has one and only one resource.
    static constexpr unsigned                    max_nof_srs_res = MAX_NOF_DU_UES;
    const ran_cell_config                        cell_cfg;
    const std::optional<tdd_ul_dl_config_common> tdd_ul_dl_cfg_common;
    // Default SRS configuration for the cell.
    const srs_config default_srs_cfg;
    srs_cell_common  srs_common_params;
    // List of all SRS resources available to the cell; these resources can be allocated to different UEs.
    std::vector<du_srs_resource> cell_srs_res_list;
    // Keeps the count (i.e, \ref srs_res_usage[cell_res_id]) of how many UEs have been assigned the resource ID
    // \ref cell_res_id.
    std::vector<unsigned> srs_res_usage;
    // Slot_offset used to build the SRS-Config for aperiodic SRS.
    unsigned slot_offset;
  };

  // Contains the resources for the different cells of the DU.
  slotted_id_table<du_cell_index_t, cell_context, MAX_NOF_DU_CELLS> cells;
};

} // namespace ocudu
