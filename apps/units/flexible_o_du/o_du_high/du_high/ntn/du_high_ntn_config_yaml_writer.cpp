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

static YAML::Node build_ecef_node(const ecef_coordinates_t& ecef)
{
  YAML::Node node;
  node["pos_x"] = ecef.position_x;
  node["pos_y"] = ecef.position_y;
  node["pos_z"] = ecef.position_z;
  node["vel_x"] = ecef.velocity_vx;
  node["vel_y"] = ecef.velocity_vy;
  node["vel_z"] = ecef.velocity_vz;
  return node;
}

static YAML::Node build_orbital_node(const orbital_coordinates_t& orb)
{
  YAML::Node node;
  node["semi_major_axis"] = orb.semi_major_axis;
  node["eccentricity"]    = orb.eccentricity;
  node["periapsis"]       = orb.periapsis;
  node["longitude"]       = orb.longitude;
  node["inclination"]     = orb.inclination;
  node["mean_anomaly"]    = orb.mean_anomaly;
  return node;
}

static YAML::Node build_geodetic_node(const geodetic_coordinates_t& loc, bool with_altitude = true)
{
  YAML::Node node;
  node["latitude"]  = loc.latitude;
  node["longitude"] = loc.longitude;
  if (with_altitude) {
    node["altitude"] = loc.altitude;
  }
  return node;
}

static YAML::Node build_ta_info_node(const ta_info_t& ta)
{
  YAML::Node node;
  node["ta_common"]               = ta.ta_common;
  node["ta_common_drift"]         = ta.ta_common_drift;
  node["ta_common_drift_variant"] = ta.ta_common_drift_variant;
  if (ta.ta_common_offset) {
    node["ta_common_offset"] = *ta.ta_common_offset;
  }
  return node;
}

static YAML::Node build_polarization_node(const ntn_polarization_t& pol)
{
  YAML::Node node;
  if (pol.dl) {
    node["dl"] = to_string(*pol.dl);
  }
  if (pol.ul) {
    node["ul"] = to_string(*pol.ul);
  }
  return node;
}

static void fill_optional_ephemeris(YAML::Node&                                                                   node,
                                    const std::optional<std::variant<ecef_coordinates_t, orbital_coordinates_t>>& ephem)
{
  if (!ephem) {
    return;
  }
  if (const auto* ecef = std::get_if<ecef_coordinates_t>(&*ephem)) {
    node["ephemeris_info_ecef"] = build_ecef_node(*ecef);
  } else if (const auto* orb = std::get_if<orbital_coordinates_t>(&*ephem)) {
    node["ephemeris_orbital"] = build_orbital_node(*orb);
  }
}

void ocudu::fill_ntn_config_in_yaml_schema(YAML::Node& node, const du_high_unit_cell_ntn_config& config)
{
  auto ntn_node = node["ntn"];

  ntn_node["cell_specific_koffset"] = config.cell_specific_koffset.count();

  if (config.ntn_ul_sync_validity_dur) {
    ntn_node["ntn_ul_sync_validity_dur"] = *config.ntn_ul_sync_validity_dur;
  }

  if (config.ta_info) {
    ntn_node["ta_info"] = build_ta_info_node(*config.ta_info);
  }

  if (config.epoch_timestamp) {
    ntn_node["epoch_timestamp"] = timepoint_to_iso8601(*config.epoch_timestamp);
  }

  if (config.feeder_link_info) {
    YAML::Node fl_node;
    fl_node["enable_doppler_compensation"] = config.feeder_link_info->enable_doppler_compensation;
    fl_node["dl_freq"]                     = config.feeder_link_info->dl_freq;
    fl_node["ul_freq"]                     = config.feeder_link_info->ul_freq;
    ntn_node["feeder_link_info"]           = fl_node;
  }

  if (config.ntn_gateway_location) {
    ntn_node["ntn_gateway_location"] = build_geodetic_node(*config.ntn_gateway_location);
  }

  if (config.epoch_time) {
    YAML::Node epoch_node;
    epoch_node["sfn"]             = config.epoch_time->sfn;
    epoch_node["subframe_number"] = config.epoch_time->subframe_number;
    ntn_node["epoch_time"]        = epoch_node;
  }

  if (config.epoch_sfn_offset) {
    ntn_node["epoch_sfn_offset"] = *config.epoch_sfn_offset;
  }

  if (config.use_state_vector) {
    ntn_node["use_state_vector"] = *config.use_state_vector;
  }

  ntn_node["propagator_type"] =
      (config.propagator_type == ocudu_ntn::orbit_propagator_type::keplerian) ? "keplerian" : "rk4";

  fill_optional_ephemeris(ntn_node, config.ephemeris_info);

  if (config.polarization) {
    ntn_node["polarization"] = build_polarization_node(*config.polarization);
  }

  if (config.ta_report) {
    ntn_node["ta_report"] = *config.ta_report;
  }

  if (config.reference_location) {
    ntn_node["reference_location"] = build_geodetic_node(*config.reference_location, false);
  }

  if (config.distance_threshold) {
    ntn_node["distance_threshold"] = *config.distance_threshold;
  }

  if (config.t_service) {
    ntn_node["t_service"] = timepoint_to_iso8601(*config.t_service);
  }

  if (config.moving_ref_location) {
    ntn_node["moving_ref_location"] = build_geodetic_node(*config.moving_ref_location, false);
  }

  // NTN neighbor cells.
  if (!config.ncells.empty()) {
    YAML::Node ncells_node;
    for (const auto& ncell : config.ncells) {
      YAML::Node ncell_node;
      if (ncell.phys_cell_id) {
        ncell_node["pci"] = static_cast<unsigned>(*ncell.phys_cell_id);
      }
      if (ncell.carrier_freq) {
        ncell_node["carrier_freq"] = ncell.carrier_freq->value();
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
