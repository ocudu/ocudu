// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

namespace ocudu {

/// Port channel estimator frequency domain smoothing strategy.
enum class port_channel_estimator_fd_smoothing_strategy {
  /// No smoothing strategy.
  none = 0,
  /// Averages all frequency domain estimates.
  mean,
  /// Filters in the frequency domain with a low pass filter.
  filter,
};

/// Port channel estimator time domain interpolation strategy.
enum class port_channel_estimator_td_interpolation_strategy {
  /// Performs linear interpolation between the symbols containing DM-RS.
  interpolate = 0,
  /// Averages all time domain estimates.
  average
};

} // namespace ocudu
