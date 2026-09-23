// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/adt/expected.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/ofh/compression/compression_params.h"
#include "ocudu/ran/antenna_topology.h"
#include "ocudu/ran/bs_channel_bandwidth.h"
#include "ocudu/ran/subcarrier_spacing.h"
#include "ocudu/support/units.h"
#include <optional>
#include <string>
#include <vector>

namespace ocudu {
namespace ofh {
namespace test {

/// Logging configuration.
struct logger_config {
  /// Path to log file or "stdout" to print to console.
  std::string filename = "stdout";
  /// Default log level for all layers.
  ocudulog::basic_levels level = ocudulog::basic_levels::warning;
};

/// Downlink beamforming (Category B O-RU) configuration.
struct beamforming_config {
  /// Enables downlink beamforming. The antenna topology is derived from the number of downlink ports.
  bool enable = false;
  /// Beamforming weights compression parameters.
  ru_compression_params bfw_compr_params = {compression_type::none, 16};
};

/// User-defined test parameters.
struct test_parameters {
  bool                  silent                              = false;
  logger_config         logger_cfg                          = {};
  bool                  is_prach_control_plane_enabled      = true;
  bool                  ignore_ecpri_payload_size_field     = false;
  ru_compression_params data_compr_params                   = {compression_type::BFP, 9};
  ru_compression_params prach_compr_params                  = {compression_type::BFP, 9};
  bool                  is_downlink_static_comp_hdr_enabled = false;
  bool                  is_uplink_static_comp_hdr_enabled   = false;
  bool                  is_downlink_parallelized            = true;
  units::bytes          mtu                                 = units::bytes(9000);
  std::vector<unsigned> prach_port_id                       = {4, 5};
  std::vector<unsigned> dl_port_id                          = {0, 1, 2, 3};
  std::vector<unsigned> ul_port_id                          = {0, 1};
  bs_channel_bandwidth  channel_bw_mhz                      = bs_channel_bandwidth::MHz20;
  subcarrier_spacing    scs                                 = subcarrier_spacing::kHz30;
  std::string           tdd_pattern_str                     = "7d2u";
  bool                  use_loopback_receiver               = false;
  unsigned              nof_test_slots                      = 1000;
  beamforming_config    beamforming_cfg                     = {};
};

/// \brief Gets the downlink antenna topology given the number of downlink ports.
///
/// \return The antenna topology, or \c std::nullopt if the number of ports does not map to a supported topology.
std::optional<antenna_topology> get_dl_antenna_topology(unsigned nof_dl_ports);

/// \brief Parses OFH integration test arguments.
///
/// \return Parsed test configuration if the parsing is successful, otherwise the exit code from CLI11.
expected<test_parameters, int> parse_test_configuration(int argc, char** argv);

} // namespace test
} // namespace ofh
} // namespace ocudu
