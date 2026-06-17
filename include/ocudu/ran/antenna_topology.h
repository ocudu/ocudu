// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include <cstdint>

namespace ocudu {

/// \brief Supported antenna topologies.
///
/// An antenna topology is defined by a number of independent panels. Each panel that contain a matrix of
/// \f$(N_1, N_2)\f$ antennas with one or two polarizations.
///
/// The values for each enumeration are grouped in 4 nibbles (groups of 4 bits), from MSB to LSB:
/// - Number of panels minus one;
/// - Number of elements on the first dimension minus one;
/// - Number of elements on the second dimension minus one; and
/// - Number of polarizations minus one.
enum class antenna_topology : uint16_t {
  /// Single antenna port with a single polarization.
  one_port = 0x0000,
  /// Single antenna port with two polarizations.
  two_port = 0x0001,
  /// Four independent antenna ports with a single polarization.
  four_ports = 0x3000,
  /// Eight independent antenna ports with a single polarization.
  eight_ports = 0x7000,
  /// Single-panel of 2x1 antenna ports with two polarizations.
  single_panel_two_one = 0x0101,
  /// Single-panel of 2x2 antenna ports with two polarizations.
  single_panel_two_two = 0x0111,
  /// Single-panel of 4x1 antenna ports with two polarizations.
  single_panel_four_one = 0x0301,
};

/// Gets the antenna topology number of panels.
constexpr unsigned get_nof_antenna_panels(antenna_topology topology)
{
  return (static_cast<uint16_t>(topology) >> 12) + 1;
}

/// Gets the antenna topology number of elements in the first dimension.
constexpr unsigned get_nof_antenna_elements_dim1(antenna_topology topology)
{
  return (static_cast<uint16_t>(topology) >> 8 & 0xf) + 1;
}

/// Gets the antenna topology number of beams for the first dimension.
constexpr unsigned get_nof_beams_dim1(antenna_topology topology)
{
  // Get number of elements for this dimension.
  unsigned nof_elements_dim = get_nof_antenna_elements_dim1(topology);

  // Select oversampling factor.
  unsigned oversampling_dim = (nof_elements_dim > 1) ? 4 : 1;

  return nof_elements_dim * oversampling_dim;
}

/// Gets the antenna topology number of elements in the second dimension.
constexpr unsigned get_nof_antenna_elements_dim2(antenna_topology topology)
{
  return (static_cast<uint16_t>(topology) >> 4 & 0xf) + 1;
}

/// Gets the antenna topology number of beams for the first dimension.
constexpr unsigned get_nof_beams_dim2(antenna_topology topology)
{
  // Get number of elements for this dimension.
  unsigned nof_elements_dim = get_nof_antenna_elements_dim2(topology);

  // Select oversampling factor.
  unsigned oversampling_dim = (nof_elements_dim > 1) ? 4 : 1;

  return nof_elements_dim * oversampling_dim;
}

/// Gets the antenna topology number of polarizations.
constexpr unsigned get_nof_antenna_polarizations(antenna_topology topology)
{
  return (static_cast<uint16_t>(topology) & 0xf) + 1;
}

/// Gets the antenna topology total number of antenna ports.
constexpr unsigned get_total_nof_ports(antenna_topology topology)
{
  unsigned nof_panels        = get_nof_antenna_panels(topology);
  unsigned nof_elements_dim1 = get_nof_antenna_elements_dim1(topology);
  unsigned nof_elements_dim2 = get_nof_antenna_elements_dim2(topology);
  unsigned nof_polarizations = get_nof_antenna_polarizations(topology);

  return nof_panels * nof_elements_dim1 * nof_elements_dim2 * nof_polarizations;
}

/// Gets the antenna topology total number of beams.
constexpr unsigned get_total_nof_beams(antenna_topology topology)
{
  unsigned nof_panels        = get_nof_antenna_panels(topology);
  unsigned nof_beams_dim1    = get_nof_beams_dim1(topology);
  unsigned nof_beams_dim2    = get_nof_beams_dim2(topology);
  unsigned nof_polarizations = get_nof_antenna_polarizations(topology);

  return get_total_nof_ports(topology) + nof_panels * nof_beams_dim1 * nof_beams_dim2 * nof_polarizations;
}

/// Convert the antenna topology to a constant string.
inline const char* to_string(antenna_topology topology)
{
  switch (topology) {
    case antenna_topology::one_port:
      return "one-port";
    case antenna_topology::two_port:
      return "two-ports";
    case antenna_topology::single_panel_two_one:
      return "single-panel/two-one";
    case antenna_topology::single_panel_two_two:
      return "single-panel/two-two";
    case antenna_topology::single_panel_four_one:
      return "single-panel/four-one";
    case antenna_topology::four_ports:
      return "4-ports";
    case antenna_topology::eight_ports:
      return "8-ports";
  }
  return "undefined";
}

} // namespace ocudu
