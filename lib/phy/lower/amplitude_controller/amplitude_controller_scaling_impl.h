// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief Amplitude controller implementation using scaling.

#pragma once

#include "ocudu/phy/lower/amplitude_controller/amplitude_controller.h"
#include "ocudu/support/math/math_utils.h"

namespace ocudu {

/// Scaling amplitude controller implementation.
class amplitude_controller_scaling_impl : public amplitude_controller
{
  /// Gain factor applied to the input signal.
  float amplitude_gain;

public:
  /// \brief Constructs an amplitude controller.
  /// \param[in] gain_dB  Sets the gain factor in dB applied to the input signal.
  amplitude_controller_scaling_impl(float gain_dB) : amplitude_gain(convert_dB_to_amplitude(gain_dB))
  {
    // Do nothing.
  }

  // See interface for documentation.
  amplitude_controller_metrics process(span<cf_t> output, span<const cf_t> input) override;
};

} // namespace ocudu.
