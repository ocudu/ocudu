// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/scheduler/config/pucch_resource_builder_params.h"
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

/// Resources that the RRM allocates to a UE in a cell.
struct ue_rrm_config {
  ue_pucch_rrm_config pucch;
  ue_csi_rrm_config   csi;

  bool operator==(const ue_rrm_config& other) const { return pucch == other.pucch and csi == other.csi; }
};

} // namespace ocudu
