// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ntn_config_yaml_writer.h"
#include "ntn_satellite_config.h"

using namespace ocudu;

std::string ocudu::ntn_timepoint_to_iso8601(const std::chrono::system_clock::time_point& tp)
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

YAML::Node ocudu::build_geodetic_yaml_node(const geodetic_coordinates_t& loc, bool with_altitude)
{
  YAML::Node node;
  node["latitude"]  = loc.latitude;
  node["longitude"] = loc.longitude;
  if (with_altitude) {
    node["altitude"] = loc.altitude;
  }
  return node;
}

YAML::Node ocudu::build_ntn_polarization_yaml_node(const ntn_polarization_t& pol)
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

static void fill_optional_ephemeris(YAML::Node& node, const std::optional<ntn_ephemeris_info_t>& ephem)
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

void ocudu::fill_ntn_satellite_in_yaml_schema(YAML::Node& node, const ntn_satellite_config& config)
{
  if (config.satellite_idx) {
    node["satellite_idx"] = *config.satellite_idx;
  }

  if (config.epoch_timestamp) {
    node["epoch_timestamp"] = ntn_timepoint_to_iso8601(*config.epoch_timestamp);
  }

  fill_optional_ephemeris(node, config.ephemeris_info);

  if (config.gateway_location) {
    node["gateway_location"] = build_geodetic_yaml_node(*config.gateway_location);
  }

  if (config.ta_info) {
    node["ta_info"] = build_ta_info_node(*config.ta_info);
  }
  node["propagator_type"] =
      (config.propagator_type == ocudu_ntn::orbit_propagator_type::keplerian) ? "keplerian" : "rk4";
}

void ocudu::fill_ntn_satellites_in_yaml_schema(YAML::Node& node, const std::vector<ntn_satellite_config>& satellites)
{
  if (satellites.empty()) {
    return;
  }

  auto       ntn_node = node["ntn"];
  YAML::Node sats_node;

  for (const auto& sat : satellites) {
    YAML::Node sat_node;
    fill_ntn_satellite_in_yaml_schema(sat_node, sat);
    sats_node.push_back(sat_node);
  }

  ntn_node["satellites"] = sats_node;
}
