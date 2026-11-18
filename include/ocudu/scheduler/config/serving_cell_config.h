// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/configured_grant/cg_configuration.h"
#include "ocudu/ran/csi_rs/csi_meas_config.h"
#include "ocudu/ran/pdcch/downlink_preemption.h"
#include "ocudu/ran/pucch/pucch_configuration.h"
#include "ocudu/ran/pucch/pucch_tpc.h"
#include "ocudu/ran/pucch/srs_tpc.h"
#include "ocudu/ran/pusch/pusch_configuration.h"
#include "ocudu/ran/pusch/pusch_tpc.h"
#include "ocudu/ran/radio_link_monitoring.h"
#include "ocudu/ran/serv_cell_index.h"
#include "ocudu/ran/srs/srs_configuration.h"
#include "ocudu/scheduler/config/bwp_configuration.h"

namespace ocudu {

/// Converts a uint into a \ref serv_cell_index.
inline serv_cell_index_t uint_to_serv_cell_index(unsigned idx)
{
  return idx < MAX_NOF_SERVING_CELLS ? static_cast<serv_cell_index_t>(idx)
                                     : static_cast<serv_cell_index_t>(MAX_NOF_SERVING_CELLS);
}

/// "PDCCH-Config" - UE-dedicated PDCCH configuration as per TS 38.331.
struct pdcch_config {
  /// List of CORESETs to be used by the UE. In case of CORESET Id overlaps with commonControlResourceSet,
  /// the CORESET in this list takes precedence. The network configures at most 3 CORESETs per BWP per cell (including
  /// UE-specific and common CORESETs).
  std::vector<coreset_configuration> coresets;
  /// List of SearchSpaces to be used by the UE. The network configures at most 10 Search Spaces per BWP per cell
  /// (including UE-specific and common Search Spaces).
  std::vector<search_space_configuration> search_spaces;
  /// Configuration of downlink preemption indications to be monitored in this cell.
  std::optional<downlink_preemption> dl_preemption;
  /// TPC Commands Configuration to configure UE for extracting TPC commands for PUSCH from a group-TPC messages on DCI.
  std::optional<pusch_tpc_command_config> pusch_tpc_cmd_cfg;
  /// TPC Commands Configuration to configure UE for extracting TPC commands for PUCCH from a group-TPC messages on DCI.
  std::optional<pucch_tpc_command_config> pucch_tpc_cmd_cfg;
  /// TPC Commands Configuration to configure UE for extracting TPC commands for SRS from a group-TPC messages on DCI.
  std::optional<srs_tpc_command_config> srs_tpc_cmd_cfg;
  // TODO: add remaining fields.

  bool operator==(const pdcch_config& rhs) const
  {
    return coresets == rhs.coresets && search_spaces == rhs.search_spaces && dl_preemption == rhs.dl_preemption &&
           pusch_tpc_cmd_cfg == rhs.pusch_tpc_cmd_cfg && pucch_tpc_cmd_cfg == rhs.pucch_tpc_cmd_cfg &&
           srs_tpc_cmd_cfg == rhs.srs_tpc_cmd_cfg;
  }
  bool operator!=(const pdcch_config& rhs) const { return !(rhs == *this); }
};

/// "BWP-DownlinkDedicated" as per TS 38.331.
struct bwp_downlink_dedicated {
  std::optional<pdcch_config>                 pdcch_cfg;
  std::optional<radio_link_monitoring_config> rlm_cfg;
  // TODO: Remaining

  bool operator==(const bwp_downlink_dedicated& other) const
  {
    return pdcch_cfg == other.pdcch_cfg and rlm_cfg == other.rlm_cfg;
  }

  bool operator!=(const bwp_downlink_dedicated& other) const { return !(other == *this); }
};

/// Used to configure the dedicated UE-specific parameters of an UL BWP.
/// \remark See TS 38.331, BWP-UplinkDedicated.
struct bwp_uplink_dedicated {
  std::optional<pusch_config>     pusch_cfg;
  std::optional<cg_configuration> cg_cfg;
  std::optional<srs_config>       srs_cfg;

  bool operator==(const bwp_uplink_dedicated& other) const
  {
    return pusch_cfg == other.pusch_cfg and srs_cfg == other.srs_cfg and cg_cfg == other.cg_cfg;
  }
};

/// Uplink configuration, as per \c UplinkConfig, in \c ServingCellConfig, TS 38.331.
struct uplink_config {
  bwp_uplink_dedicated init_ul_bwp;
  // TODO: add remaining fields.

  bool operator==(const uplink_config& other) const { return init_ul_bwp == other.init_ul_bwp; }
};

/// \c ServingCellConfig, as per TS 38.331.
struct serving_cell_config {
  /// DU-specific cell identifier.
  du_cell_index_t cell_index;
  /// Initial Downlink BWP.
  bwp_downlink_dedicated init_dl_bwp;
  /// \c uplinkConfig, containing the UL configuration.
  std::optional<uplink_config> ul_config;
  /// \c CSI-MeasConfig.
  std::optional<csi_meas_config> csi_meas_cfg;

  bool operator==(const serving_cell_config& other) const
  {
    return cell_index == other.cell_index and init_dl_bwp == other.init_dl_bwp and ul_config == other.ul_config and
           csi_meas_cfg == other.csi_meas_cfg;
  }
};

} // namespace ocudu
