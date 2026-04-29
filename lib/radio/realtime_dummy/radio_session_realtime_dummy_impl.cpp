// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "radio_session_realtime_dummy_impl.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/ocuduvec/zero.h"
#include <thread>

using namespace ocudu;

radio_session_realtime_dummy_impl::radio_session_realtime_dummy_impl(const radio_configuration::radio& config,
                                                                     task_executor&        async_task_executor,
                                                                     radio_event_notifier& notification_handler) :
  logger(ocudulog::fetch_basic_logger("RF"))
{
  // Set the epoch of TS0.
  ts0_epoch = std::chrono::duration_cast<std::chrono::nanoseconds>(
      std::chrono::high_resolution_clock::now().time_since_epoch());

  sampling_rate_hz = config.sampling_rate_Hz;

  // Set the emulated TX and RX buffer sizes to hold 1 ms of baseband signal.
  max_nof_buffered_rx_samples = static_cast<uint64_t>(sampling_rate_hz / 1000);
  max_nof_buffered_tx_samples = max_nof_buffered_rx_samples;

  next_transmit_timestamp = 0;
  next_receive_timestamp  = 0;

  start_requested.store(false);

  // Set the TX processing delay of the radio to 1 microsecond.
  tx_processing_delay_samples = static_cast<uint64_t>(sampling_rate_hz / 100000);

  report_fatal_error_if_not(tx_processing_delay_samples < max_nof_buffered_tx_samples,
                            "The emulated TX processing delay must be smaller than the emulated TX buffer size.");
  report_error_if_not(max_nof_buffered_tx_samples >= static_cast<uint64_t>(sampling_rate_hz / 10000),
                      "The emulated TX buffer must hold at least 100 microseconds of baseband signal.");
  report_error_if_not(max_nof_buffered_rx_samples >= static_cast<uint64_t>(sampling_rate_hz / 10000),
                      "The emulated RX buffer must hold at least 100 microseconds of baseband signal.");
}

// See the radio_session interface for documentation.
baseband_gateway_timestamp radio_session_realtime_dummy_impl::read_current_time()
{
  return get_current_rf_timestamp();
}

void radio_session_realtime_dummy_impl::start(baseband_gateway_timestamp init_time)
{
  // Set the next timestamp for the RX samples.
  next_receive_timestamp        = init_time;
  bool expected_start_requested = false;
  if (!start_requested.compare_exchange_weak(expected_start_requested, true)) {
    report_fatal_error("Called start when radio was already running");
  }
}

void radio_session_realtime_dummy_impl::stop() {}

bool radio_session_realtime_dummy_impl::set_tx_gain(unsigned port_id, double gain_dB)
{
  return true;
}

bool radio_session_realtime_dummy_impl::set_rx_gain(unsigned port_id, double gain_dB)
{
  return true;
}

bool radio_session_realtime_dummy_impl::set_tx_freq(unsigned stream_id, double center_freq_Hz)
{
  return true;
}

bool radio_session_realtime_dummy_impl::set_rx_freq(unsigned stream_id, double center_freq_Hz)
{
  return true;
}

baseband_gateway_receiver::metadata radio_session_realtime_dummy_impl::receive(baseband_gateway_buffer_writer& data)
{
  // Sleep until the radio is requested to start.
  while (!start_requested.load()) {
    OCUDU_RTSAN_SCOPED_DISABLER(scoped_disabler);
    std::this_thread::sleep_for(std::chrono::microseconds(10));
  }

  unsigned nof_requested_samples = data.get_nof_samples();
  ocudu_assert(
      nof_requested_samples <= max_nof_buffered_rx_samples,
      "Cannot provide the requested number of samples (i.e., {}), since the maximum RX buffer size is {} samples.",
      nof_requested_samples,
      max_nof_buffered_rx_samples);

  // Fill the buffer with zeros.
  for (unsigned i_channel = 0, nof_channels = data.get_nof_channels(); i_channel < nof_channels; ++i_channel) {
    ocuduvec::zero(data.get_channel_buffer(i_channel));
  }

  // If the timestamp of the next sample to be provided to the stack is smaller than the timestamp of the earliest
  // sample in the RX buffer, it means that samples have been dropped due to a buffer overflow.
  baseband_gateway_timestamp current_rf_timestamp = get_current_rf_timestamp();
  baseband_gateway_timestamp earliest_timestamp_in_rx_buffer =
      current_rf_timestamp > max_nof_buffered_rx_samples ? current_rf_timestamp - max_nof_buffered_rx_samples : 0;
  if (next_receive_timestamp < earliest_timestamp_in_rx_buffer) {
    logger.warning(
        "RX Overflow detected while receiving TS={}, current RF TS: {}", next_receive_timestamp, current_rf_timestamp);

    // Provide the earliest samples in the buffer to the stack and update the next RX timestamp accordingly. This
    // results in a discontinuity in the timestamp of the received samples, which the stack must handle.
    next_receive_timestamp = earliest_timestamp_in_rx_buffer + data.get_nof_samples();
    return metadata{.ts = earliest_timestamp_in_rx_buffer};
  }

  // Sleep until all the requested samples are available in the buffer.
  baseband_gateway_timestamp last_requested_sample_timestamp = next_receive_timestamp + nof_requested_samples - 1;
  while (last_requested_sample_timestamp > get_current_rf_timestamp()) {
    OCUDU_RTSAN_SCOPED_DISABLER(scoped_disabler);
    std::this_thread::sleep_for(std::chrono::microseconds(1));
  }

  // Update the timestamp for the next receive call and return the current timestamp.
  metadata return_md = {.ts = next_receive_timestamp};
  next_receive_timestamp += nof_requested_samples;
  return return_md;
}

void radio_session_realtime_dummy_impl::transmit(const baseband_gateway_buffer_reader&        data,
                                                 const baseband_gateway_transmitter_metadata& md)
{
  baseband_gateway_timestamp first_requested_sample_ts = md.ts;
  unsigned                   nof_requested_samples     = data.get_nof_samples();

  // If the samples to be transmitted are in the past with respect to previous transmissions, notify a late and return.
  if (first_requested_sample_ts < next_transmit_timestamp) {
    logger.warning("TX late detected at while transmitting TS={}, expected TS: {}",
                   first_requested_sample_ts,
                   next_transmit_timestamp);

    // Update the next transmit timestamp to the latest sample that could be transmitted.
    next_transmit_timestamp = std::max(next_transmit_timestamp, first_requested_sample_ts + nof_requested_samples);
    return;
  }

  // If the samples to be transmitted are not contiguous to the latest transmission, notify a transmission gap.
  if ((next_transmit_timestamp != 0) && (first_requested_sample_ts > next_transmit_timestamp)) {
    logger.warning("TX discontinuity detected while transmitting TS={}, expected TS: {}",
                   first_requested_sample_ts,
                   next_transmit_timestamp);
  }

  // If the timestamp of the first sample to be transmitted is not ahead of the current timestamp by at least the TX
  // processing delay, notify an underflow and return (samples are dropped).
  baseband_gateway_timestamp current_rf_timestamp = get_current_rf_timestamp();
  baseband_gateway_timestamp required_tx_timestamp_in_buffer =
      current_rf_timestamp > tx_processing_delay_samples ? current_rf_timestamp - tx_processing_delay_samples : 0;
  if (first_requested_sample_ts < required_tx_timestamp_in_buffer) {
    logger.warning("TX underflow detected while transmitting TS={}, current RF TS: {}",
                   first_requested_sample_ts,
                   current_rf_timestamp);
    return;
  }

  // If the timestamp of the samples requested to be transmitted is ahead of the current RF timestamp by more than the
  // TX buffering depth, block until that's no longer the case.
  baseband_gateway_timestamp last_requested_sample_ts = first_requested_sample_ts + nof_requested_samples - 1;
  while (last_requested_sample_ts >
         (get_current_rf_timestamp() + tx_processing_delay_samples + max_nof_buffered_tx_samples)) {
    OCUDU_RTSAN_SCOPED_DISABLER(scoped_disabler);
    std::this_thread::sleep_for(std::chrono::microseconds(1));
  }

  // Update the next expected TX timestamp.
  next_transmit_timestamp = last_requested_sample_ts + 1;
}

baseband_gateway_timestamp radio_session_realtime_dummy_impl::get_current_rf_timestamp()
{
  // Get the time since the epoch.
  auto time_since_epoch = std::chrono::duration_cast<std::chrono::nanoseconds>(
      std::chrono::high_resolution_clock::now().time_since_epoch());

  return (time_since_epoch.count() - ts0_epoch.count()) * sampling_rate_hz / 1000000000U;
}
