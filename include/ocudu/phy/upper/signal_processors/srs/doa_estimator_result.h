// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief Direction of Arrival (DOA) estimator result definition.

#pragma once

#include "ocudu/adt/static_vector.h"
#include "ocudu/phy/antenna_ports.h"

namespace ocudu {

/// \brief Direction of Arrival (DOA) estimator results.
///
/// For all the detected signal components, it reports the estimated broadside angle and the spectrum strength.
struct doa_estimator_result {
  struct doa_component_type {
    /// \brief Broadside angle of arrival, in degrees.
    ///
    /// Briefly, the angle with respect to the perpendicular to the linear array. For a more precise definition see,
    /// e.g., [here](https://www.mathworks.com/help/phased/ug/spherical-coordinates.html#bsl6_dn).
    float broadside_angle_degrees;
    /// \brief Spectrum strength (a qualitative metric, no units).
    ///
    /// This value can be thought as a detection metric: the higher, the stronger is the signal component in the
    /// direction of broadside_angle_degrees.
    float spectrum_strength;
  };

  /// Constructor: defines the number of reported components.
  explicit doa_estimator_result(unsigned nof_components) : doa_components(nof_components) {}

  /// \brief List of detected DOA components.
  ///
  /// The components are sorted by decreasing spectrum strength. The number of detected components cannot exceed the
  /// number of Rx antenna ports.
  static_vector<doa_component_type, MAX_PORTS> doa_components;
};

} // namespace ocudu
