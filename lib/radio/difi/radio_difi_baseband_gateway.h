// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "radio_difi_rx_stream.h"
#include "radio_difi_tx_stream.h"
#include "ocudu/gateways/baseband/baseband_gateway.h"

namespace ocudu {

/// \brief Implements baseband_gateway for DIFI.
///
/// Pairs one DIFI transmit stream (DL: DU → RU) with one DIFI receive stream (UL: RU → DU).
class radio_difi_baseband_gateway : public baseband_gateway
{
public:
  radio_difi_baseband_gateway(radio_event_notifier&                           notifier,
                              const radio_difi_tx_stream::stream_description& tx_config,
                              const radio_difi_rx_stream::stream_description& rx_config) :
    tx_stream(tx_config, notifier), rx_stream(rx_config, notifier)
  {
  }

  // See baseband_gateway for documentation.
  baseband_gateway_transmitter& get_transmitter() override { return tx_stream; }

  // See baseband_gateway for documentation.
  baseband_gateway_receiver& get_receiver() override { return rx_stream; }

  /// Direct access to the transmit stream (used by session for start/stop).
  radio_difi_tx_stream& get_tx_stream() { return tx_stream; }

  /// Direct access to the receive stream (used by session for start/stop).
  radio_difi_rx_stream& get_rx_stream() { return rx_stream; }

  /// Returns true if both the transmit and receive streams initialised successfully.
  bool is_successful() const { return tx_stream.is_successful() && rx_stream.is_successful(); }

private:
  radio_difi_tx_stream tx_stream;
  radio_difi_rx_stream rx_stream;
};

} // namespace ocudu
