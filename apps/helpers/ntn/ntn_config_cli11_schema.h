// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/ntn.h"
#include "CLI/CLI11.hpp"
#include <chrono>
#include <optional>
#include <string>
#include <vector>

namespace ocudu {

struct ntn_satellite_config;

/// Adds a timestamp option accepting Unix time in ms or an ISO 8601 string (YYYY-MM-DDTHH:MM:SS[.mmm]).
CLI::Option* add_ntn_timestamp_option(CLI::App&                                             app,
                                      const std::string&                                    name,
                                      std::optional<std::chrono::system_clock::time_point>& dest,
                                      const std::string&                                    desc);

/// Configures geodetic coordinates (latitude/longitude and optionally altitude) CLI11 options.
void configure_cli11_geodetic_coordinates(CLI::App& app, geodetic_coordinates_t& location, bool with_altitude = true);

/// Configures the CLI11 options of a satellite reference or definition: satellite_idx and/or inline orbital state
/// (epoch_timestamp, ephemeris subcommands, gateway_location, ta_info, propagator_type).
/// \param app The CLI11 application or subcommand to configure.
/// \param sat Satellite configuration to populate.
void configure_cli11_ntn_satellite_args(CLI::App& app, ntn_satellite_config& sat);

/// Configures the global NTN satellites section CLI11 options (ntn.satellites list).
/// \param app The CLI11 application or subcommand to configure.
/// \param ntn_satellites Satellite list to populate.
void configure_cli11_ntn_satellites_args(CLI::App& app, std::vector<ntn_satellite_config>& ntn_satellites);

/// Configures the NTN service link polarization CLI11 options (--dl/--ul, each lhcp/rhcp/linear).
/// \param app The CLI11 application or subcommand to configure.
/// \param polarization Polarization configuration to populate.
void configure_cli11_ntn_polarization(CLI::App& app, ntn_polarization_t& polarization);

} // namespace ocudu
