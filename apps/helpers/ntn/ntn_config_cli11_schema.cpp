// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ntn_config_cli11_schema.h"
#include "ntn_satellite_config.h"
#include "ocudu/support/cli11_utils.h"
#include "ocudu/support/config_parsers.h"
#include "ocudu/support/string_parsing_utils.h"
#include <regex>

using namespace ocudu;

static bool is_number(const std::string& s)
{
  return !s.empty() && std::all_of(s.begin(), s.end(), ::isdigit);
}

static bool is_valid_timestamp(const std::string& input)
{
  static const std::regex timestamp_regex(R"(^\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}(\.\d{1,3})?$)",
                                          std::regex::ECMAScript);

  return std::regex_match(input, timestamp_regex);
}

/// Parse ISO 8601 with optional milliseconds: "YYYY-MM-DDTHH:MM:SS[.mmm]" and return UTC timepoint.
static expected<std::chrono::system_clock::time_point, std::string> parse_timestamp_ms(const std::string& datetime)
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
  auto   tp      = std::chrono::system_clock::from_time_t(seconds);
  tp += std::chrono::milliseconds(milliseconds);
  return tp;
}

CLI::Option* ocudu::add_ntn_timestamp_option(CLI::App&                                             app,
                                             const std::string&                                    name,
                                             std::optional<std::chrono::system_clock::time_point>& dest,
                                             const std::string&                                    desc)
{
  return app
      .add_option_function<std::string>(
          name,
          [&dest, name](const std::string& value) {
            if (is_number(value)) {
              const auto ms_since_epoch = parse_int<int64_t>(value);
              report_fatal_error_if_not(ms_since_epoch.has_value(),
                                        fmt::format("Invalid {} value '{}'", name, value).c_str());
              dest = std::chrono::system_clock::time_point(std::chrono::milliseconds(*ms_since_epoch));
            } else {
              dest = parse_timestamp_ms(value).value();
            }
          },
          desc)
      ->check([](const std::string& input) -> std::string {
        return (!is_number(input) && !is_valid_timestamp(input)) ? "Invalid timestamp format" : std::string{};
      });
}

void ocudu::configure_cli11_geodetic_coordinates(CLI::App& app, geodetic_coordinates_t& location, bool with_altitude)
{
  add_option(app, "--latitude", location.latitude, "Latitude [degree]")
      ->capture_default_str()
      ->check(CLI::Range(-90.0, 90.0));
  add_option(app, "--longitude", location.longitude, "Longitude [degree]")
      ->capture_default_str()
      ->check(CLI::Range(-180.0, 180.0));
  if (with_altitude) {
    add_option(app, "--altitude", location.altitude, "Altitude [m]")
        ->capture_default_str()
        ->check(CLI::Range(-1000.0, 20000.0));
  }
}

static void configure_cli11_ta_info(CLI::App& app, ta_info_t& ta_info)
{
  add_option(app, "--ta_common", ta_info.ta_common, "TA common")
      ->capture_default_str()
      ->check(CLI::Range(0.0, 270730.0));
  add_option(app, "--ta_common_drift", ta_info.ta_common_drift, "Drift rate of the common TA")
      ->capture_default_str()
      ->check(CLI::Range(-51.4606, 51.4606));
  add_option(app, "--ta_common_drift_variant", ta_info.ta_common_drift_variant, "Drift rate variation of the common TA")
      ->capture_default_str()
      ->check(CLI::Range(0.0, 0.57898));
  add_option(app, "--ta_common_offset", ta_info.ta_common_offset, "Constant offset added to TA common")
      ->capture_default_str()
      ->check(CLI::Range(0.0, 10000.0));
}

static void configure_cli11_ephemeris_info_ecef(CLI::App& app, ecef_coordinates_t& ephemeris_info)
{
  add_option(app, "--pos_x", ephemeris_info.position_x, "X Position of the satellite [m]")
      ->required()
      ->check(CLI::Range(-43620761.6, 43620759.3));
  add_option(app, "--pos_y", ephemeris_info.position_y, "Y Position of the satellite [m]")
      ->required()
      ->check(CLI::Range(-43620761.6, 43620759.3));
  add_option(app, "--pos_z", ephemeris_info.position_z, "Z Position of the satellite [m]")
      ->required()
      ->check(CLI::Range(-43620761.6, 43620759.3));
  add_option(app, "--vel_x", ephemeris_info.velocity_vx, "X Velocity of the satellite [m/s]")
      ->required()
      ->check(CLI::Range(-7864.32, 7864.26));
  add_option(app, "--vel_y", ephemeris_info.velocity_vy, "Y Velocity of the satellite [m/s]")
      ->required()
      ->check(CLI::Range(-7864.32, 7864.26));
  add_option(app, "--vel_z", ephemeris_info.velocity_vz, "Z Velocity of the satellite [m/s]")
      ->required()
      ->check(CLI::Range(-7864.32, 7864.26));
}

static void configure_cli11_ephemeris_info_orbital(CLI::App& app, orbital_coordinates_t& ephemeris_info)
{
  add_option(app, "--semi_major_axis", ephemeris_info.semi_major_axis, "Semi-major axis of the satellite [m]")
      ->required()
      ->check(CLI::Range(6500000.0, 42998632.07));
  add_option(app, "--eccentricity", ephemeris_info.eccentricity, "Eccentricity of the satellite [-]")
      ->required()
      ->check(CLI::Range(0.0, 0.01500510825));
  add_option(app, "--periapsis", ephemeris_info.periapsis, "Periapsis of the satellite [rad]")
      ->required()
      ->check(CLI::Range(0.0, 6.28407400155));
  add_option(app, "--longitude", ephemeris_info.longitude, "Longitude of the satellites angle of ascending node [rad]")
      ->required()
      ->check(CLI::Range(0.0, 6.28407400155));
  add_option(app, "--inclination", ephemeris_info.inclination, "Inclination of the satellite [rad]")
      ->required()
      ->check(CLI::Range(-1.57101850624, 1.57101848283));
  add_option(app, "--mean_anomaly", ephemeris_info.mean_anomaly, "Mean anomaly of the satellite [rad]")
      ->required()
      ->check(CLI::Range(0.0, 6.28407400155));
}

static void add_ephemeris_subcommands(CLI::App& app, std::optional<ntn_ephemeris_info_t>& dest)
{
  static ecef_coordinates_t ecef_coords;
  CLI::App*                 ecef_subcmd = add_subcommand(app, "ephemeris_info_ecef", "ECEF ephemeris");
  configure_cli11_ephemeris_info_ecef(*ecef_subcmd, ecef_coords);
  ecef_subcmd->parse_complete_callback([&dest]() { dest = ecef_coords; });

  static orbital_coordinates_t orbital_coords;
  CLI::App*                    orb_subcmd = add_subcommand(app, "ephemeris_orbital", "Orbital ephemeris");
  configure_cli11_ephemeris_info_orbital(*orb_subcmd, orbital_coords);
  orb_subcmd->parse_complete_callback([&dest]() { dest = orbital_coords; });
}

void ocudu::configure_cli11_ntn_satellite_args(CLI::App& app, ntn_satellite_config& sat)
{
  app.add_option("--satellite_idx",
                 sat.satellite_idx,
                 "Satellite index. Required when defining a satellite object; optional when referencing or "
                 "inline-defining a satellite, in which case it is mutually exclusive with epoch_timestamp, "
                 "ephemeris_info, gateway_location and ta_info.");

  add_ntn_timestamp_option(app,
                           "--epoch_timestamp",
                           sat.epoch_timestamp,
                           "Epoch timestamp (Unix ms or ISO 8601: YYYY-MM-DDTHH:MM:SS[.mmm])");

  app.add_option_function<std::string>(
         "--propagator_type",
         [&sat](const std::string& value) {
           sat.propagator_type = (value == "keplerian") ? ocudu_ntn::orbit_propagator_type::keplerian
                                                        : ocudu_ntn::orbit_propagator_type::rk4;
         },
         "Orbit propagator: rk4 or keplerian")
      ->check(CLI::IsMember({"rk4", "keplerian"}));

  add_ephemeris_subcommands(app, sat.ephemeris_info);

  static geodetic_coordinates_t gateway_loc;
  CLI::App*                     gw_subcmd = add_subcommand(app, "gateway_location", "NTN gateway geodetic coordinates");
  configure_cli11_geodetic_coordinates(*gw_subcmd, gateway_loc);
  gw_subcmd->parse_complete_callback([&sat]() { sat.gateway_location = gateway_loc; });

  static ta_info_t ta_info_val;
  CLI::App* ta_subcmd = add_subcommand(app, "ta_info", "TA info override (mutually exclusive with gateway_location)");
  configure_cli11_ta_info(*ta_subcmd, ta_info_val);
  ta_subcmd->parse_complete_callback([&sat]() { sat.ta_info = ta_info_val; });
}

void ocudu::configure_cli11_ntn_satellites_args(CLI::App& app, std::vector<ntn_satellite_config>& ntn_satellites)
{
  add_option_cell(
      app,
      "--satellites",
      [&ntn_satellites](const std::vector<std::string>& values) {
        ntn_satellites.resize(values.size());
        for (unsigned i = 0, e = values.size(); i != e; ++i) {
          CLI::App subapp("NTN satellite", "satellite #" + std::to_string(i));
          subapp.config_formatter(create_yaml_config_parser());
          subapp.allow_config_extras(CLI::config_extras_mode::capture);
          configure_cli11_ntn_satellite_args(subapp, ntn_satellites[i]);
          std::istringstream ss(values[i]);
          subapp.parse_from_stream(ss);
        }
      },
      "Globally-defined NTN satellites referenced by satellite_idx in cell ntn configs");
}

void ocudu::configure_cli11_ntn_polarization(CLI::App& app, ntn_polarization_t& polarization)
{
  add_option_function<std::string>(
      app,
      "--dl",
      [&polarization](const std::string& value) {
        if (value == to_string(ntn_polarization_t::polarization_type::lhcp)) {
          polarization.dl = ntn_polarization_t::polarization_type::lhcp;
        } else if (value == to_string(ntn_polarization_t::polarization_type::rhcp)) {
          polarization.dl = ntn_polarization_t::polarization_type::rhcp;
        } else {
          polarization.dl = ntn_polarization_t::polarization_type::linear;
        }
      },
      "Polarization information for downlink transmission on service link")
      ->check(CLI::IsMember({"lhcp", "rhcp", "linear"}, CLI::ignore_case));

  add_option_function<std::string>(
      app,
      "--ul",
      [&polarization](const std::string& value) {
        if (value == to_string(ntn_polarization_t::polarization_type::lhcp)) {
          polarization.ul = ntn_polarization_t::polarization_type::lhcp;
        } else if (value == to_string(ntn_polarization_t::polarization_type::rhcp)) {
          polarization.ul = ntn_polarization_t::polarization_type::rhcp;
        } else {
          polarization.ul = ntn_polarization_t::polarization_type::linear;
        }
      },
      "Polarization information for uplink transmission on service link")
      ->check(CLI::IsMember({"lhcp", "rhcp", "linear"}, CLI::ignore_case));
}
