// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

namespace ocudu {

/// Collects the time alignment measurement metrics.
struct time_alignment_measurement {
  /// Actual time alignment measurement in seconds.
  double time_alignment;
  /// Time measurement resolution in seconds.
  double resolution;
  /// Minimum measurement.
  double min;
  /// Maximum measurement.
  double max;
};

} // namespace ocudu
