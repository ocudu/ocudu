// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "apps/helpers/ntn/ntn_satellite_config.h"
#include "ocudu/ran/ntn.h"
#include <chrono>
#include <optional>
#include <vector>

namespace ocudu {

/// Application-level per-neighbor NTN cell configuration.
/// Each neighbor must either reference a globally-defined satellite_idx or provide an inline satellite definition
/// with epoch_timestamp and ephemeris_info.
struct du_high_unit_ntn_neighbor_cell_config {
  /// Reference to the neighbor's satellite (global reference or inline definition).
  ntn_satellite_config sat_ref;

  /// Per-neighbor identity.
  std::optional<pci_t>   phys_cell_id;
  std::optional<arfcn_t> carrier_freq;

  /// Per-neighbor NTN parameters.
  std::optional<std::chrono::milliseconds> cell_specific_koffset;
  std::optional<unsigned>                  ntn_ul_sync_validity_dur;
  std::optional<unsigned>                  k_mac;
  std::optional<ntn_polarization_t>        polarization;
  std::optional<bool>                      ta_report;
  std::optional<bool>                      use_state_vector;
};

/// Application-level sat_switch_with_resync configuration.
struct du_high_unit_sat_switch_config {
  /// Reference to the switch target's satellite (global reference or inline definition).
  ntn_satellite_config sat_ref;

  /// Switch timing.
  std::optional<std::chrono::system_clock::time_point> t_service_start;
  std::optional<unsigned>                              ssb_time_offset_sf;

  /// Target cell NTN parameters after switch.
  std::optional<unsigned>                  ntn_ul_sync_validity_dur;
  std::optional<std::chrono::milliseconds> cell_specific_koffset;
  std::optional<unsigned>                  k_mac;
  std::optional<ntn_polarization_t>        polarization;
  std::optional<bool>                      ta_report;
  std::optional<bool>                      use_state_vector;

  /// Whether to promote this sat-switch's target parameters to become the serving cell's NTN config at the serving
  /// cell's t_service (when the source satellite stops serving, per TS 38.331 clause 5.7.19), on top of advertising
  /// it in SIB19 up to that point. Requires t_service to be set in the serving cell NTN config.
  bool promote_to_serving = false;
  /// When promote_to_serving is enabled, whether to keep the pre-switch neighbor cell list (ncells) unchanged
  /// (true) or clear it (false) in the promoted config.
  bool promote_neighbors = false;
};

/// Application-level NTN configuration for an NTN serving cell (NTN band). Absent for a TN-band serving cell
/// that only reports NTN neighbor cells (see du_high_unit_cell_ntn_config::serving).
struct du_high_unit_ntn_serving_cell_config {
  /// Reference to the serving cell's satellite (global reference or inline definition).
  ntn_satellite_config sat_ref;

  /// Reference location of the serving cell in geodetic coordinates format (in degrees).
  std::optional<geodetic_coordinates_t> reference_location;
  /// Distance from the serving cell reference location, as defined in TS 38.304. Each step represents 50m.
  std::optional<unsigned> distance_threshold;
  /// Indicates the time information on when a cell provided via NTN is going to stop serving the area it is currently
  /// covering. UTC timepoint.
  std::optional<std::chrono::system_clock::time_point> t_service;
  /// Optional offset (in SFN) between the SIB19 transmission slot and the epoch time (EpochTime IE) of the NTN
  /// assistance info. Allows sending NTN assistance information that will become valid epoch_sfn_offset number of
  /// system frames after SIB19 Tx slot.
  std::optional<uint64_t> epoch_sfn_offset;
  /// If provided it will be used to fill the EpochTime section in SIB19.
  std::optional<epoch_time_t> epoch_time;
  /// Scheduling offset used for timing relationships modified for NTN operation (see TS 38.213 and TS 38.300,
  /// Section 16.14.2). The unit is milliseconds.
  ///
  /// \note In the specifications, the K_offset field is expressed as a number of slots assuming a subcarrier spacing of
  /// 15 kHz (i.e., 1 slot = 1 ms). To avoid ambiguity with other subcarrier spacings, this parameter is represented in
  /// the implementation as std::chrono::milliseconds.
  std::chrono::milliseconds cell_specific_koffset;
  /// Scheduling offset provided by network if downlink and uplink frame timing are not aligned at gNB.
  std::optional<unsigned> k_mac;
  /// A validity duration configured by the network for assistance information which indicates the maximum time duration
  /// (from epochTime) during which the UE can apply assistance information without having acquired new assistance
  /// information. Values {5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 120, 180, 240, 900} seconds.
  /// Per TS 38.331, an absent ntn-UlSyncValidityDuration for the NTN serving cell has no defined fallback (unlike
  /// the TN-cell case, which is explicitly left to UE implementation), and neighbor cells/sat-switch may inherit
  /// this value when they omit their own.
  unsigned ntn_ul_sync_validity_dur = 5;
  /// Whether to broadcast Ephemeris information as ECEF state vectors (if true) or ECI Orbital parameters (if false).
  /// If not provided, the value is derived from the variant of ephemeris_info.
  /// If provided and does not match the variant of ephemeris_info, the ephemeris_info is converted accordingly.
  std::optional<bool> use_state_vector;
  /// Parameters of the feeder link used to compute the Doppler shifts.
  std::optional<feeder_link_info_t> feeder_link_info;
  /// Indicates polarization information for downlink/uplink transmission on service link.
  std::optional<ntn_polarization_t> polarization;
  /// Indicates reporting of timing advance is enabled. Per TS 38.331, ta-Report is mandatory present for the
  /// serving cell in SIB19 (unlike for neighbor cells or SatSwitchWithReSync, where it is optional), so this field
  /// is not optional and defaults to disabled.
  bool ta_report = false;
  /// Moving reference location for NTN Earth-moving cell (R18).
  std::optional<geodetic_coordinates_t> moving_ref_location;
  /// Satellite switch with resynchronization parameters (R18).
  std::optional<du_high_unit_sat_switch_config> sat_switch_with_resync;
};

/// Application-level per-cell NTN configuration. Valid both for an NTN serving cell (NTN band, \c serving
/// populated) and for a TN-band serving cell that only reports NTN neighbor cells in SIB19 (\c serving absent).
struct du_high_unit_cell_ntn_config {
  /// NTN serving cell configuration. Present only when this cell is an NTN serving cell (NTN band); absent
  /// for a TN-band serving cell that only configures \c ncells.
  std::optional<du_high_unit_ntn_serving_cell_config> serving;
  /// List of NTN neighbor cells. Valid for both TN and NTN serving cells.
  std::vector<du_high_unit_ntn_neighbor_cell_config> ncells;
};

} // namespace ocudu
