// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/arfcn.h"
#include "ocudu/ran/pci.h"
#include <chrono>
#include <optional>
#include <variant>

namespace ocudu {

/// Satellite ephemeris in format of position and velocity state vector in ECEF.
struct ecef_coordinates_t {
  /// X coordinate of position state vector. Unit is meter.
  double position_x;
  /// Y coordinate of position state vector. Unit is meter.
  double position_y;
  /// Z coordinate of position state vector. Unit is meter.
  double position_z;
  /// X coordinate of velocity state vector. Unit is meter/second.
  double velocity_vx;
  /// Y coordinate of velocity state vector. Unit is meter/second.
  double velocity_vy;
  /// Z coordinate of velocity state vector. Unit is meter/second.
  double velocity_vz;
};

/// Satellite ephemeris in format of orbital parameters in ECI. See NIMA TR 8350.2.
struct orbital_coordinates_t {
  /// Semi major axis. Unit is meter.
  double semi_major_axis;
  /// Eccentricity. Unitless.
  double eccentricity;
  /// Argument of periapsis. Unit is radian.
  double periapsis;
  /// Longitude of ascending node. Unit is radian.
  double longitude;
  /// Mean anomaly M at epoch time. Unit is radian.
  double mean_anomaly;
  /// Inclination. Unit is radian.
  double inclination;
};

/// Satellite ephemeris, either as an ECEF state vector or as ECI orbital parameters.
using ntn_ephemeris_info_t = std::variant<ecef_coordinates_t, orbital_coordinates_t>;

/// Timing advance information for NTN.
struct ta_info_t {
  /// Network-controlled common timing advanced value and it may include any timing offset considered necessary by the
  /// network. ta-Common with value of 0 is supported. Unit is us.
  double ta_common;
  /// Indicate drift rate of the common TA. Unit is us/s.
  double ta_common_drift;
  /// Indicate drift rate variation of the common TA. Unit is us/s^2.
  double ta_common_drift_variant;
  /// Constant offset added to the NTA-common broadcast in SIB19 to model fixed system delays independent of satellite
  /// position. Unit is us.
  std::optional<double> ta_common_offset;
};

/// EpochTime is used to indicate the epoch time for the NTN assistance information, and it is defined as the starting
/// time of a DL sub-frame, indicated by a SFN and a sub-frame number signaled together with the assistance information.
struct epoch_time_t {
  /// For serving cell, it indicates the current SFN or the next upcoming SFN after the frame where the message
  /// indicating the epochTime is received. For neighbour cell, it indicates the SFN nearest to the frame where the
  /// message indicating the epochTime is received.
  unsigned sfn;
  /// Sub-frame number within the SFN.
  unsigned subframe_number;
};

/// Parameters of the feeder link used to compute the Doppler shifts.
struct feeder_link_info_t {
  /// Flag to enable/disable doppler compensation for the feeder link.
  bool enable_doppler_compensation;
  /// Downlink frequency of the feeder link. Unit is Hz.
  double dl_freq;
  /// Uplink frequency of the feeder link. Unit is Hz.
  double ul_freq;
};

/// Geodetic coordinates of the NTN Gateway location.
struct geodetic_coordinates_t {
  /// Latitude. Unit is degree.
  double latitude;
  /// Longitude. Unit is degree.
  double longitude;
  /// Altitude. Unit is meter.
  double altitude;
};

/// Indicates polarization information for downlink/uplink transmission on service link.
struct ntn_polarization_t {
  enum class polarization_type { rhcp, lhcp, linear };
  /// If present, this parameter indicates polarization information for downlink transmission on service link.
  std::optional<polarization_type> dl;
  /// If present, this parameter indicates Polarization information for uplink service link.
  std::optional<polarization_type> ul;
};

inline const char* to_string(ntn_polarization_t::polarization_type pt)
{
  switch (pt) {
    case ntn_polarization_t::polarization_type::rhcp:
      return "rhcp";
    case ntn_polarization_t::polarization_type::lhcp:
      return "lhcp";
    case ntn_polarization_t::polarization_type::linear:
      return "linear";
    default:
      return "invalid";
  }
}

/// NTN config parameters.
struct ntn_config {
  /// Reference epoch time for NTN assistance information.
  std::optional<epoch_time_t> epoch_time;
  /// A validity duration configured by the network for assistance information which indicates the maximum time duration
  /// (from epochTime) during which the UE can apply assistance information without having acquired new assistance
  /// information. Values {5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 120, 180, 240, 900} seconds.
  std::optional<unsigned> ntn_ul_sync_validity_dur;
  /// Scheduling offset used for timing relationships modified for NTN operation (see TS 38.213 and TS 38.300,
  /// Section 16.14.2). The unit is milliseconds.
  ///
  /// \note In the specifications, the K_offset field is expressed as a number of slots assuming a subcarrier spacing of
  /// 15 kHz (i.e., 1 slot = 1 ms). To avoid ambiguity with other subcarrier spacings, this parameter is represented in
  /// the implementation as std::chrono::milliseconds.
  std::optional<std::chrono::milliseconds> cell_specific_koffset;
  /// Scheduling offset provided by network if downlink and uplink frame timing are not aligned at gNB.
  std::optional<unsigned> k_mac;
  /// Network-controlled common timing advanced value, and it may include any timing offset considered necessary by the
  /// network.
  std::optional<ta_info_t> ta_info;
  /// Indicates polarization information for downlink/uplink transmission on service link.
  std::optional<ntn_polarization_t> polarization;
  /// This field provides satellite ephemeris either in format of position and velocity state vector or in format of
  /// orbital parameters.
  std::optional<ntn_ephemeris_info_t> ephemeris_info;
  /// When this field is included in SIB19, it indicates reporting of timing advanced is enabled.
  std::optional<bool> ta_report;
};

/// NTN coverage enhancements defines parameters used to improve UE connectivity under high path-loss conditions.
struct ntn_cov_enh_t {
  /// The number of repetition slots for PUCCH transmission with HARQ-ACK information for Msg4, see clause 9.2.6 in
  /// TS 38.213.
  unsigned nof_msg4_harq_ack_rep;
  /// This threshold is used by the UE for determining the configuration of the MAC entity for PUCCH repetition for Msg4
  /// HARQ-ACK, as specified in clause 6.2.1 in TS 38.321.
  std::optional<unsigned> rsrp_thres_msg4_harq_ack;
};

/// Provides parameters for the target satellite required to perform satellite switch with resynchronization.
struct sat_switch_with_resync_t {
  using ssb_time_offset_t = bounded_integer<uint8_t, 0, 159>;
  /// NTN config.
  ntn_config ntn_cfg;
  /// Indicate the epoch time for the NTN assistance information in the NTN-config. UTC timepoint.
  std::optional<std::chrono::system_clock::time_point> epoch_timestamp;
  /// Geodetic coordinates (in degrees) of the NTN Gateway location. Used to compute TA-info.
  std::optional<geodetic_coordinates_t> ntn_gateway_location;
  /// Indicates the time information on when the target satellite is going to start serving the area currently covered
  /// by the serving satellite. The reference point for t-ServiceStart is the uplink time synchronization reference
  /// point of the serving satellite. UTC timepoint.
  std::optional<std::chrono::system_clock::time_point> t_service_start;
  /// Indicates the time offset of the SSB from target satellite at its uplink time synchronization reference point with
  /// respect to the SSB from source satellite at its uplink time synchronization reference point. It is given in
  /// number of subframes.  Values: [0, 159]
  std::optional<ssb_time_offset_t> ssb_time_offset_sf;
};

/// Max number of neighboring NTN cells.
/// Derived from ASN.1: ntn-NeighCellConfigList and ntn-NeighCellConfigListExt each hold up to 4 entries, giving 8
/// total.
constexpr size_t MAX_NOF_NTN_NEIGHBORS = 8U;

/// Neighbor NTN cell configuration.
struct neighbor_ntn_cell {
  std::optional<ntn_config> ntn_cfg;
  std::optional<arfcn_t>    carrier_freq;
  std::optional<pci_t>      phys_cell_id;
};

} // namespace ocudu
