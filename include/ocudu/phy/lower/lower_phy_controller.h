// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/gateways/baseband/baseband_gateway_timestamp.h"

namespace ocudu {

/// \brief Lower physical layer - control interface.
///
/// Provides a start and stop methods.
///
/// \remark The methods \c start() and \c stop() must be called in order and only once; other uses will result in
/// undefined behavior.
class lower_phy_controller
{
public:
  /// Default destructor.
  virtual ~lower_phy_controller() = default;

  /// \brief Starts the lower physical layer operation.
  ///
  /// The fist uplink processing block is expected at \c init_time and the first downlink processing block transmission
  /// will be relative to this time.
  ///
  /// \param[in] init_time     Initial time in clock ticks.
  /// \param[in] sfn0_ref_time System Frame Number (SFN) zero reference time.
  virtual void start(baseband_gateway_timestamp init_time, baseband_gateway_timestamp sfn0_ref_time = 0) = 0;

  /// \brief Stops the lower physical layer operation.
  ///
  /// Waits for all asynchronous processes to be over before returning.
  virtual void stop() = 0;
};

} // namespace ocudu
