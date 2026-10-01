// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief Amplitude controller factories.

#pragma once

#include "ocudu/phy/lower/amplitude_controller/amplitude_controller.h"
#include <memory>

namespace ocudu {

/// Describes an amplitude controller configuration for the clipping implementation.
struct amplitude_controller_clipping_config {
  /// Enables or disables the clipping process.
  bool enable_clipping;
  /// Gain to be applied before the clipping process.
  float input_gain_dB;
  /// Signal amplitude that is mapped to the full scale output of the radio device.
  float full_scale_lin;
  /// Maximum signal amplitude in dB relative to full scale allowed at the output of the amplitude controller.
  float ceiling_dBFS;
};

/// Describes an amplitude controller factory.
class amplitude_controller_factory
{
public:
  /// Default destructor.
  virtual ~amplitude_controller_factory() = default;

  /// \brief Creates an amplitude controller.
  /// \return A unique pointer to an amplitude controller instance.
  virtual std::unique_ptr<amplitude_controller> create() = 0;
};

/// Creates a clipping amplitude controller factory.
std::shared_ptr<amplitude_controller_factory>
create_amplitude_controller_clipping_factory(const amplitude_controller_clipping_config& config);

/// Creates a scaling amplitude controller factory.
std::shared_ptr<amplitude_controller_factory> create_amplitude_controller_scaling_factory(float gain_dB_);

} // namespace ocudu
