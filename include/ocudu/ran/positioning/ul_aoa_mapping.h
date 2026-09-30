// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief UL Angle of Arrival report mapping, as per TS 38.133, Tables 13.4.1-1 and 13.4.1-2.

#pragma once

#include "ocudu/support/ocudu_assert.h"
#include <algorithm>
#include <cmath>
#include <cstdint>

namespace ocudu {

/// Maximum azimuth Angle of Arrival reported value.
constexpr uint16_t MAX_AZIMUTH_AOA_REPORTED_VALUE = 3599;
/// Maximum zenith Angle of Arrival reported value.
constexpr uint16_t MAX_ZENITH_AOA_REPORTED_VALUE = 1799;

/// \brief Maps an azimuth Angle of Arrival in degrees to its reported value, as per TS 38.133, Table 13.4.1-1.
///
/// The reported value n = {0,...,3599} covers the angles [-180 + 0.1n, -180 + 0.1(n + 1)) degrees.
/// \param[in] degrees Azimuth Angle of Arrival, in degrees. Values: [-180, 180).
inline uint16_t azimuth_aoa_to_reported_value(float degrees)
{
  ocudu_assert(
      degrees >= -180.0F && degrees < 180.0F, "Azimuth Angle of Arrival {} degrees is outside [-180, 180).", degrees);

  // Single precision rounds the offset of angles just below 180 degrees up to 360 degrees, out of the last interval.
  const double value = std::floor((static_cast<double>(degrees) + 180.0) * 10.0);
  // Clamping keeps the reported value valid when asserts are disabled.
  return static_cast<uint16_t>(std::clamp(value, 0.0, static_cast<double>(MAX_AZIMUTH_AOA_REPORTED_VALUE)));
}

/// \brief Maps a zenith Angle of Arrival in degrees to its reported value, as per TS 38.133, Table 13.4.1-2.
///
/// The reported value n = {0,...,1799} covers the angles [0.1n, 0.1(n + 1)) degrees.
/// \param[in] degrees Zenith Angle of Arrival, in degrees. Values: [0, 180).
inline uint16_t zenith_aoa_to_reported_value(float degrees)
{
  ocudu_assert(degrees >= 0.0F && degrees < 180.0F, "Zenith Angle of Arrival {} degrees is outside [0, 180).", degrees);

  const double value = std::floor(static_cast<double>(degrees) * 10.0);
  // Clamping keeps the reported value valid when asserts are disabled.
  return static_cast<uint16_t>(std::clamp(value, 0.0, static_cast<double>(MAX_ZENITH_AOA_REPORTED_VALUE)));
}

} // namespace ocudu
