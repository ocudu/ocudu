// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/support/time_alignment_estimator/time_alignment_measurement.h"
#include "ocudu/phy/upper/signal_processors/srs/doa_estimator_result.h"
#include "ocudu/ran/srs/srs_channel_matrix.h"
#include <optional>

namespace ocudu {

/// Sounding Reference Signals propagation channel estimation results.
struct srs_estimator_result {
  /// \brief Wideband estimated channel matrix.
  ///
  /// The channel matrix is normalized by the noise variance, so that its Frobenius norm square gives a rough estimate
  /// of the SNR for the sounding reference signals.
  /// \note The SNR inferred from the channel matrix is only meant as a qualitative indicator of the SRS reception and
  /// does not provide meaningful information about the achievable throughput.
  srs_channel_matrix channel_matrix;
  /// Wideband energy per resource element (EPRE), in decibel.
  std::optional<float> epre_dB;
  /// Wideband reference signal received power (RSRP), in decibel.
  std::optional<float> rsrp_dB;
  /// Wideband measured noise variance as a linear quantity.
  std::optional<float> noise_variance;
  /// Measured time alignment.
  time_alignment_measurement time_alignment;
  /// Estimated direction of arrival (DOA) components.
  std::optional<doa_estimator_result> doa_result;
};

} // namespace ocudu
