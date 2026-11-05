// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "radio_stream_realtime_loopback_impl.h"
#include "ocudu/gateways/baseband/buffer/baseband_gateway_buffer_reader.h"
#include "ocudu/gateways/baseband/buffer/baseband_gateway_buffer_writer.h"
#include "ocudu/gateways/baseband/buffer/baseband_gateway_buffer_writer_view.h"
#include "ocudu/ocudulog/ocudulog.h"
#include <thread>

using namespace ocudu;

/// Zeroes all the channels of a baseband buffer.
static void zero_buffer(baseband_gateway_buffer_writer& data)
{
  for (unsigned i_channel = 0, nof_channels = data.get_nof_channels(); i_channel != nof_channels; ++i_channel) {
    ocuduvec::zero(data.get_channel_buffer(i_channel));
  }
}

radio_stream_realtime_loopback_impl::radio_stream_realtime_loopback_impl(unsigned            nof_channels,
                                                                         double              sampling_rate_Hz,
                                                                         const shared_state& state_) :
  logger(ocudulog::fetch_basic_logger("RF")),
  sampling_rate_hz(sampling_rate_Hz),
  state(state_),
  max_nof_buffered_rx_samples(static_cast<uint64_t>(sampling_rate_hz / 100)),
  max_nof_buffered_tx_samples(max_nof_buffered_rx_samples),
  tx_processing_delay_samples(static_cast<uint64_t>(sampling_rate_hz / 100000)),
  buffer(nof_channels, max_nof_buffered_tx_samples * 10)
{
  report_fatal_error_if_not(tx_processing_delay_samples < max_nof_buffered_tx_samples,
                            "The emulated TX processing delay must be smaller than the emulated TX buffer size.");
  report_error_if_not(max_nof_buffered_tx_samples >= static_cast<uint64_t>(sampling_rate_hz / 10000),
                      "The emulated TX buffer must hold at least 100 microseconds of baseband signal.");
  report_error_if_not(max_nof_buffered_rx_samples >= static_cast<uint64_t>(sampling_rate_hz / 10000),
                      "The emulated RX buffer must hold at least 100 microseconds of baseband signal.");
}

unsigned radio_stream_realtime_loopback_impl::get_receiver_optimal_buffer_size() const
{
  // Return a buffer size capable of holding 100 microseconds of samples.
  return sampling_rate_hz / 10000;
}

void radio_stream_realtime_loopback_impl::start(baseband_gateway_timestamp init_time)
{
  next_receive_timestamp = init_time;
  buffer.set_first_expected_rx_timestamp(init_time);
}

baseband_gateway_receiver::metadata radio_stream_realtime_loopback_impl::receive(baseband_gateway_buffer_writer& data)
{
  auto token = state.stop_control.get_token();

  // Sleep until the radio is requested to start. A stop request releases the caller without samples.
  while (!state.start_requested.load(std::memory_order_acquire)) {
    if (token.is_stop_requested()) {
      zero_buffer(data);
      return {.ts = next_receive_timestamp};
    }
    OCUDU_RTSAN_SCOPED_DISABLER(scoped_disabler);
    std::this_thread::sleep_for(std::chrono::microseconds(1));
  }

  unsigned nof_requested_samples = data.get_nof_samples();
  ocudu_assert(
      nof_requested_samples <= max_nof_buffered_rx_samples,
      "Cannot provide the requested number of samples (i.e., {}), since the maximum RX buffer size is {} samples.",
      nof_requested_samples,
      max_nof_buffered_rx_samples);

  // Samples older than the receive buffering depth are lost, as in a receiver buffer overflow.
  baseband_gateway_timestamp current_rf_timestamp = state.get_current_rf_timestamp();
  baseband_gateway_timestamp earliest_timestamp_in_rx_buffer =
      current_rf_timestamp > max_nof_buffered_rx_samples ? current_rf_timestamp - max_nof_buffered_rx_samples : 0;
  if (next_receive_timestamp < earliest_timestamp_in_rx_buffer) {
    logger.warning(
        "RX Overflow detected while receiving TS={}, current RF TS: {}", next_receive_timestamp, current_rf_timestamp);

    uint64_t nof_samples_to_skip = earliest_timestamp_in_rx_buffer - next_receive_timestamp;
    if (nof_samples_to_skip >= nof_requested_samples) {
      zero_buffer(data);
    } else {
      // Zero the lost samples and fill the remaining ones in the same positions as if there had been no overflow.
      for (unsigned i_channel = 0, nof_channels = data.get_nof_channels(); i_channel != nof_channels; ++i_channel) {
        ocuduvec::zero(data.get_channel_buffer(i_channel).first(nof_samples_to_skip));
      }
      baseband_gateway_buffer_writer_view offset_buffer(
          data, nof_samples_to_skip, nof_requested_samples - nof_samples_to_skip);
      buffer.read(offset_buffer, earliest_timestamp_in_rx_buffer);
    }

    metadata return_md = {.ts = next_receive_timestamp};
    next_receive_timestamp += nof_requested_samples;
    return return_md;
  }

  // Sleep until all the requested samples are available at the antenna. A stop request releases the caller.
  baseband_gateway_timestamp last_requested_sample_timestamp = next_receive_timestamp + nof_requested_samples - 1;
  while (last_requested_sample_timestamp > current_rf_timestamp) {
    if (token.is_stop_requested()) {
      zero_buffer(data);
      metadata return_md = {.ts = next_receive_timestamp};
      next_receive_timestamp += nof_requested_samples;
      return return_md;
    }
    OCUDU_RTSAN_SCOPED_DISABLER(scoped_disabler);
    std::this_thread::sleep_for(std::chrono::microseconds(1));
    current_rf_timestamp = state.get_current_rf_timestamp();
  }

  buffer.read(data, next_receive_timestamp);

  metadata return_md = {.ts = next_receive_timestamp};
  next_receive_timestamp += nof_requested_samples;
  return return_md;
}

void radio_stream_realtime_loopback_impl::transmit(const baseband_gateway_buffer_reader&        data,
                                                   const baseband_gateway_transmitter_metadata& md)
{
  auto token = state.stop_control.get_token();
  if (OCUDU_UNLIKELY(token.is_stop_requested())) {
    return;
  }

  baseband_gateway_timestamp first_requested_sample_ts = md.ts;
  unsigned                   nof_requested_samples     = data.get_nof_samples();
  if (nof_requested_samples == 0) {
    return;
  }

  // Samples in the past with respect to previous transmissions are late and dropped.
  if (first_requested_sample_ts < next_transmit_timestamp) {
    logger.warning("TX late detected at while transmitting TS={}, expected TS: {}",
                   first_requested_sample_ts,
                   next_transmit_timestamp);

    // Update the next transmit timestamp to the latest sample that could be transmitted.
    next_transmit_timestamp = std::max(next_transmit_timestamp, first_requested_sample_ts + nof_requested_samples);
    return;
  }

  // Samples that do not reach the emulated radio at least the TX processing delay before their transmission time
  // underflow and are dropped.
  baseband_gateway_timestamp current_rf_timestamp            = state.get_current_rf_timestamp();
  baseband_gateway_timestamp required_tx_timestamp_in_buffer = current_rf_timestamp + tx_processing_delay_samples;
  if (first_requested_sample_ts < required_tx_timestamp_in_buffer) {
    logger.warning("TX underflow detected while transmitting TS={}, required TS: {}, lagging by {} samples.",
                   first_requested_sample_ts,
                   required_tx_timestamp_in_buffer,
                   required_tx_timestamp_in_buffer - first_requested_sample_ts);
    return;
  }

  // Block while the samples are further ahead of the RF time than the emulated TX buffering depth.
  baseband_gateway_timestamp last_requested_sample_ts = first_requested_sample_ts + nof_requested_samples - 1;
  while (last_requested_sample_ts + tx_processing_delay_samples >
         state.get_current_rf_timestamp() + max_nof_buffered_tx_samples) {
    if (token.is_stop_requested()) {
      return;
    }
    OCUDU_RTSAN_SCOPED_DISABLER(scoped_disabler);
    std::this_thread::sleep_for(std::chrono::microseconds(1));
  }

  buffer.write(data, first_requested_sample_ts);

  next_transmit_timestamp = last_requested_sample_ts + 1;
}
