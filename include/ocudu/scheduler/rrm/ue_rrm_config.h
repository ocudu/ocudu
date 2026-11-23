// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/harq_id.h"
#include "ocudu/ran/pdcch/dci_packing.h"
#include "ocudu/ran/pdsch/pdsch_mcs.h"
#include "ocudu/ran/resource_allocation/vrb_to_prb.h"
#include "ocudu/scheduler/rrm/cell_pucch_resources.h"
#include <optional>

namespace ocudu {

/// Periodic CSI report of a UE, which the UE sends on PUCCH.
struct ue_periodic_csi_rrm_config {
  /// ID of the cell CSI resource.
  pucch_csi_resource_id res_id;
  /// Slot offset of the report.
  unsigned offset;

  bool operator==(const ue_periodic_csi_rrm_config& other) const
  {
    return res_id == other.res_id and offset == other.offset;
  }
};

/// PUCCH resources that the RRM allocates to a UE. They are IDs of the cell resources and slot offsets.
struct ue_pucch_rrm_config {
  /// ID of the cell configuration of the HARQ-ACK resource sets that the UE uses.
  pucch_resource_set_config_id res_set_cfg_id;
  /// ID of the cell SR resource.
  pucch_sr_resource_id sr_res_id;
  /// Slot offset of the SR.
  unsigned sr_offset;
  /// Whether PUCCH repetition is enabled for the Format 1/3/4 resources.
  bool rep_f1_3_4_enabled = false;
  /// Whether PUCCH repetition is enabled for the Format 0/2 resources.
  bool rep_f0_2_enabled = false;

  bool operator==(const ue_pucch_rrm_config& other) const
  {
    return res_set_cfg_id == other.res_set_cfg_id and sr_res_id == other.sr_res_id and sr_offset == other.sr_offset and
           rep_f1_3_4_enabled == other.rep_f1_3_4_enabled and rep_f0_2_enabled == other.rep_f0_2_enabled;
  }
};

/// CSI resources that the RRM allocates to a UE.
struct ue_csi_rrm_config {
  /// Periodic CSI report. Present only if the cell configures periodic CSI reports.
  std::optional<ue_periodic_csi_rrm_config> periodic_report;

  bool operator==(const ue_csi_rrm_config& other) const { return periodic_report == other.periodic_report; }
};

/// PDSCH parameters that the RRM allocates to a UE.
struct ue_pdsch_rrm_config {
  /// Number of DL HARQ processes. See TS 38.331, \c nrofHARQ-ProcessesForPDSCH.
  uint8_t nof_harqs = MAX_NOF_HARQS_NON_NTN;
  /// A bit set to 1 indicates a DL HARQ process with disabled HARQ feedback. See TS 38.331, \c
  /// downlinkHARQ-FeedbackDisabled.
  harq_dl_feedback_disabled_mask harq_feedback_disabled = harq_dl_feedback_disabled_mask(MAX_NOF_HARQS);
  /// Maximum number of PDSCH layers of the UE in the cell. See TS 38.331, \c maxMIMO-Layers. Absent if only the cell
  /// limits the number of layers.
  std::optional<uint8_t> max_nof_layers;
  /// MCS table of the PDSCH. See TS 38.331, \c mcs-Table.
  pdsch_mcs_table mcs_table = pdsch_mcs_table::qam64;
  /// VRB-to-PRB mapping of the PDSCH. See TS 38.331, \c vrb-ToPRB-Interleaver.
  vrb_to_prb::mapping_type interleaving = vrb_to_prb::mapping_type::non_interleaved;
  /// Resource allocation type of the PDSCH for DCI format 1_1. See TS 38.331, \c resourceAllocation.
  resource_allocation res_alloc = resource_allocation::resource_allocation_type_1;
  /// Number of bits of the HARQ process number field of the DCI format 1_1.
  harq_process_num_field_size dci_harq_field_size = harq_process_num_field_size::n4;
  /// Maximum number of PDSCH repetitions of the UE. A value of 1 means no repetitions. See TS 38.331, \c
  /// repetitionNumber-r16.
  uint8_t max_nof_repetitions = 1;

  bool operator==(const ue_pdsch_rrm_config& other) const
  {
    return nof_harqs == other.nof_harqs and harq_feedback_disabled == other.harq_feedback_disabled and
           max_nof_layers == other.max_nof_layers and mcs_table == other.mcs_table and
           interleaving == other.interleaving and res_alloc == other.res_alloc and
           dci_harq_field_size == other.dci_harq_field_size and max_nof_repetitions == other.max_nof_repetitions;
  }
};

/// PUSCH parameters that the RRM allocates to a UE.
struct ue_pusch_rrm_config {
  /// Number of UL HARQ processes. See TS 38.331, \c nrofHARQ-ProcessesForPUSCH.
  uint8_t nof_harqs = MAX_NOF_HARQS_NON_NTN;
  /// A bit set to 1 indicates a UL HARQ process in mode A, and a bit set to 0 a process in mode B. See TS 38.331, \c
  /// uplinkHARQ-mode.
  harq_ul_mode_mask harq_mode = ~harq_ul_mode_mask(MAX_NOF_HARQS);

  bool operator==(const ue_pusch_rrm_config& other) const
  {
    return nof_harqs == other.nof_harqs and harq_mode == other.harq_mode;
  }
};

/// Resources that the RRM allocates to a UE in a cell.
struct ue_rrm_config {
  ue_pucch_rrm_config pucch;
  ue_csi_rrm_config   csi;
  ue_pdsch_rrm_config pdsch;
  ue_pusch_rrm_config pusch;

  bool operator==(const ue_rrm_config& other) const
  {
    return pucch == other.pucch and csi == other.csi and pdsch == other.pdsch and pusch == other.pusch;
  }
};

} // namespace ocudu
