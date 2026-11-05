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
///
/// The stream operates in real time, using the RF clock as time reference, and emulates the following real time issues
/// coming from the stack:
///
/// - TX side lates:      a transmission request contains samples in the past with respect to previous transmissions.
///                       The samples are dropped.
/// - TX side underflows: a transmission request contains samples that are not ahead of the current RF timestamp by at
///                       least \ref tx_processing_delay_samples. The samples are dropped.
/// - TX side blocking:   a transmission request contains samples ahead of the current RF timestamp by more than
///                       \ref max_nof_buffered_samples. The call blocks until the samples fit or the radio stops.
/// - RX side overflows:  the next sample to receive lags the current RF timestamp by more than
///                       \ref max_nof_buffered_samples. This is detected on the next call to \c receive(). Zeros are
///                       returned in place of the lost samples.
///
/// The loopback buffer also drops transmitted samples that would overwrite samples the receiver has not read yet.
class radio_stream_realtime_loopback_impl : public baseband_gateway,
                                            public baseband_gateway_transmitter,
                                            public baseband_gateway_receiver
{
public:
  /// Session resources shared by all the streams. The referenced objects outlive the streams.
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
  /// \param[in] sampling_rate_hz Sampling rate in hertz.
  /// \param[in] state            Session resources shared by all the streams.
  radio_stream_realtime_loopback_impl(unsigned nof_channels, double sampling_rate_hz, const shared_state& state);

  /// \brief Prepares the stream for receiving samples starting at the given timestamp.
  ///
  /// Called before the session signals the start request.
  /// \param[in] init_time Timestamp of the first received sample.
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
  /// Loopback buffer size, in multiples of the buffering depth.
  static constexpr unsigned nof_buffer_laps = 10;

  /// \brief Buffering depth of the emulated radio, i.e., 10 ms of samples.
  ///
  /// The receiver overflows when it lags the RF time by more samples, and the transmitter blocks when it leads the RF
  /// time by more samples.
  uint64_t max_nof_buffered_samples;
  /// Minimum time, in samples, between passing samples to the emulated transmitter and their transmission, i.e., 10 us.
  uint64_t tx_processing_delay_samples;
  /// Loopback buffer connecting the transmitter and the receiver.
  loopback_buffer buffer;
};

} // namespace ocudu
