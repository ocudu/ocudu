// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "radio_session_realtime_loopback_impl.h"
#include "ocudu/gateways/baseband/buffer/baseband_gateway_buffer_reader.h"
#include "ocudu/gateways/baseband/buffer/baseband_gateway_buffer_writer.h"
#include "ocudu/gateways/baseband/buffer/baseband_gateway_buffer_writer_view.h"
#include "ocudu/ocudulog/ocudulog.h"
#include <thread>

using namespace ocudu;

radio_session_realtime_loopback_impl::radio_session_realtime_loopback_impl(const radio_configuration::radio& config,
                                                                           task_executor&        async_task_executor,
                                                                           radio_event_notifier& notification_handler) :
  radio_session_realtime_loopback_impl(config,
                                       async_task_executor,
                                       notification_handler,
                                       get_current_rf_timestamp_realtime_clock)
{
}

radio_session_realtime_loopback_impl::radio_session_realtime_loopback_impl(
    const radio_configuration::radio&                    config,
    task_executor&                                       async_task_executor,
    radio_event_notifier&                                notification_handler,
    const unique_function<baseband_gateway_timestamp()>& current_rf_timestamp_fn) :
  logger(ocudulog::fetch_basic_logger("RF")),
  ts0_epoch(std::chrono::duration_cast<std::chrono::nanoseconds>(
      std::chrono::high_resolution_clock::now().time_since_epoch())),
  sampling_rate_hz(config.sampling_rate_Hz),
  next_receive_timestamp(0),
  next_transmit_timestamp(0),
  max_nof_buffered_rx_samples(static_cast<uint64_t>(sampling_rate_hz / 100)),
  max_nof_buffered_tx_samples(max_nof_buffered_rx_samples),
  tx_processing_delay_samples(static_cast<uint64_t>(sampling_rate_hz / 100000)),
  start_requested(false),
  buffer(config.rx_streams[0].channels.size(), max_nof_buffered_tx_samples * 10),
  get_current_rf_timestamp(current_rf_timestamp_fn)
{
  report_fatal_error_if_not(tx_processing_delay_samples < max_nof_buffered_tx_samples,
                            "The emulated TX processing delay must be smaller than the emulated TX buffer size.");
  report_error_if_not(max_nof_buffered_tx_samples >= static_cast<uint64_t>(sampling_rate_hz / 10000),
                      "The emulated TX buffer must hold at least 100 microseconds of baseband signal.");
  report_error_if_not(max_nof_buffered_rx_samples >= static_cast<uint64_t>(sampling_rate_hz / 10000),
                      "The emulated RX buffer must hold at least 100 microseconds of baseband signal.");
}

unsigned radio_session_realtime_loopback_impl::get_receiver_optimal_buffer_size() const
{
  /// Return a buffer size capable of holding 100 microseconds of samples.
  return sampling_rate_hz / 10000;
}

// See the radio_session interface for documentation.
baseband_gateway_timestamp radio_session_realtime_loopback_impl::read_current_time()
{
  return get_current_rf_timestamp();
}

void radio_session_realtime_loopback_impl::start(baseband_gateway_timestamp init_time)
{
  // Reset the stop control. This will block if there is an earlier stop request that has not been completed.
  stop_control.reset();

  // Set the next timestamp for the RX samples.
  next_receive_timestamp = init_time;
  buffer.set_first_expected_rx_timestamp(init_time);
  bool expected_start_requested = false;
  if (!start_requested.compare_exchange_weak(expected_start_requested, true)) {
    report_fatal_error("Called start when radio is already running");
  }
}

void radio_session_realtime_loopback_impl::stop()
{
  // Request the radio to stop.
  stop_control.stop();
}

bool radio_session_realtime_loopback_impl::set_tx_gain(unsigned port_id, double gain_dB)
{
  return true;
}

bool radio_session_realtime_loopback_impl::set_rx_gain(unsigned port_id, double gain_dB)
{
  return true;
}

bool radio_session_realtime_loopback_impl::set_tx_freq(unsigned stream_id, double center_freq_Hz)
{
  return true;
}

bool radio_session_realtime_loopback_impl::set_rx_freq(unsigned stream_id, double center_freq_Hz)
{
  return true;
}

baseband_gateway_receiver::metadata radio_session_realtime_loopback_impl::receive(baseband_gateway_buffer_writer& data)
{
  // Sleep until the radio is requested to start.
  while (!start_requested.load()) {
    OCUDU_RTSAN_SCOPED_DISABLER(scoped_disabler);
    std::this_thread::sleep_for(std::chrono::microseconds(1));
  }

  unsigned nof_requested_samples = data.get_nof_samples();
  ocudu_assert(
      nof_requested_samples <= max_nof_buffered_rx_samples,
      "Cannot provide the requested number of samples (i.e., {}), since the maximum RX buffer size is {} samples.",
      nof_requested_samples,
      max_nof_buffered_rx_samples);

  // If the timestamp of the next sample to be provided to the stack is smaller than the timestamp of the earliest
  // sample in the RX buffer, it means that samples have been dropped due to a buffer overflow.
  baseband_gateway_timestamp current_rf_timestamp = get_current_rf_timestamp();
  baseband_gateway_timestamp earliest_timestamp_in_rx_buffer =
      current_rf_timestamp > max_nof_buffered_rx_samples ? current_rf_timestamp - max_nof_buffered_rx_samples : 0;
  if (next_receive_timestamp < earliest_timestamp_in_rx_buffer) {
    logger.warning(
        "RX Overflow detected while receiving TS={}, current RF TS: {}", next_receive_timestamp, current_rf_timestamp);

    unsigned nof_samples_to_skip = earliest_timestamp_in_rx_buffer - next_receive_timestamp;
    if (nof_samples_to_skip >= nof_requested_samples) {
      // If there are more samples to skip than requested samples, zero the entire buffer.
      for (unsigned i_channel = 0, nof_channels = data.get_nof_channels(); i_channel != nof_channels; ++i_channel) {
        ocuduvec::zero(data.get_channel_buffer(i_channel));
      }
    } else {
      // Otherwise, zero the first samples of the output, and fill the remaining samples from the loopback buffer.
      for (unsigned i_channel = 0, nof_channels = data.get_nof_channels(); i_channel != nof_channels; ++i_channel) {
        ocuduvec::zero(data.get_channel_buffer(i_channel).first(nof_samples_to_skip));

        // Copy the available samples in the same positions as if there had been no overflow.
        baseband_gateway_buffer_writer_view offset_buffer(
            data, nof_samples_to_skip, nof_requested_samples - nof_samples_to_skip);
        buffer.read(offset_buffer, earliest_timestamp_in_rx_buffer);
      }
    }

    // Update the timestamp for the next receive call and return the current timestamp.
    metadata return_md = {.ts = next_receive_timestamp};
    next_receive_timestamp += nof_requested_samples;

    return return_md;
  }

  // Sleep until all the requested samples are available in the buffer.
  baseband_gateway_timestamp last_requested_sample_timestamp = next_receive_timestamp + nof_requested_samples - 1;
  while (last_requested_sample_timestamp > get_current_rf_timestamp()) {
    OCUDU_RTSAN_SCOPED_DISABLER(scoped_disabler);
    std::this_thread::sleep_for(std::chrono::microseconds(1));
  }

  // Read samples from the loopback buffer.
  buffer.read(data, next_receive_timestamp);

  // Update the timestamp for the next receive call and return the current timestamp.
  metadata return_md = {.ts = next_receive_timestamp};
  next_receive_timestamp += nof_requested_samples;
  return return_md;
}

void radio_session_realtime_loopback_impl::transmit(const baseband_gateway_buffer_reader&        data,
                                                    const baseband_gateway_transmitter_metadata& md)
{
  auto token = stop_control.get_token();
  if (OCUDU_UNLIKELY(token.is_stop_requested())) {
    return;
  }

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

  // If the timestamp of the first sample to be transmitted is not ahead of the current timestamp by at least the TX
  // processing delay, notify an underflow and return (samples are dropped).
  baseband_gateway_timestamp current_rf_timestamp            = get_current_rf_timestamp();
  baseband_gateway_timestamp required_tx_timestamp_in_buffer = current_rf_timestamp + tx_processing_delay_samples;
  if (first_requested_sample_ts < required_tx_timestamp_in_buffer) {
    logger.warning("TX underflow detected while transmitting TS={}, required TS: {}, lagging by {} samples.",
                   first_requested_sample_ts,
                   required_tx_timestamp_in_buffer,
                   required_tx_timestamp_in_buffer - first_requested_sample_ts);
    return;
  }

  // If the timestamp of the samples requested to be transmitted is ahead of the current RF timestamp by more than the
  // TX buffering depth, block until that's no longer the case.
  baseband_gateway_timestamp last_requested_sample_ts = first_requested_sample_ts + nof_requested_samples - 1;
  while (last_requested_sample_ts >
         (get_current_rf_timestamp() - tx_processing_delay_samples + max_nof_buffered_tx_samples)) {
    OCUDU_RTSAN_SCOPED_DISABLER(scoped_disabler);
    std::this_thread::sleep_for(std::chrono::microseconds(1));
  }

  // Write the samples to the loopback buffer.
  buffer.write(data, first_requested_sample_ts);

  // Update the next expected TX timestamp.
  next_transmit_timestamp = last_requested_sample_ts + 1;
}
