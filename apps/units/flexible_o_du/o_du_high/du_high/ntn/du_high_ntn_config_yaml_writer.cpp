// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "du_high_ntn_config_yaml_writer.h"
#include "du_high_unit_cell_ntn_config.h"

using namespace ocudu;

/// Convert time_point to ISO 8601 format string (YYYY-MM-DDTHH:MM:SS.mmm).
static std::string timepoint_to_iso8601(const std::chrono::system_clock::time_point& tp)
{
  auto   ms           = std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch());
  auto   secs         = std::chrono::duration_cast<std::chrono::seconds>(ms);
  time_t time         = secs.count();
  int    milliseconds = (ms.count() % 1000);

  std::tm tm_utc = *std::gmtime(&time);
  char    buf[32];
  std::strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%S", &tm_utc);

  // Format milliseconds with leading zeros.
  char ms_buf[8];
  std::snprintf(ms_buf, sizeof(ms_buf), ".%03d", milliseconds);

  return std::string(buf) + ms_buf;
}

void ocudu::fill_ntn_config_in_yaml_schema(YAML::Node& node, const du_high_unit_cell_ntn_config& config)
{
  auto ntn_node = node["ntn"];

  ntn_node["cell_specific_koffset"] = config.cell_specific_koffset.count();

  if (config.ntn_ul_sync_validity_dur) {
    ntn_node["ntn_ul_sync_validity_dur"] = config.ntn_ul_sync_validity_dur.value();
  }

  if (config.ta_info) {
    YAML::Node       ta_info_node;
    const ta_info_t& ta_info                = *config.ta_info;
    ta_info_node["ta_common"]               = ta_info.ta_common;
    ta_info_node["ta_common_drift"]         = ta_info.ta_common_drift;
    ta_info_node["ta_common_drift_variant"] = ta_info.ta_common_drift_variant;
    if (ta_info.ta_common_offset) {
      ta_info_node["ta_common_offset"] = *ta_info.ta_common_offset;
    }

    ntn_node["ta_info"] = ta_info_node;
  }

  if (config.epoch_timestamp) {
    ntn_node["epoch_timestamp"] = timepoint_to_iso8601(config.epoch_timestamp.value());
  }

  if (config.feeder_link_info) {
    YAML::Node fl_node;
    fl_node["enable_doppler_compensation"] = config.feeder_link_info.value().enable_doppler_compensation;
    fl_node["dl_freq"]                     = config.feeder_link_info.value().dl_freq;
    fl_node["ul_freq"]                     = config.feeder_link_info.value().ul_freq;

    ntn_node["feeder_link_info"] = fl_node;
  }

  if (config.ntn_gateway_location) {
    YAML::Node gw_loc_node;
    gw_loc_node["latitude"]  = config.ntn_gateway_location.value().latitude;
    gw_loc_node["longitude"] = config.ntn_gateway_location.value().longitude;
    gw_loc_node["altitude"]  = config.ntn_gateway_location.value().altitude;

    ntn_node["ntn_gateway_location"] = gw_loc_node;
  }

  if (config.epoch_time.has_value()) {
    YAML::Node epoch_node;
    epoch_node["sfn"]             = config.epoch_time.value().sfn;
    epoch_node["subframe_number"] = config.epoch_time.value().subframe_number;

    ntn_node["epoch_time"] = epoch_node;
  }

  if (config.epoch_sfn_offset) {
    ntn_node["epoch_sfn_offset"] = config.epoch_sfn_offset.value();
  }

  if (config.use_state_vector) {
    ntn_node["use_state_vector"] = config.use_state_vector.value();
  }

  ntn_node["propagator_type"] =
      (config.propagator_type == ocudu_ntn::orbit_propagator_type::keplerian) ? "keplerian" : "rk4";

  if (std::holds_alternative<ecef_coordinates_t>(config.ephemeris_info)) {
    YAML::Node  ephemeris_node;
    const auto& ephem       = std::get<ecef_coordinates_t>(config.ephemeris_info);
    ephemeris_node["pos_x"] = ephem.position_x;
    ephemeris_node["pos_y"] = ephem.position_y;
    ephemeris_node["pos_z"] = ephem.position_z;
    ephemeris_node["vel_x"] = ephem.velocity_vx;
    ephemeris_node["vel_y"] = ephem.velocity_vy;
    ephemeris_node["vel_z"] = ephem.velocity_vz;

    ntn_node["ephemeris_info_ecef"] = ephemeris_node;
  }

  if (std::holds_alternative<orbital_coordinates_t>(config.ephemeris_info)) {
    YAML::Node  orb_node;
    const auto& orb             = std::get<orbital_coordinates_t>(config.ephemeris_info);
    orb_node["semi_major_axis"] = orb.semi_major_axis;
    orb_node["eccentricity"]    = orb.eccentricity;
    orb_node["periapsis"]       = orb.periapsis;
    orb_node["longitude"]       = orb.longitude;
    orb_node["inclination"]     = orb.inclination;
    orb_node["mean_anomaly"]    = orb.mean_anomaly;

    ntn_node["ephemeris_orbital"] = orb_node;
  }

  if (config.polarization.has_value()) {
    const auto& pol = config.polarization.value();
    YAML::Node  pol_node;
    auto        pol_type_to_str = [](ntn_polarization_t::polarization_type t) -> const char* {
      switch (t) {
        case ntn_polarization_t::polarization_type::rhcp:
          return "rhcp";
        case ntn_polarization_t::polarization_type::lhcp:
          return "lhcp";
        default:
          return "linear";
      }
    };
    if (pol.dl) {
      pol_node["dl"] = pol_type_to_str(pol.dl.value());
    }
    if (pol.ul) {
      pol_node["ul"] = pol_type_to_str(pol.ul.value());
    }
    ntn_node["polarization"] = pol_node;
  }

  if (config.ta_report.has_value()) {
    ntn_node["ta_report"] = config.ta_report.value();
  }

  if (config.reference_location.has_value()) {
    YAML::Node ref_loc_node;
    ref_loc_node["latitude"]       = config.reference_location.value().latitude;
    ref_loc_node["longitude"]      = config.reference_location.value().longitude;
    ntn_node["reference_location"] = ref_loc_node;
  }
  if (config.distance_threshold.has_value()) {
    ntn_node["distance_threshold"] = config.distance_threshold.value();
  }

  if (config.t_service.has_value()) {
    ntn_node["t_service"] = timepoint_to_iso8601(config.t_service.value());
  }

  // Moving reference location (R18 extension).
  if (config.moving_ref_location.has_value()) {
    YAML::Node mov_ref_node;
    mov_ref_node["latitude"]        = config.moving_ref_location.value().latitude;
    mov_ref_node["longitude"]       = config.moving_ref_location.value().longitude;
    ntn_node["moving_ref_location"] = mov_ref_node;
  }

  // NTN neighbor cells.
  if (!config.ncells.empty()) {
    YAML::Node ncells_node;
    for (const auto& ncell : config.ncells) {
      YAML::Node ncell_node;
      if (ncell.phys_cell_id.has_value()) {
        ncell_node["pci"] = static_cast<unsigned>(ncell.phys_cell_id.value());
      }
      if (ncell.carrier_freq.has_value()) {
        ncell_node["carrier_freq"] = ncell.carrier_freq.value().value();
      }
      ncells_node.push_back(ncell_node);
    }
    ntn_node["ncells"] = ncells_node;
  }

  // Satellite switch with resynchronization (R18 extension).
  if (config.sat_switch_with_resync.has_value()) {
    const auto& sat_sw = config.sat_switch_with_resync.value();
    YAML::Node  sat_sw_node;

    // TODO: add ntn-config dump.

    if (sat_sw.t_service_start.has_value()) {
      sat_sw_node["t_service_start"] = timepoint_to_iso8601(sat_sw.t_service_start.value());
    }

    if (sat_sw.ssb_time_offset_sf.has_value()) {
      sat_sw_node["ssb_time_offset_sf"] = static_cast<unsigned>(sat_sw.ssb_time_offset_sf->value());
    }

    ntn_node["sat_switch_with_resync"] = sat_sw_node;
  }
}
