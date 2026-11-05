// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "loopback_buffer.h"
#include "ocudu/adt/unique_function.h"
#include "ocudu/gateways/baseband/baseband_gateway.h"
#include "ocudu/gateways/baseband/baseband_gateway_receiver.h"
#include "ocudu/gateways/baseband/baseband_gateway_transmitter.h"
#include "ocudu/support/synchronization/stop_event.h"
#include <atomic>

namespace ocudu {

/// \brief Realtime loopback radio stream.
///
/// Connects the transmitter of a stream to the receiver of the same stream through a loopback buffer. All the streams
/// of a session share the RF clock, the start request and the stop control.
class radio_stream_realtime_loopback_impl : public baseband_gateway,
                                            public baseband_gateway_transmitter,
                                            public baseband_gateway_receiver
{
public:
  /// Collects the session resources shared by all the streams.
  struct shared_state {
    /// Returns the current RF timestamp.
    const unique_function<baseband_gateway_timestamp()>& get_current_rf_timestamp;
    /// Start request flag.
    const std::atomic<bool>& start_requested;
    /// Stop control.
    rt_stop_event_source& stop_control;
  };

  /// \brief Creates a loopback stream.
  /// \param[in] nof_channels     Number of channels of the stream.
  /// \param[in] sampling_rate_Hz Sampling rate in hertz.
  /// \param[in] state            Session resources shared by all the streams.
  radio_stream_realtime_loopback_impl(unsigned nof_channels, double sampling_rate_Hz, const shared_state& state);

  /// \brief Prepares the stream for receiving samples starting at the given timestamp.
  ///
  /// It must be called before the session start request is signaled.
  void start(baseband_gateway_timestamp init_time);

  // See the baseband_gateway interface for documentation.
  unsigned get_transmitter_optimal_buffer_size() const override { return 0; }

  // See the baseband_gateway interface for documentation.
  unsigned get_receiver_optimal_buffer_size() const override;

  // See the baseband_gateway interface for documentation.
  baseband_gateway_transmitter& get_transmitter() override { return *this; }

  // See the baseband_gateway interface for documentation.
  baseband_gateway_receiver& get_receiver() override { return *this; }

  // See the baseband_gateway_receiver interface for documentation.
  metadata receive(baseband_gateway_buffer_writer& data) override;

  // See the baseband_gateway_transmitter interface for documentation.
  void transmit(const baseband_gateway_buffer_reader& data, const baseband_gateway_transmitter_metadata& md) override;

private:
  /// Radio logger.
  ocudulog::basic_logger& logger;
  /// Sampling rate in hertz.
  double sampling_rate_hz;
  /// Session resources shared by all the streams.
  shared_state state;
  /// Timestamp of the next sample to deliver to the stack.
  baseband_gateway_timestamp next_receive_timestamp = 0;
  /// Expected timestamp of the next sample to be transmitted.
  baseband_gateway_timestamp next_transmit_timestamp = 0;
  /// Maximum number of samples the emulated receiver buffers before overflowing.
  uint64_t max_nof_buffered_rx_samples;
  /// Maximum number of samples the emulated transmitter buffers ahead of the RF time.
  uint64_t max_nof_buffered_tx_samples;
  /// Minimum time, in samples, between passing samples to the emulated transmitter and their transmission.
  uint64_t tx_processing_delay_samples;
  /// Loopback buffer connecting the transmitter and the receiver.
  loopback_buffer buffer;
};

} // namespace ocudu
