// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "loopback_buffer.h"
#include "ocudu/ocuduvec/copy.h"

using namespace ocudu;

void loopback_buffer::write(const baseband_gateway_buffer_reader& data, baseband_gateway_timestamp timestamp)
{
  unsigned nof_requested_samples = data.get_nof_samples();
  ocudu_assert(nof_requested_samples <= buffer.get_nof_samples(),
               "Buffer size is too small to hold the requested data.");
  ocudu_assert(data.get_nof_channels() == buffer.get_nof_channels(),
               "Number of channels in the data does not match the buffer.");

  // Check that the requested samples are not in the past with respect to previous transmissions. This ensures that the
  // last TX timestamp is never set to a smaller value. This comparison does not need to be atomic because the last TX
  // timestamp is only updated from this thread.
  baseband_gateway_timestamp last_tx_ts = last_tx_timestamp.load(std::memory_order_acquire);
  report_fatal_error_if_not(
      timestamp > last_tx_ts,
      "Requesting to write samples [{}, {}) which are in the past compared to the last TX timestamp, i.e., {}.",
      timestamp,
      timestamp + nof_requested_samples,
      last_tx_ts);

  // Check that the buffer contents to be overwritten have already been read. This comparison does not need to be atomic
  // because the last received timestamp can never be set to a smaller value. Therefore, if the condition for safe
  // writing is met at the atomic read, it is guaranteed to be met in the future.
  baseband_gateway_timestamp last_rx_ts = last_rx_timestamp.load(std::memory_order_acquire);
  report_fatal_error_if_not(
      (timestamp + nof_requested_samples) - last_rx_ts <= buffer.get_nof_samples(),
      "Attempting to write samples [{}, {}) that would overwrite unread sample {}. buffer size: {}",
      timestamp,
      timestamp + nof_requested_samples,
      last_rx_ts,
      buffer.get_nof_samples());

  // Compute the location in the buffer where the first sample of the data should be written.
  unsigned write_index = get_index(timestamp);

  // Determine if it is required to wrap around the buffer;
  unsigned write_length     = std::min(nof_requested_samples, buffer.get_nof_samples() - write_index);
  unsigned remaining_length = nof_requested_samples - write_length;

  for (unsigned i_chanel = 0, nof_channels = data.get_nof_channels(); i_chanel != nof_channels; ++i_chanel) {
    span<const ci16_t> src = data.get_channel_buffer(i_chanel);
    span<ci16_t>       dst = buffer.get_writer().get_channel_buffer(i_chanel);
    ocuduvec::copy(dst.subspan(write_index, write_length), src.first(write_length));

    // Wrap around the buffer if necessary.
    if (remaining_length > 0) {
      ocuduvec::copy(dst.first(remaining_length), src.subspan(write_length, remaining_length));
    }
  }

  // Update the timestamp of the last transmitted sample in the buffer.
  last_tx_timestamp.store(timestamp + nof_requested_samples - 1, std::memory_order_release);
}

void loopback_buffer::set_first_expected_rx_timestamp(baseband_gateway_timestamp timestamp)
{
  // Check that no samples have been written to the buffer.
  if (last_tx_timestamp.load(std::memory_order_acquire) != 0U) {
    report_fatal_error("The first RX timestamp must be set before any sample is written to the buffer.");
  }

  // Do nothing if the requested timestamp if 0.
  if (timestamp == 0) {
    return;
  }

  // Set the last RX timestamp to one sample before the first expected timestamp to be requested from the buffer.
  baseband_gateway_timestamp expected_last_rx_timestamp = 0U;
  if (!last_rx_timestamp.compare_exchange_strong(expected_last_rx_timestamp, timestamp - 1U)) {
    report_fatal_error("Unexpected last RX timestamp when setting it for the first time");
  }
}

void loopback_buffer::read(baseband_gateway_buffer_writer& data, baseband_gateway_timestamp timestamp)
{
  unsigned nof_requested_samples = data.get_nof_samples();
  ocudu_assert(nof_requested_samples <= buffer.get_nof_samples(),
               "Buffer size is too small to hold the requested data.");
  ocudu_assert(data.get_nof_channels() == buffer.get_nof_channels(),
               "Number of channels in the data does not match the buffer.");

  // Make sure that the requested samples are not in the past with respect to previous requests. This ensures that the
  // last RX timestamp is never set to a smaller value. This comparison does not need to be atomic because the last RX
  // timestamp is only updated from this thread.
  baseband_gateway_timestamp last_rx_ts = last_rx_timestamp.load(std::memory_order_acquire);
  report_fatal_error_if_not((timestamp == 0) || (timestamp > last_rx_ts),
                            "Attempting to read samples in the past. Timestamp: {}, last read timestamp: {}",
                            timestamp,
                            last_rx_ts);

  // Check if the requested samples have already been written to the buffer. This comparison does not need to be
  // atomic because the last TX timestamp is never set to a smaller value. Therefore, if the condition for safe reading
  // is met at the atomic read, it is guaranteed to be met in the future.
  baseband_gateway_timestamp last_tx_ts = last_tx_timestamp.load(std::memory_order_acquire);
  if (last_tx_ts < timestamp + nof_requested_samples - 1) {
    // Reading is not safe. Print a warning and skip the operation.
    logger.warning("Attempting to read samples [{}, {}), ahead of the last written timestamp, i.e., {}",
                   timestamp,
                   timestamp + nof_requested_samples,
                   last_tx_ts);
    return;
  }

  // Compute the location in the buffer where the first sample of the data should be read.
  unsigned read_index = get_index(timestamp);

  // Determine if it is required to wrap around the buffer;
  unsigned read_length = std::min(nof_requested_samples, buffer.get_nof_samples() - read_index);

  unsigned remaining_length = nof_requested_samples - read_length;

  for (unsigned i_channel = 0, nof_channels = data.get_nof_channels(); i_channel != nof_channels; ++i_channel) {
    span<const ci16_t> src = buffer.get_writer().get_channel_buffer(i_channel);
    span<ci16_t>       dst = data.get_channel_buffer(i_channel);
    ocuduvec::copy(dst.first(read_length), src.subspan(read_index, read_length));

    // Wrap around the buffer if necessary.
    if (remaining_length > 0) {
      ocuduvec::copy(dst.subspan(read_length, remaining_length), src.first(remaining_length));
    }
  }

  // Advance the last RX timestamp.
  last_rx_timestamp.store(timestamp + nof_requested_samples - 1, std::memory_order_release);
}
