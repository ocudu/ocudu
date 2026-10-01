// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/gateways/baseband/baseband_gateway_timestamp.h"
#include <chrono>

namespace ocudu {

/// Interface for transmitter alignment from the receiver.
class radio_zmq_tx_align_interface
{
public:
  /// Default destructor.
  virtual ~radio_zmq_tx_align_interface() = default;

  /// Aligns a transmit stream by transmitting zeros until the indicated timestamp.
  virtual bool align(baseband_gateway_timestamp timestamp, std::chrono::milliseconds timeout) = 0;
};

} // namespace ocudu
