// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/fapi/cell_config.h"
#include "ocudu/fapi_adaptor/precoding_codebook_repository.h"
#include "ocudu/fapi_adaptor/uci_part2_correspondence_repository.h"
#include "ocudu/ocudulog/logger.h"
#include "ocudu/ran/antenna_topology.h"
#include "ocudu/ran/prach/rach_config_common.h"

namespace ocudu {

class downlink_pdu_validator;
class downlink_processor_pool;
class resource_grid_pool;
class uplink_pdu_validator;
class uplink_request_processor;
class uplink_pdu_slot_repository_pool;

namespace odu {
struct du_cell_config;
} // namespace odu

namespace fapi_adaptor {

/// PHY-FAPI P7 sector fastpath adaptor configuration.
struct phy_fapi_p7_sector_fastpath_adaptor_config {
  /// Base station sector identifier.
  unsigned sector_id;
  /// Request headroom size in slots.
  unsigned nof_slots_request_headroom;
  /// Allow uplink requests on empty UL_TTI.requests.
  bool allow_request_on_empty_ul_tti = false;
  /// Subcarrier spacing as per TS38.211 Section 4.2.
  subcarrier_spacing scs;
  /// Common subcarrier spacing, as per TS38.331 Section 6.2.2.
  subcarrier_spacing scs_common;
  /// Carrier cell configuration.
  fapi::carrier_config carrier_cfg;
  /// Topology of the transmit antennas.
  antenna_topology tx_ant_topology = antenna_topology::one_port;
  /// PRACH cell configuration.
  rach_config_common prach_cfg;
  /// PRACH port list.
  std::vector<uint8_t> prach_ports;
  /// Value in dBm at the antenna connector equivalent to 0 dBFS for a receive gain of 0 dB.
  float dbfs_to_dbm_conversion_factor;
  /// Value in dB relative to Full Scale (dBFS) equivalent to 0 dB in normalized units, i.e., as coming from the
  /// physical layer.
  float db_to_dbfs_conversion_factor;
  /// \brief NTN k_mac in slots at the cell SCS; zero for terrestrial cells.
  ///
  /// The gNB DL and UL frames are misaligned by k_mac, so uplink that the UE transmits in its UL slot p is received
  /// at DL-clock slot p + k_mac. The adaptor passes it to the PHY as the PDU slot offset, and reports the
  /// RACH.indication occasion slot in the UE UL frame.
  unsigned ntn_k_mac_slots = 0;
};

/// PHY-FAPI P7 sector fastpath adaptor dependencies.
struct phy_fapi_p7_sector_fastpath_adaptor_dependencies {
  /// Logger.
  ocudulog::basic_logger& logger;
  /// Downlink processor pool.
  downlink_processor_pool& dl_processor_pool;
  /// Downlink resource grid pool.
  resource_grid_pool& dl_rg_pool;
  /// Downlink PDU validator.
  const downlink_pdu_validator& dl_pdu_validator;
  /// Uplink request processor.
  uplink_request_processor& ul_request_processor;
  /// Uplink slot PDU repository.
  uplink_pdu_slot_repository_pool& ul_pdu_repository;
  /// Uplink PDU validator.
  const uplink_pdu_validator& ul_pdu_validator;
  /// Precoding codebook repository.
  std::unique_ptr<precoding_codebook_repository> pm_repo;
  /// UCI Part2 correspondence repository.
  std::unique_ptr<uci_part2_correspondence_repository> part2_repo;
};

} // namespace fapi_adaptor
} // namespace ocudu
