// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/span.h"
#include "ocudu/adt/static_vector.h"
#include "ocudu/ran/csi_rs/csi_meas_config.h"
#include "ocudu/ran/dmrs/dmrs.h"
#include "ocudu/ran/pdsch/pdsch_prb_bundling.h"
#include "ocudu/ran/pdsch/pdsch_time_domain_resource.h"
#include "ocudu/ran/pucch/pucch_configuration.h"
#include "ocudu/ran/sch/tbs_calculator.h"
#include "ocudu/ran/sr_configuration.h"
#include "ocudu/scheduler/result/resource_block_group.h"
#include "ocudu/scheduler/rrm/cell_pucch_resources.h"
#include <optional>
#include <vector>

namespace ocudu {

struct ran_cell_config;

/// PUCCH resources that the RRM allocates to the UEs of a cell.
struct cell_pucch_rrm_config {
  /// UE-dedicated PUCCH resources of the cell, and the map from the IDs that the RRM allocates to a UE to them.
  cell_pucch_resources res;
  /// Periodicity of the SR.
  sr_periodicity sr_period = sr_periodicity::sl_40;
  /// Maximum code rate of the Format 2, 3 and 4 resources.
  max_pucch_code_rate max_code_rate_234 = max_pucch_code_rate::not_set;
  /// Slot offsets between a PDSCH and its HARQ-ACK.
  static_vector<uint8_t, 8> dl_data_to_ul_ack;
};

/// PDSCH parameters of the UEs of a cell.
struct cell_pdsch_rrm_config {
  /// \c xOverhead of the \c PDSCH-ServingCellConfig of the UEs.
  x_overhead x_ov_head = x_overhead::not_set;
  /// Additional DMRS positions of the PDSCH of mapping type A.
  dmrs_additional_positions dmrs_add_pos = dmrs_additional_positions::pos2;
  /// Maximum number of front-loaded DMRS symbols of the PDSCH of mapping type A.
  dmrs_max_length dmrs_max_len = dmrs_max_length::len1;
  /// RBG size of the PDSCH resource allocation type 0. See TS 38.331, \c rbg-Size.
  rbg_size rbg_sz = rbg_size::config1;
  /// Size of the static PRB bundle of the PDSCH. If absent, the UE applies a size of 2 PRBs. See TS 38.331, \c
  /// prb-BundlingType.
  std::optional<pdsch_prb_bundle_size> prb_bundle_size;
  /// DCI format 1_1 time domain allocation list of the UEs with PDSCH repetitions: the common list, plus one entry
  /// for the repetitions. Empty if the cell has no PDSCH repetitions. See \ref make_ue_pdsch_td_alloc_list.
  std::vector<pdsch_time_domain_resource_allocation> rep_td_alloc_list;
};

/// CSI-RS parameters of the UEs of a cell.
struct cell_csi_rrm_config {
  /// ZP-CSI-RS resources, which the UE rate matches the PDSCH around.
  std::vector<zp_csi_rs_resource> zp_csi_rs_res_list;
  /// Periodic set of the ZP-CSI-RS resources. The ZP-CSI-RS resource set ID of the set is 0.
  std::optional<zp_csi_rs_resource_set> p_zp_csi_rs_res;
};

/// PUSCH parameters of the UEs of a cell.
struct cell_pusch_rrm_config {
  /// \c xOverhead of the \c PUSCH-ServingCellConfig of the UEs.
  x_overhead x_ov_head = x_overhead::not_set;
};

/// Cell resources that the RRM allocates to the UEs of a cell.
struct cell_rrm_config {
  cell_pucch_rrm_config pucch;
  cell_pdsch_rrm_config pdsch;
  cell_csi_rrm_config   csi;
  cell_pusch_rrm_config pusch;
};

/// \brief Makes the dedicated PDSCH time domain allocation list of a UE.
///
/// The list is empty if the UE has no repetitions, thus it uses the common list. Else, it is the list of the cell, with
/// the repetition count of the UE in the last entry.
///
/// \param[in] cell_rep_list Repetition list of the cell. See \ref cell_pdsch_rrm_config::rep_td_alloc_list.
/// \param[in] nof_repetitions Number of PDSCH repetitions of the UE. A value of 1 means no repetitions.
std::vector<pdsch_time_domain_resource_allocation>
make_ue_pdsch_td_alloc_list(span<const pdsch_time_domain_resource_allocation> cell_rep_list, unsigned nof_repetitions);

/// Generates the cell resources that the RRM allocates to the UEs of a cell.
cell_rrm_config make_cell_rrm_config(const ran_cell_config& cell_cfg);

} // namespace ocudu
