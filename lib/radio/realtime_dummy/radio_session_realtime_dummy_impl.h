// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/gateways/baseband/baseband_gateway_receiver.h"
#include "ocudu/gateways/baseband/baseband_gateway_transmitter.h"
#include "ocudu/gateways/baseband/buffer/baseband_gateway_buffer_dynamic.h"
#include "ocudu/radio/radio_session.h"
#include "ocudu/support/executors/task_executor.h"

namespace ocudu {

/// Describes a loopback radio session.
class radio_session_realtime_dummy_impl : public radio_session,
                                          public radio_management_plane,
                                          public baseband_gateway,
                                          public baseband_gateway_transmitter,
                                          public baseband_gateway_receiver
{
  /// Radio session logger.
  ocudulog::basic_logger& logger;

public:
  radio_session_realtime_dummy_impl(const radio_configuration::radio& config,
                                    task_executor&                    async_task_executor,
                                    radio_event_notifier&             notification_handler);

  // See the radio_session interface for documentation.
  radio_management_plane& get_management_plane() override { return *this; }

  // See the radio_session interface for documentation.
  baseband_gateway& get_baseband_gateway(unsigned stream_id) override
  {
    ocudu_assert(stream_id == 0, "Only a single stream is supported", stream_id);

    return *this;
  }

  // See the radio_session interface for documentation.
  baseband_gateway_timestamp read_current_time() override;

  // See the radio_session interface for documentation.
  void start(baseband_gateway_timestamp init_time) override;

  // See the radio_session interface for documentation.
  void stop() override;

  // See the radio_management_plane interface for documentation.
  bool set_tx_gain(unsigned port_id, double gain_dB) override;

  // See the radio_management_plane interface for documentation.
  bool set_rx_gain(unsigned port_id, double gain_dB) override;

  // See the radio_management_plane interface for documentation.
  bool set_tx_freq(unsigned stream_id, double center_freq_Hz) override;

  // See the radio_management_plane interface for documentation.
  bool set_rx_freq(unsigned stream_id, double center_freq_Hz) override;

  // See the baseband_gateway interface for documentation.
  unsigned get_transmitter_optimal_buffer_size() const override { return 0; }

  // See the baseband_gateway interface for documentation.
  unsigned get_receiver_optimal_buffer_size() const override { return 0; }

  // See the baseband_gateway interface for documentation.
  baseband_gateway_transmitter& get_transmitter() override { return *this; }

  // See the baseband_gateway interface for documentation.
  baseband_gateway_receiver& get_receiver() override { return *this; }

  // See the baseband_gateway_receiver interface for documentation.
  metadata receive(baseband_gateway_buffer_writer& data) override;

  // See the baseband_gateway_transmitter interface for documentation.
  void transmit(const baseband_gateway_buffer_reader& data, const baseband_gateway_transmitter_metadata& md) override;

private:
  /// The system time corresponding to timestamp 0 in nanoseconds.
  std::chrono::nanoseconds ts0_epoch;

  /// Timestamp of the next sample to deliver to the stack.
  baseband_gateway_timestamp next_receive_timestamp;

  /// \brief Expected timestamp of the next sample to be transmitted.
  ///
  /// It is used to detect late events, where a transmission request contains samples in the past relative to previous
  /// transmissions.
  baseband_gateway_timestamp next_transmit_timestamp;

  /// Emulates the buffering depth limitations of the radio receiver.
  ///
  /// It is used to simulate overflow events, when the buffer reaches its maximum occupation.
  uint64_t max_nof_buffered_rx_samples;

  /// \brief Emulates the buffering depth limitations of the radio transmitter.
  ///
  /// It is used to block transmit calls in the event that the buffer reaches its maximum occupation.
  uint64_t max_nof_buffered_tx_samples;

  /// \brief Emulates the transmission processing delay of the radio.
  ///
  /// If is used to simulate underflow events. A real radio requires that the samples to be transmitted are passed to
  /// the driver some time in advance of the actual transmission time. Given a current RX timestamp, if the radio cannot
  /// retrieve samples from the buffer at \c get_current_rx_timestamp() + \c tx_processing_delay_samples, an underflow
  /// event is logged.
  uint64_t tx_processing_delay_samples;

  /// Sampling rate common to all channels.
  double sampling_rate_hz;

  std::atomic<bool> start_requested;

  /// \brief Derives the current RF timestamp, based on the system time and the epoch of timestamp 0.
  ///
  /// This is the timestamp of the samples currently at the antenna port, and is common for the transmitter and receiver
  /// chains.
  baseband_gateway_timestamp get_current_rf_timestamp();
};

} // namespace ocudu
