// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/gateways/baseband/baseband_gateway_base.h"

namespace ocudu {

class baseband_gateway_transmitter;
class baseband_gateway_receiver;

/// Describes a baseband gateway interface.
class baseband_gateway : public baseband_gateway_base
{
public:
  /// \brief Gets the transmitter optimal buffer size.
  /// \return The optimal transmit buffer size if available. Otherwise, zero.
  virtual unsigned get_transmitter_optimal_buffer_size() const { return 0; }

  /// \brief Gets the receiver optimal buffer size.
  /// \return The optimal receive buffer size if available. Otherwise, zero.
  virtual unsigned get_receiver_optimal_buffer_size() const { return 0; }

  /// Gets the transmitter gateway.
  virtual baseband_gateway_transmitter& get_transmitter() = 0;

  /// Gets the receiver gateway.
  virtual baseband_gateway_receiver& get_receiver() = 0;
};

} // namespace ocudu
