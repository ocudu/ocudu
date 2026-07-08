// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ntn_config_update_remote_command.h"
#include "nlohmann/json.hpp"
#include "ocudu/adt/expected.h"
#include "ocudu/ran/ntn.h"
#include "ocudu/support/json_utils.h"
#include <ctime>
#include <iomanip>
#include <sstream>

using namespace ocudu;
using namespace ocudu_ntn;

namespace ocudu {

OCUDU_JSON_DEFINE_TYPE_STRUCT(feeder_link_info_t,
                              (enable_doppler_compensation, "enable_doppler_compensation"),
                              (dl_freq, "dl_freq"),
                              (ul_freq, "ul_freq"))

OCUDU_JSON_DEFINE_TYPE_STRUCT(geodetic_coordinates_t,
                              (latitude, "latitude"),
                              (longitude, "longitude"),
                              (altitude, "altitude"))

OCUDU_JSON_DEFINE_TYPE_STRUCT(ta_info_t,
                              (ta_common, "ta_common", required),
                              (ta_common_drift, "ta_common_drift", optional),
                              (ta_common_drift_variant, "ta_common_drift_variant", optional))

OCUDU_JSON_DEFINE_TYPE_STRUCT(ecef_coordinates_t,
                              (position_x, "position_x"),
                              (position_y, "position_y"),
                              (position_z, "position_z"),
                              (velocity_vx, "velocity_vx"),
                              (velocity_vy, "velocity_vy"),
                              (velocity_vz, "velocity_vz"))

OCUDU_JSON_DEFINE_TYPE_STRUCT(orbital_coordinates_t,
                              (semi_major_axis, "semi_major_axis"),
                              (eccentricity, "eccentricity"),
                              (periapsis, "periapsis"),
                              (longitude, "longitude"),
                              (inclination, "inclination"),
                              (mean_anomaly, "mean_anomaly"))
} // namespace ocudu

static expected<plmn_identity, std::string> parse_plmn(const nlohmann::json& json)
{
  auto plmn_result = validate_json<std::string>(json, "plmn");
  if (!plmn_result) {
    return make_unexpected(plmn_result.error());
  }
  auto plmn = plmn_identity::parse(plmn_result->get_ref<const nlohmann::json::string_t&>());
  if (!plmn) {
    return make_unexpected("Invalid PLMN identity value");
  }
  return *plmn;
}

static expected<nr_cell_identity, std::string> parse_nci(const nlohmann::json& json)
{
  auto nci_result = validate_json<uint64_t>(json, "nci");
  if (!nci_result) {
    return make_unexpected(nci_result.error());
  }
  auto nci = nr_cell_identity::create(nci_result->get<uint64_t>());
  if (!nci) {
    return make_unexpected("Invalid NR cell identity value");
  }
  return *nci;
}

/// Parse ISO 8601 with optional milliseconds: "YYYY-MM-DDTHH:MM:SS[.mmm]"
static expected<int64_t, std::string> parse_timestamp_ms(const std::string& datetime)
{
  std::tm tm           = {};
  int     milliseconds = 0;

  size_t      dot_pos = datetime.find('.');
  std::string base    = datetime;

  if (dot_pos != std::string::npos) {
    base = datetime.substr(0, dot_pos);
  }

  std::istringstream ss(base);
  ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");
  if (ss.fail()) {
    return make_unexpected("Invalid datetime format (expected YYYY-MM-DDTHH:MM:SS[.mmm])");
  }

  if (dot_pos != std::string::npos) {
    std::string ms_str = datetime.substr(dot_pos + 1);
    if (ms_str.length() > 3) {
      ms_str = ms_str.substr(0, 3);
    }
    while (ms_str.length() < 3) {
      ms_str += '0';
    }
    milliseconds = std::stoi(ms_str);
  }

  time_t seconds = timegm(&tm);
  return static_cast<int64_t>(seconds * 1000 + milliseconds);
}

static expected<std::chrono::system_clock::time_point, std::string> parse_epoch_time(const nlohmann::json& json)
{
  auto epoch_field = json.find("epoch_timestamp");
  if (epoch_field == json.end()) {
    return make_unexpected("'epoch_timestamp' field is required");
  }

  // Parse epoch_timestamp as milliseconds (number or ISO string).
  int64_t epoch_ms;

  if (epoch_field->is_number()) {
    // Parse as milliseconds (Unix timestamp).
    auto epoch_ms_result = validate_json<uint64_t>(json, "epoch_timestamp");
    if (!epoch_ms_result) {
      return make_unexpected(epoch_ms_result.error());
    }
    epoch_ms = static_cast<int64_t>(epoch_ms_result->get<uint64_t>());
  } else if (epoch_field->is_string()) {
    // Parse as UTC time string (YYYY-MM-DDTHH:MM:SS[.mmm]).
    auto epoch_ms_result = parse_timestamp_ms(epoch_field->get<std::string>());
    if (!epoch_ms_result) {
      return make_unexpected(fmt::format("Failed to parse 'epoch_timestamp': {}", epoch_ms_result.error()));
    }
    epoch_ms = *epoch_ms_result;
  } else {
    return make_unexpected(
        "'epoch_timestamp' must be either a number (milliseconds) or a string (YYYY-MM-DDTHH:MM:SS[.mmm])");
  }

  // Convert milliseconds to time_point.
  auto epoch_time_point = std::chrono::system_clock::time_point(std::chrono::milliseconds(epoch_ms));

  // Validate that the timestamp is in the future.
  auto now_ts = std::chrono::system_clock::now();
  if (epoch_time_point < now_ts) {
    return make_unexpected("'epoch_timestamp' value is in past");
  }

  return epoch_time_point;
}

static expected<std::optional<ta_info_t>, std::string> parse_ta_info(const nlohmann::json& json)
{
  if (json.contains("ta_info")) {
    auto ta_info_result = validate_json<ta_info_t>(json, "ta_info");
    if (!ta_info_result) {
      return make_unexpected(ta_info_result.error());
    }
    return *ta_info_result;
  }
  return std::nullopt;
}

static expected<std::optional<feeder_link_info_t>, std::string> parse_feeder_link_info(const nlohmann::json& json)
{
  if (json.contains("feeder_link_info")) {
    auto feeder_link_result = validate_json<feeder_link_info_t>(json, "feeder_link_info");
    if (!feeder_link_result) {
      return make_unexpected(feeder_link_result.error());
    }
    return *feeder_link_result;
  }
  return std::nullopt;
}

static expected<std::optional<geodetic_coordinates_t>, std::string>
parse_ntn_gateway_location(const nlohmann::json& json)
{
  if (json.contains("ntn_gateway_location")) {
    auto gateway_location_result = validate_json<geodetic_coordinates_t>(json, "ntn_gateway_location");
    if (!gateway_location_result) {
      return make_unexpected(gateway_location_result.error());
    }
    return *gateway_location_result;
  }
  return std::nullopt;
}

static expected<ntn_polarization_t::polarization_type, std::string> parse_polarization_type(const std::string& s)
{
  if (s == to_string(ntn_polarization_t::polarization_type::rhcp)) {
    return ntn_polarization_t::polarization_type::rhcp;
  }
  if (s == to_string(ntn_polarization_t::polarization_type::lhcp)) {
    return ntn_polarization_t::polarization_type::lhcp;
  }
  if (s == to_string(ntn_polarization_t::polarization_type::linear)) {
    return ntn_polarization_t::polarization_type::linear;
  }
  return make_unexpected(fmt::format("Invalid polarization type '{}' (expected 'rhcp', 'lhcp', or 'linear')", s));
}

static expected<std::optional<ntn_polarization_t>, std::string> parse_polarization(const nlohmann::json& json)
{
  if (!json.contains("polarization")) {
    return std::nullopt;
  }
  const auto&        pol_json = json["polarization"];
  ntn_polarization_t pol;
  if (pol_json.contains("dl")) {
    auto dl_result = parse_polarization_type(pol_json["dl"].get<std::string>());
    if (!dl_result) {
      return make_unexpected(fmt::format("Invalid 'polarization.dl': {}", dl_result.error()));
    }
    pol.dl = *dl_result;
  }
  if (pol_json.contains("ul")) {
    auto ul_result = parse_polarization_type(pol_json["ul"].get<std::string>());
    if (!ul_result) {
      return make_unexpected(fmt::format("Invalid 'polarization.ul': {}", ul_result.error()));
    }
    pol.ul = *ul_result;
  }
  return pol;
}

static expected<std::optional<std::chrono::system_clock::time_point>, std::string>
parse_t_service(const nlohmann::json& json)
{
  if (!json.contains("t_service")) {
    return std::nullopt;
  }
  const auto& field = json["t_service"];
  int64_t     ms;
  if (field.is_number()) {
    ms = field.get<int64_t>();
  } else if (field.is_string()) {
    auto ms_result = parse_timestamp_ms(field.get<std::string>());
    if (!ms_result) {
      return make_unexpected(fmt::format("Failed to parse 't_service': {}", ms_result.error()));
    }
    ms = *ms_result;
  } else {
    return make_unexpected("'t_service' must be a number (milliseconds) or a string (YYYY-MM-DDTHH:MM:SS[.mmm])");
  }
  return std::chrono::system_clock::time_point(std::chrono::milliseconds(ms));
}

static expected<std::optional<std::vector<neighbor_ntn_cell>>, std::string> parse_ncells(const nlohmann::json& json)
{
  if (!json.contains("ncells")) {
    return std::nullopt;
  }

  const auto& ncells_json = json["ncells"];
  if (!ncells_json.is_array()) {
    return make_unexpected("'ncells' must be an array");
  }

  if (ncells_json.size() > MAX_NOF_NTN_NEIGHBORS) {
    return make_unexpected(
        fmt::format("'ncells' supports at most {} entries", static_cast<unsigned>(MAX_NOF_NTN_NEIGHBORS)));
  }

  std::vector<neighbor_ntn_cell> ncells;
  ncells.reserve(ncells_json.size());

  for (size_t i = 0; i != ncells_json.size(); ++i) {
    const auto& entry = ncells_json[i];
    if (!entry.is_object()) {
      return make_unexpected(fmt::format("'ncells[{}]' must be an object", i));
    }

    // Strict schema: only pci and carrier_freq are supported via WS.
    for (auto it = entry.begin(); it != entry.end(); ++it) {
      if (it.key() != "pci" && it.key() != "carrier_freq") {
        return make_unexpected(
            fmt::format("'ncells[{}].{}' is not supported; only 'pci' and 'carrier_freq' are allowed", i, it.key()));
      }
    }

    neighbor_ntn_cell neigh;
    if (entry.contains("pci")) {
      auto result = validate_json<unsigned>(entry, "pci");
      if (!result) {
        return make_unexpected(fmt::format("'ncells[{}].pci': {}", i, result.error()));
      }
      unsigned pci_val = result->get<unsigned>();
      if (pci_val > MAX_PCI) {
        return make_unexpected(fmt::format("'ncells[{}].pci' must be in range [0, {}]", i, MAX_PCI));
      }
      neigh.phys_cell_id = static_cast<pci_t>(pci_val);
    }

    if (entry.contains("carrier_freq")) {
      auto result = validate_json<unsigned>(entry, "carrier_freq");
      if (!result) {
        return make_unexpected(fmt::format("'ncells[{}].carrier_freq': {}", i, result.error()));
      }
      unsigned arfcn = result->get<unsigned>();
      if (arfcn > 3279165U) {
        return make_unexpected(fmt::format("'ncells[{}].carrier_freq' must be in range [0, 3279165]", i));
      }
      neigh.carrier_freq = arfcn_t{arfcn};
    }

    ncells.push_back(neigh);
  }

  return ncells;
}

static expected<ntn_ephemeris_info_t, std::string> parse_ephemeris_info(const nlohmann::json& json)
{
  auto ephemeris_info_it = json.find("ephemeris_info");
  if (ephemeris_info_it == json.end()) {
    return make_unexpected("'ephemeris_info' object is missing and it is mandatory");
  }
  if (ephemeris_info_it->contains("ecef")) {
    auto result = validate_json<ecef_coordinates_t>(*ephemeris_info_it, "ecef");
    if (!result) {
      return make_unexpected(result.error());
    }
    return result->get<ecef_coordinates_t>();
  }
  if (ephemeris_info_it->contains("orbital")) {
    auto result = validate_json<orbital_coordinates_t>(*ephemeris_info_it, "orbital");
    if (!result) {
      return make_unexpected(result.error());
    }
    return result->get<orbital_coordinates_t>();
  }
  return make_unexpected("Invalid ephemeris_info value");
}

static expected<ntn_config, std::string> parse_sat_switch_ntn_cfg(const nlohmann::json& json)
{
  ntn_config cfg;

  if (json.contains("cell_specific_koffset")) {
    auto result = validate_json<unsigned>(json, "cell_specific_koffset");
    if (!result) {
      return make_unexpected(result.error());
    }
    cfg.cell_specific_koffset = std::chrono::milliseconds(result->get<unsigned>());
  }

  if (json.contains("k_mac")) {
    auto result = validate_json<unsigned>(json, "k_mac");
    if (!result) {
      return make_unexpected(result.error());
    }
    cfg.k_mac = result->get<unsigned>();
  }

  if (json.contains("ntn_ul_sync_validity_dur")) {
    auto result = validate_json<unsigned>(json, "ntn_ul_sync_validity_dur");
    if (!result) {
      return make_unexpected(result.error());
    }
    cfg.ntn_ul_sync_validity_dur = result->get<unsigned>();
  }

  if (json.contains("ta_info")) {
    auto result = parse_ta_info(json);
    if (!result) {
      return make_unexpected(result.error());
    }
    cfg.ta_info = *result;
  }

  if (json.contains("ta_report")) {
    auto result = validate_json<bool>(json, "ta_report");
    if (!result) {
      return make_unexpected(result.error());
    }
    cfg.ta_report = result->get<bool>();
  }

  auto pol_result = parse_polarization(json);
  if (!pol_result) {
    return make_unexpected(pol_result.error());
  }
  cfg.polarization = *pol_result;

  if (json.contains("ephemeris_info")) {
    auto result = parse_ephemeris_info(json);
    if (!result) {
      return make_unexpected(result.error());
    }
    cfg.ephemeris_info = *result;
  }

  return cfg;
}

static expected<std::optional<sat_switch_with_resync_t>, std::string>
parse_sat_switch_with_resync(const nlohmann::json& json)
{
  if (!json.contains("sat_switch_with_resync")) {
    return std::nullopt;
  }

  const auto&              sat_json = json["sat_switch_with_resync"];
  sat_switch_with_resync_t sat;

  if (sat_json.contains("epoch_timestamp")) {
    const auto& field = sat_json["epoch_timestamp"];
    int64_t     ms;
    if (field.is_number()) {
      ms = field.get<int64_t>();
    } else if (field.is_string()) {
      auto ms_result = parse_timestamp_ms(field.get<std::string>());
      if (!ms_result) {
        return make_unexpected(
            fmt::format("Failed to parse 'sat_switch_with_resync.epoch_timestamp': {}", ms_result.error()));
      }
      ms = *ms_result;
    } else {
      return make_unexpected("'sat_switch_with_resync.epoch_timestamp' must be a number (ms) or ISO string");
    }
    sat.epoch_timestamp = std::chrono::system_clock::time_point(std::chrono::milliseconds(ms));
  }

  if (sat_json.contains("ntn_gateway_location")) {
    auto result = validate_json<geodetic_coordinates_t>(sat_json, "ntn_gateway_location");
    if (!result) {
      return make_unexpected(
          fmt::format("Failed to parse 'sat_switch_with_resync.ntn_gateway_location': {}", result.error()));
    }
    sat.ntn_gateway_location = *result;
  }

  if (sat_json.contains("t_service_start")) {
    const auto& field = sat_json["t_service_start"];
    int64_t     ms;
    if (field.is_number()) {
      ms = field.get<int64_t>();
    } else if (field.is_string()) {
      auto ms_result = parse_timestamp_ms(field.get<std::string>());
      if (!ms_result) {
        return make_unexpected(
            fmt::format("Failed to parse 'sat_switch_with_resync.t_service_start': {}", ms_result.error()));
      }
      ms = *ms_result;
    } else {
      return make_unexpected("'sat_switch_with_resync.t_service_start' must be a number (ms) or ISO string");
    }
    sat.t_service_start = std::chrono::system_clock::time_point(std::chrono::milliseconds(ms));
  }

  if (sat_json.contains("ssb_time_offset_sf")) {
    auto result = validate_json<unsigned>(sat_json, "ssb_time_offset_sf");
    if (!result) {
      return make_unexpected(result.error());
    }
    unsigned val = result->get<unsigned>();
    if (val > 159) {
      return make_unexpected("'sat_switch_with_resync.ssb_time_offset_sf' must be in range [0, 159]");
    }
    sat.ssb_time_offset_sf = sat_switch_with_resync_t::ssb_time_offset_t{static_cast<uint8_t>(val)};
  }

  if (!sat_json.contains("ntn_cfg")) {
    return make_unexpected("'sat_switch_with_resync.ntn_cfg' is required when sat_switch_with_resync is present");
  }

  auto ntn_cfg_result = parse_sat_switch_ntn_cfg(sat_json["ntn_cfg"]);
  if (!ntn_cfg_result) {
    return make_unexpected(fmt::format("Failed to parse 'sat_switch_with_resync.ntn_cfg': {}", ntn_cfg_result.error()));
  }
  sat.ntn_cfg = *ntn_cfg_result;

  return sat;
}

/// Unified function to parse NTN config from JSON.
/// If base_config is provided, fields are optional (cell overrides common config).
/// If base_config is not provided, required fields must be present (complete config per cell).
static expected<ntn_cell_config_update_info, std::string>
parse_cell_ntn_config(const nlohmann::json&                    json,
                      const plmn_identity&                     plmn,
                      const nr_cell_identity&                  nci,
                      const ntn_cell_config_update_info* const base_config = nullptr)
{
  ntn_cell_config_update_info cell_req;

  // If base_config is provided, start with it; otherwise start fresh.
  if (base_config) {
    cell_req = *base_config;
  }

  // Set cell identity.
  cell_req.nr_cgi.plmn_id = plmn;
  cell_req.nr_cgi.nci     = nci;

  // Parse epoch_time (required if no base_config, optional otherwise).
  if (json.contains("epoch_timestamp")) {
    auto epoch_result = parse_epoch_time(json);
    if (!epoch_result) {
      return make_unexpected(epoch_result.error());
    }
    cell_req.epoch_time = *epoch_result;
  } else if (!base_config) {
    return make_unexpected("'epoch_timestamp' is required when no common config is provided");
  }

  // Parse ntn_ul_sync_validity_duration (required if no base_config, optional otherwise).
  if (json.contains("ntn_ul_sync_validity_duration")) {
    auto ul_sync_duration_result = validate_json<int>(json, "ntn_ul_sync_validity_duration");
    if (!ul_sync_duration_result) {
      return make_unexpected(ul_sync_duration_result.error());
    }
    int duration_val = ul_sync_duration_result->get<int>();
    if (duration_val < 0) {
      return make_unexpected("ntn_ul_sync_validity_duration must be non-negative");
    }
    cell_req.ntn_ul_sync_validity_duration = static_cast<unsigned>(duration_val);
  } else if (!base_config) {
    return make_unexpected("'ntn_ul_sync_validity_duration' is required when no common config is provided");
  }

  // Parse ephemeris_info (required if no base_config, optional otherwise).
  if (json.contains("ephemeris_info")) {
    auto ephemeris_info_result = parse_ephemeris_info(json);
    if (!ephemeris_info_result) {
      return make_unexpected(ephemeris_info_result.error());
    }
    cell_req.ephemeris_info = *ephemeris_info_result;
  } else if (!base_config) {
    return make_unexpected("'ephemeris_info' is required when no common config is provided");
  }

  // Parse optional fields (always optional).
  if (json.contains("ta_info")) {
    auto ta_info_result = parse_ta_info(json);
    if (!ta_info_result) {
      return make_unexpected(ta_info_result.error());
    }
    if (ta_info_result->has_value()) {
      cell_req.ta_info = *ta_info_result;
    }
  }

  if (json.contains("feeder_link_info")) {
    auto feeder_link_result = parse_feeder_link_info(json);
    if (!feeder_link_result) {
      return make_unexpected(feeder_link_result.error());
    }
    if (feeder_link_result->has_value()) {
      cell_req.feeder_link_info = *feeder_link_result;
    }
  }

  if (json.contains("ntn_gateway_location")) {
    auto gateway_location_result = parse_ntn_gateway_location(json);
    if (!gateway_location_result) {
      return make_unexpected(gateway_location_result.error());
    }
    if (gateway_location_result->has_value()) {
      cell_req.ntn_gateway_location = *gateway_location_result;
    }
  }

  if (json.contains("reference_location")) {
    const auto& ref_loc_json = json["reference_location"];
    auto        lat_result   = validate_json<double>(ref_loc_json, "latitude");
    if (!lat_result) {
      return make_unexpected(fmt::format("'reference_location.latitude': {}", lat_result.error()));
    }
    auto lon_result = validate_json<double>(ref_loc_json, "longitude");
    if (!lon_result) {
      return make_unexpected(fmt::format("'reference_location.longitude': {}", lon_result.error()));
    }
    geodetic_coordinates_t ref_loc{};
    ref_loc.latitude            = lat_result->get<double>();
    ref_loc.longitude           = lon_result->get<double>();
    cell_req.reference_location = ref_loc;
  }

  if (json.contains("distance_threshold")) {
    auto result = validate_json<unsigned>(json, "distance_threshold");
    if (!result) {
      return make_unexpected(result.error());
    }
    cell_req.distance_threshold = result->get<unsigned>();
  }

  auto t_service_result = parse_t_service(json);
  if (!t_service_result) {
    return make_unexpected(t_service_result.error());
  }
  cell_req.t_service = *t_service_result;

  auto pol_result = parse_polarization(json);
  if (!pol_result) {
    return make_unexpected(pol_result.error());
  }
  cell_req.polarization = *pol_result;

  if (json.contains("ta_report")) {
    auto result = validate_json<bool>(json, "ta_report");
    if (!result) {
      return make_unexpected(result.error());
    }
    cell_req.ta_report = result->get<bool>();
  }

  auto ncells_result = parse_ncells(json);
  if (!ncells_result) {
    return make_unexpected(ncells_result.error());
  }
  cell_req.ncells = *ncells_result;

  auto sat_switch_result = parse_sat_switch_with_resync(json);
  if (!sat_switch_result) {
    return make_unexpected(sat_switch_result.error());
  }
  cell_req.sat_switch_with_resync = *sat_switch_result;

  if (json.contains("moving_ref_location")) {
    const auto& mov_json   = json["moving_ref_location"];
    auto        lat_result = validate_json<double>(mov_json, "latitude");
    if (!lat_result) {
      return make_unexpected(fmt::format("'moving_ref_location.latitude': {}", lat_result.error()));
    }
    auto lon_result = validate_json<double>(mov_json, "longitude");
    if (!lon_result) {
      return make_unexpected(fmt::format("'moving_ref_location.longitude': {}", lon_result.error()));
    }
    geodetic_coordinates_t mov_loc{};
    mov_loc.latitude             = lat_result->get<double>();
    mov_loc.longitude            = lon_result->get<double>();
    cell_req.moving_ref_location = mov_loc;
  }

  return cell_req;
}

error_type<std::string> ntn_config_update_remote_command::execute(const nlohmann::json& json)
{
  // Validate cells array presence.
  auto cells_it = json.find("cells");
  if (cells_it == json.end()) {
    return make_unexpected("'cells' field is required");
  }

  if (!cells_it->is_array()) {
    return make_unexpected("'cells' must be an array");
  }

  if (cells_it->empty()) {
    return make_unexpected("'cells' array is empty");
  }

  // Check if common config is provided.
  std::optional<ntn_cell_config_update_info> common_config;
  auto                                       common_it = json.find("common_ntn_config");
  if (common_it != json.end()) {
    // Parse common config using unified parser with dummy plmn/nci (will be overridden per cell).
    plmn_identity    dummy_plmn    = plmn_identity::test_value();
    nr_cell_identity dummy_nci     = nr_cell_identity::max();
    auto             common_result = parse_cell_ntn_config(*common_it, dummy_plmn, dummy_nci, nullptr);
    if (!common_result) {
      return make_unexpected(fmt::format("Failed to parse common_ntn_config: {}", common_result.error()));
    }
    common_config = *common_result;
  }

  // Build request with cell configs.
  // If any cell fails parsing, reject the entire request.
  ntn_config_update_info req;
  req.cells.reserve(cells_it->size());

  for (const auto& cell_json : *cells_it) {
    // Parse cell identity.
    auto plmn_result = parse_plmn(cell_json);
    if (!plmn_result) {
      return make_unexpected(fmt::format("Failed to parse cell identity: {}", plmn_result.error()));
    }

    auto nci_result = parse_nci(cell_json);
    if (!nci_result) {
      return make_unexpected(fmt::format("Failed to parse cell NCI: {}", nci_result.error()));
    }

    // Parse cell config: if common_config exists, use it as base (overrides optional);
    // otherwise, parse complete config (required fields must be present).
    const ntn_cell_config_update_info* base_config_ptr = common_config.has_value() ? &(*common_config) : nullptr;
    auto cell_req_result = parse_cell_ntn_config(cell_json, *plmn_result, *nci_result, base_config_ptr);
    if (!cell_req_result) {
      return make_unexpected(
          fmt::format("Failed to parse config for cell NCI={}: {}", nci_result->value(), cell_req_result.error()));
    }

    req.cells.push_back(*cell_req_result);
  }

  // Send multi-cell update.
  auto result = ntn_cfgr.handle_ntn_config_update(req);

  if (!result.failed.empty()) {
    // Format failed cells list with PLMN and NCI information.
    std::vector<std::string> failed_cells_str;
    failed_cells_str.reserve(result.failed.size());

    for (const auto& cell : result.failed) {
      failed_cells_str.push_back(fmt::format("PLMN={}, NCI={}", cell.plmn_id, cell.nci));
    }

    return make_unexpected(fmt::format("NTN update command procedure failed for {} cell(s): [{}]",
                                       result.failed.size(),
                                       fmt::join(failed_cells_str, ", ")));
  }

  return {};
}
