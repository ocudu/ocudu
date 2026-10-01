// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief List of thresholds for the PRACH detector.

#pragma once

#include "ocudu/ran/prach/prach_format_type.h"
#include "ocudu/ran/prach/prach_subcarrier_spacing.h"
#include <tuple>

namespace ocudu::detail {

/// Flag applied to each threshold entry.
enum class threshold_flag {
  /// Threshold entry not suitable for proper PRACH detection.
  red,
  /// Threshold entry with suboptimal PRACH detection performance.
  orange,
  /// Threshold entry meeting PRACH detection requirements.
  green
};

/// Parameters that affect the detection threshold value.
struct threshold_params {
  /// Number of receive antenna ports.
  unsigned nof_rx_ports = 0;
  /// Subcarrier spacing.
  prach_subcarrier_spacing scs = prach_subcarrier_spacing::invalid;
  /// PRACH format type.
  prach_format_type format = prach_format_type::invalid;
  /// Zero correlation zone.
  unsigned zero_correlation_zone = 0;
};

/// Pairs the PRACH detector threshold, combine-symbol flag and margin.
using threshold_and_margin_type = std::tuple<float, bool, unsigned>;

/// Checks the quality flag of the threshold for the given configurations.
threshold_flag get_threshold_flag(const threshold_params& params);

/// Retrieves the (threshold, combine-symbol flag, margin) tuple corresponding to the given configuration.
threshold_and_margin_type get_threshold_and_margin(const threshold_params& params);

} // namespace ocudu::detail
