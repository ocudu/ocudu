// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ntn/orbit_propagator_type.h"
#include "ocudu/ran/nr_cgi.h"
#include "ocudu/ran/ntn.h"

namespace ocudu {
namespace ocudu_ntn {

/// NTN serving cell configuration: static fields used for SIB19 generation.
/// Dynamic fields (ephemeris, ta_info, epoch_time) are filled by the generator from the orbital state.
struct ntn_serving_cell_config {
  /// SIB19 fields exempt from SIB1 valuetag (changes don't trigger SI change notifications).
  /// Moving reference location for NTN Earth-moving cell. Exempt from SI change determination per TS 38.331.
  std::optional<geodetic_coordinates_t> moving_reference_location;
  /// Cell-level constant offset added to ta_common in SIB19, modelling fixed system delays (e.g. cable, processing).
  std::optional<double> ta_common_offset;
  /// If present, overrides the epoch time SFN/subframe broadcast in SIB19. Usually not needed.
  std::optional<epoch_time_t> epoch_time;
  /// Validity duration for UL sync assistance info in seconds. Exempt from SI change determination.
  std::optional<unsigned> ntn_ul_sync_validity_dur;

  /// SIB19 fields tracked by SIB1 valuetag (changes require valuetag modification).
  /// Reference location for NTN quasi-Earth fixed cell (in degrees).
  std::optional<geodetic_coordinates_t> reference_location;
  /// Distance threshold from serving cell reference location. Each step represents 50m.
  std::optional<unsigned> distance_threshold;
  /// Time when cell stops serving current area (UTC timepoint).
  std::optional<std::chrono::system_clock::time_point> t_service;
  /// Cell-specific scheduling offset (k_offset) for NTN, in milliseconds.
  std::chrono::milliseconds cell_specific_koffset;
  /// Scheduling offset k_mac if DL/UL frame timing not aligned.
  std::optional<unsigned> k_mac;
  /// Polarization info for service link.
  std::optional<ntn_polarization_t> polarization;
  /// Indicates if timing advance reporting is enabled.
  std::optional<bool> ta_report;
  /// NTN coverage enhancements defines parameters used to improve UE connectivity under high path-loss conditions.
  std::optional<ntn_cov_enh_t> coverage_enhancements;

  /// Metadata fields (not directly in SIB19, used for SIB19 generation):
  /// Offset in SFN between SIB19 transmission and epoch time.
  std::optional<uint64_t> epoch_sfn_offset;
  /// Use ECEF state vectors (true) or ECI orbital parameters (false) for ephemeris format.
  std::optional<bool> use_state_vector;
  /// Feeder link parameters for Doppler computation (backend).
  std::optional<feeder_link_info_t> feeder_link_info;
};

/// Neighbor NTN cell configuration: static (non-satellite-computed) fields only.
/// Dynamic fields (ephemeris, ta_info, epoch_time) are filled by the generator from the orbital state.
struct ntn_neighbor_cell_config {
  unsigned                                 satellite_index;
  std::optional<arfcn_t>                   carrier_freq;
  std::optional<pci_t>                     phys_cell_id;
  std::optional<std::chrono::milliseconds> cell_specific_koffset;
  std::optional<unsigned>                  ntn_ul_sync_validity_dur;
  std::optional<unsigned>                  k_mac;
  std::optional<ntn_polarization_t>        polarization;
  std::optional<bool>                      ta_report;
};

/// Satellite-switch configuration: static (non-satellite-computed) fields only.
/// Dynamic fields (ephemeris, ta_info, epoch_time) are filled by the generator from the orbital state.
struct ntn_sat_switch_config {
  unsigned                                                   satellite_index;
  std::optional<std::chrono::system_clock::time_point>       t_service_start;
  std::optional<sat_switch_with_resync_t::ssb_time_offset_t> ssb_time_offset_sf;
  std::optional<unsigned>                                    ntn_ul_sync_validity_dur;
  std::optional<std::chrono::milliseconds>                   cell_specific_koffset;
  std::optional<unsigned>                                    k_mac;
  std::optional<ntn_polarization_t>                          polarization;
  std::optional<bool>                                        ta_report;
};

/// NTN Cell configuration.
struct ntn_cell_config {
  /// NR-CGI.
  nr_cell_global_id_t nr_cgi;
  /// Sector Id (4-14 bits).
  std::optional<unsigned> sector_id;
  /// SIB19 scheduling information.
  unsigned si_msg_idx;
  unsigned si_period_rf;
  unsigned si_window_len_slots;
  unsigned si_window_position;
  /// Satellite this cell is associated with.
  unsigned satellite_index = 0;
  /// NTN serving cell configuration (SIB19 fields and generation metadata).
  ntn_serving_cell_config ntn_cfg;
  /// Neighbor NTN cells listed in SIB19.
  static_vector<ntn_neighbor_cell_config, MAX_NOF_NTN_NEIGHBORS> ncells;
  /// Satellite-switch target configuration. Absent if sat-switch is not configured.
  std::optional<ntn_sat_switch_config> sat_switch;
};

/// NTN Satellite configuration (orbital propagation state, shared across cells).
struct ntn_satellite_config {
  /// Index used to associate cells with this satellite.
  unsigned satellite_index;
  /// Epoch timestamp (UTC timepoint) for ephemeris information.
  std::optional<std::chrono::system_clock::time_point> epoch_timestamp;
  /// Satellite ephemeris: ECEF state vector or ECI orbital parameters.
  ntn_ephemeris_info_t ephemeris_info;
  /// Geodetic coordinates (in degrees) of the NTN Gateway. Used by the orbital propagation module to compute TA-Info.
  std::optional<geodetic_coordinates_t> ntn_gateway_location;
  /// If present, overrides the TA info computed by the orbital propagation module (transparent architecture).
  /// Mutually exclusive with ntn_gateway_location.
  std::optional<ta_info_t> ta_info;
  /// Orbit propagation algorithm to use for this satellite.
  orbit_propagator_type propagator_type = orbit_propagator_type::rk4;
};

/// NTN Configuration manager config.
struct ntn_configuration_manager_config {
  /// NTN Satellite configuration (one entry per satellite).
  std::vector<ntn_satellite_config> satellites;
  /// NTN Cell configuration.
  std::vector<ntn_cell_config> cells;
};

} // namespace ocudu_ntn
} // namespace ocudu
