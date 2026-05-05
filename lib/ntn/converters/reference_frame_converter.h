// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../coordinates_types.h"
#include <chrono>
#include <optional>

namespace ocudu {
namespace ocudu_ntn {

/// Converter between ECEF and ECI reference frames.
/// Provides methods to convert state vectors (position and velocity) between
/// Earth-Centered Earth-Fixed (ECEF) and Earth-Centered Inertial (ECI) reference frames.
class reference_frame_converter
{
public:
  /// Construct the converter with a reference epoch time.
  explicit reference_frame_converter(std::chrono::system_clock::time_point epoch_time);

  /// \brief Convert ECEF state vector to ECI frame.
  ///
  /// \param ecef_state State vector in ECEF frame (position [m], velocity [m/s])
  /// \param current_time Current time point for the conversion (if not provided, uses epoch_time)
  /// \return State vector in ECI frame
  state_vector ecef_to_eci(const state_vector&                                  ecef_state,
                           std::optional<std::chrono::system_clock::time_point> current_time = std::nullopt) const;

  /// \brief Convert ECI state vector to ECEF frame.
  ///
  /// \param eci_state State vector in ECI frame (position [m], velocity [m/s])
  /// \param current_time Current time point for the conversion (if not provided, uses epoch_time)
  /// \return State vector in ECEF frame
  state_vector eci_to_ecef(const state_vector&                                  eci_state,
                           std::optional<std::chrono::system_clock::time_point> current_time = std::nullopt) const;

private:
  /// Reference epoch when ECEF and ECI axes are aligned.
  std::chrono::system_clock::time_point epoch_time;
};

} // namespace ocudu_ntn
} // namespace ocudu
