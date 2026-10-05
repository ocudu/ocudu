// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief Amplitude controller generic interface.

#pragma once

#include "ocudu/adt/complex.h"
#include "ocudu/adt/span.h"

namespace ocudu {

/// Power measurements and runtime metrics reported by the amplitude controller.
struct amplitude_controller_metrics {
  /// Average power after input gain and before clipping, normalized to full scale.
  float avg_power_fs;
  /// Peak power after input gain and before clipping, normalized to full scale.
  float peak_power_fs;
  /// PAPR of the input signal.
  float papr_lin;
  /// Gain factor applied to the signal by the amplitude control after the measurements.
  float gain_dB;
  /// Total number of samples processed by the clipping stage.
  uint64_t nof_processed_samples;
  /// Total number of samples clipped by the clipping stage.
  uint64_t nof_clipped_samples;
  /// Expected probability that an input sample is clipped, calculated as \c nof_clipped_samples /
  /// \c nof_processed_samples.
  double clipping_probability;
  /// Indicates whether clipping is enabled in the amplitude controller.
  bool clipping_enabled;
};

/// \brief Describes an amplitude controller interface.
///
/// The amplitude controller adapts the amplitude of the signal to be injected to the radio device.
class amplitude_controller
{
public:
  /// Default destructor.
  virtual ~amplitude_controller() = default;

  /// \brief Performs amplitude control and provides signal power measurements.
  /// \param[out] output Output signal samples.
  /// \param[in] input Input signal samples.
  /// \return Amplitude controller metrics, including power measurements and clipping statistics.
  /// \note The output size and the input size must be equal.
  virtual amplitude_controller_metrics process(span<cf_t> output, span<const cf_t> input) = 0;
};

} // namespace ocudu
