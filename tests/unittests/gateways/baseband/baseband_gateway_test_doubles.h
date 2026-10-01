// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "baseband_gateway_receiver_test_doubles.h"
#include "baseband_gateway_transmitter_test_doubles.h"
#include "ocudu/gateways/baseband/baseband_gateway.h"
#include <random>

namespace ocudu {

/// \brief Baseband gateway spy class.
///
/// Records all interactions from a lower physical layer with a baseband gateway. The receive method generates random
/// baseband samples and metadata.
class baseband_gateway_spy : public baseband_gateway
{
public:
  // See interface for documentation.
  baseband_gateway_transmitter& get_transmitter() override { return transmitter; }

  // See interface for documentation.
  baseband_gateway_receiver& get_receiver() override { return receiver; }

  /// Gets all transmit entries.
  const std::vector<baseband_gateway_transmitter_spy::entry_t>& get_transmit_entries() const
  {
    return transmitter.get_entries();
  }

  /// Gets all receive entries.
  const std::vector<baseband_gateway_receiver_spy::entry_t>& get_receive_entries() const
  {
    return receiver.get_entries();
  }

  void set_receiver_current_timestamp(baseband_gateway_timestamp timestamp)
  {
    receiver.set_current_timestamp(timestamp);
  }

  /// Clears all types of entries.
  void clear_all_entries()
  {
    transmitter.clear();
    receiver.clear();
  }

private:
  baseband_gateway_transmitter_spy transmitter;
  baseband_gateway_receiver_spy    receiver;
};

} // namespace ocudu
