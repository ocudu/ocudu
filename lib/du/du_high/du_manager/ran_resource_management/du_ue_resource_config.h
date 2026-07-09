// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/flat_map.h"
#include "ocudu/adt/slotted_array.h"
#include "ocudu/adt/slotted_vector.h"
#include "ocudu/f1u/du/f1u_config.h"
#include "ocudu/mac/mac_cell_group_config.h"
#include "ocudu/mac/mac_lc_config.h"
#include "ocudu/ran/logical_channel/lcid.h"
#include "ocudu/ran/meas_gap_config.h"
#include "ocudu/ran/physical_cell_group.h"
#include "ocudu/ran/qos/qos_parameters.h"
#include "ocudu/ran/rb_id.h"
#include "ocudu/ran/s_nssai.h"
#include "ocudu/rlc/rlc_config.h"
#include "ocudu/scheduler/scheduler_configurator.h"

namespace ocudu {
namespace odu {

/// This struct stores the accumulated CellGroupConfig.
struct cell_group_config {
  mac_cell_group_config                       mcg_cfg;
  physical_cell_group_config                  pcg_cfg;
  flat_map<serv_cell_index_t, ue_cell_config> cells;
};

/// Parameters of an SRB of the DU UE context.
struct du_ue_srb_config {
  srb_id_t      srb_id;
  rlc_config    rlc_cfg;
  mac_lc_config mac_cfg;
};

/// Parameters of a DRB of the DU UE context.
struct du_ue_drb_config {
  drb_id_t                      drb_id;
  lcid_t                        lcid;
  pdcp_sn_size                  pdcp_sn_len = pdcp_sn_size::invalid;
  s_nssai_t                     s_nssai;
  qos_flow_level_qos_parameters qos;
  f1u_config                    f1u;
  rlc_config                    rlc_cfg;
  mac_lc_config                 mac_cfg;

  bool operator==(const du_ue_drb_config& other) const
  {
    return drb_id == other.drb_id and pdcp_sn_len == other.pdcp_sn_len and s_nssai == other.s_nssai and
           qos == other.qos and f1u == other.f1u;
  }
};

/// Contention-free random access (CFRA) configuration for a UE.
struct cfra_config {
  unsigned preamble_id;
};

/// Snapshot of the DU resources taken by a UE at a given instant.
struct du_ue_resource_config {
  slotted_id_table<srb_id_t, du_ue_srb_config, MAX_NOF_SRBS> srbs;
  /// Upper layer configuration of UE DRBs
  slotted_id_vector<drb_id_t, du_ue_drb_config> drbs;
  /// CellGroupConfiguration of the RAN resources allocated to a UE.
  cell_group_config cell_group;
  /// measGapConfig chosen for the UE.
  std::optional<meas_gap_config> meas_gap;
  /// Resources allocated for contention-free random access (CFRA).
  std::optional<cfra_config> cfra;
};

} // namespace odu
} // namespace ocudu
