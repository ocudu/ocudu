// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

namespace ocudu {

/// \brief Configuration parameters for the DOA estimator.
///
/// The configuration assumes the antennas are arranged according to the Uniform Linear Array (ULA) geometry.
struct doa_estimator_configuration {
  /// \brief Number of antenna ports.
  ///
  /// This is the total number of sensing elements. Specifically, when using arrays with cross-polarized antennas (see
  /// below), both polarizations contribute to this number.
  unsigned nof_antennas;
  /// Distance between two consecutive antenna elements, normalized with respect to the wavelength.
  float antenna_distance_over_wavelength;
  /// \brief Cross-polarization flag.
  ///
  /// If true, the antenna array is assumed to consist of pairs (two consecutive elements) of collocated antennas with
  /// orthogonal polarizations. If false, antennas are not collocated and all have the same polarization.
  bool cross_polarized;
};

} // namespace ocudu
