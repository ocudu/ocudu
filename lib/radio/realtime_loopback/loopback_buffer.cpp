// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "loopback_buffer.h"
#include "ocudu/ocuduvec/copy.h"

using namespace ocudu;

void loopback_buffer::zero(baseband_gateway_timestamp begin, baseband_gateway_timestamp end)
{
  unsigned nof_samples  = end - begin;
  unsigned index        = get_index(begin);
  unsigned first_length = std::min(nof_samples, buffer.get_nof_samples() - index);
  for (unsigned i_channel = 0, nof_channels = buffer.get_nof_channels(); i_channel != nof_channels; ++i_channel) {
    span<ci16_t> dst = buffer.get_writer().get_channel_buffer(i_channel);
    ocuduvec::zero(dst.subspan(index, first_length));
    ocuduvec::zero(dst.first(nof_samples - first_length));
  }
}

bool loopback_buffer::write(const baseband_gateway_buffer_reader& data, baseband_gateway_timestamp timestamp)
{
  unsigned nof_requested_samples = data.get_nof_samples();
  unsigned buffer_size           = buffer.get_nof_samples();
  ocudu_assert(nof_requested_samples <= buffer_size, "Buffer size is too small to hold the requested data.");
  ocudu_assert(data.get_nof_channels() == buffer.get_nof_channels(),
               "Number of channels in the data does not match the buffer.");

  // The TX end timestamp is only modified from this thread, a relaxed load is enough.
  baseband_gateway_timestamp tx_end = tx_end_timestamp.load(std::memory_order_relaxed);
  report_fatal_error_if_not(
      timestamp >= tx_end,
      "Requesting to write samples [{}, {}) which are in the past compared to the last TX timestamp, i.e., {}.",
      timestamp,
      timestamp + nof_requested_samples,
      tx_end);

  // The RX end timestamp never decreases. If the samples fit at the time of loading it, they keep fitting afterwards.
  baseband_gateway_timestamp rx_end    = rx_end_timestamp.load(std::memory_order_acquire);
  baseband_gateway_timestamp write_end = timestamp + nof_requested_samples;
  if (write_end > rx_end + buffer_size) {
    // The receiver lags too much. Dropping the samples emulates a receive overflow without corrupting unread samples.
    logger.warning("Loopback: dropping samples [{}, {}) that would overwrite unread sample {}. buffer size: {}",
                   timestamp,
                   write_end,
                   rx_end,
                   buffer_size);
    return false;
  }

  // Samples that are skipped by the transmitter must read as zeros rather than as samples from previous buffer laps.
  if (timestamp > tx_end) {
    zero(std::max(tx_end, write_end - std::min<baseband_gateway_timestamp>(write_end, buffer_size)), timestamp);
  }

  unsigned write_index      = get_index(timestamp);
  unsigned write_length     = std::min(nof_requested_samples, buffer_size - write_index);
  unsigned remaining_length = nof_requested_samples - write_length;

  for (unsigned i_channel = 0, nof_channels = data.get_nof_channels(); i_channel != nof_channels; ++i_channel) {
    span<const ci16_t> src = data.get_channel_buffer(i_channel);
    span<ci16_t>       dst = buffer.get_writer().get_channel_buffer(i_channel);
    ocuduvec::copy(dst.subspan(write_index, write_length), src.first(write_length));
    ocuduvec::copy(dst.first(remaining_length), src.subspan(write_length, remaining_length));
  }

  // Publish the written samples to the receiver.
  tx_end_timestamp.store(write_end, std::memory_order_release);
  return true;
}

void loopback_buffer::set_first_expected_rx_timestamp(baseband_gateway_timestamp timestamp)
{
  report_fatal_error_if_not(tx_end_timestamp.load(std::memory_order_acquire) == 0,
                            "The first RX timestamp must be set before any sample is written to the buffer.");
  report_fatal_error_if_not(rx_end_timestamp.load(std::memory_order_acquire) == 0,
                            "The first RX timestamp must be set only once.");
  rx_end_timestamp.store(timestamp, std::memory_order_release);
}

unsigned loopback_buffer::read(baseband_gateway_buffer_writer& data, baseband_gateway_timestamp timestamp)
{
  unsigned nof_requested_samples = data.get_nof_samples();
  unsigned buffer_size           = buffer.get_nof_samples();
  ocudu_assert(nof_requested_samples <= buffer_size, "Buffer size is too small to hold the requested data.");
  ocudu_assert(data.get_nof_channels() == buffer.get_nof_channels(),
               "Number of channels in the data does not match the buffer.");

  // The RX end timestamp is only modified from this thread, a relaxed load is enough.
  baseband_gateway_timestamp rx_end = rx_end_timestamp.load(std::memory_order_relaxed);
  report_fatal_error_if_not(timestamp >= rx_end,
                            "Attempting to read samples in the past. Timestamp: {}, last read timestamp: {}",
                            timestamp,
                            rx_end);

  // The TX end timestamp never decreases. Samples available at the time of loading it remain available, and the writer
  // cannot overwrite them before the RX end timestamp advances.
  baseband_gateway_timestamp tx_end   = tx_end_timestamp.load(std::memory_order_acquire);
  baseband_gateway_timestamp read_end = timestamp + nof_requested_samples;
  unsigned                   available =
      (tx_end > timestamp) ? std::min<baseband_gateway_timestamp>(tx_end - timestamp, nof_requested_samples) : 0;
  if (available < nof_requested_samples) {
    logger.warning("Loopback: Attempting to read samples [{}, {}), ahead of the last written timestamp, i.e., {}",
                   timestamp,
                   read_end,
                   tx_end);
  }

  unsigned read_index       = get_index(timestamp);
  unsigned read_length      = std::min(available, buffer_size - read_index);
  unsigned remaining_length = available - read_length;

  for (unsigned i_channel = 0, nof_channels = data.get_nof_channels(); i_channel != nof_channels; ++i_channel) {
    span<const ci16_t> src = buffer.get_writer().get_channel_buffer(i_channel);
    span<ci16_t>       dst = data.get_channel_buffer(i_channel);
    ocuduvec::copy(dst.first(read_length), src.subspan(read_index, read_length));
    ocuduvec::copy(dst.subspan(read_length, remaining_length), src.first(remaining_length));
    // Samples not written yet are not received.
    ocuduvec::zero(dst.last(nof_requested_samples - available));
  }

  // Release the read samples to the writer. The reading position advances even if the samples were not available.
  rx_end_timestamp.store(read_end, std::memory_order_release);
  return available;
}
