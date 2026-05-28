// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/gateways/baseband/buffer/baseband_gateway_buffer_dynamic.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/ocuduvec/zero.h"
#include "ocudu/radio/radio_session.h"
#include "ocudu/support/synchronization/stop_event.h"

namespace ocudu {

/// \brief Sample loopback buffer for the realtime loopback radio.
///
/// It allows writing samples with a given timestamp and reading them later on. It is implemented as a circular buffer.
class loopback_buffer
{
public:
  /// Forbid default constructor.
  loopback_buffer() = delete;

  /// \brief Creates a loopback buffer with the given number of channels and samples.
  ///
  /// \param[in] nof_channels Number of channels in the buffer.
  /// \param[in] nof_samples  Number of samples in the buffer.
  loopback_buffer(unsigned nof_channels, unsigned nof_samples) :
    buffer(nof_channels, nof_samples),
    last_tx_timestamp(0),
    last_rx_timestamp(0),
    logger(ocudulog::fetch_basic_logger("LOOPBACK"))
  {
    for (unsigned i_channel = 0; i_channel != nof_channels; ++i_channel) {
      ocuduvec::zero(buffer.get_writer().get_channel_buffer(i_channel));
    }
  }

  /// \brief Writes samples to the buffer with a given timestamp.
  ///
  /// An assertion is thrown if either of the following conditions are met:
  ///   - The number of channels in \c data does not match the number of channels of the buffer.
  ///   - The number of samples in \c data is larger than the buffer size.
  ///   - The timestamp is in the past, i.e., smaller than or equal to the timestamp of the last written sample.
  ///   - The timestamp is too far in the future, i.e., it would overwrite unread samples in the buffer.
  ///
  /// \param[in] data      Buffer containing the samples to write.
  /// \param[in] timestamp Timestamp of the first sample in the data.
  void write(const baseband_gateway_buffer_reader& data, baseband_gateway_timestamp timestamp);

  /// \brief Reads samples from the buffer with a given timestamp.
  ///
  /// An assertion is thrown if either of the following conditions are met:
  ///   - The number of channels in \c data does not match the number of channels of the buffer.
  ///   - The number of samples in \c data is larger than the buffer size.
  ///   - The timestamp is in the past, i.e., smaller than the timestamp of the last read sample.
  ///
  /// A warning is issued and reading from the buffer is skipped if the requested timestamp is too far in the future,
  /// i.e., if it would read samples that have not been written to the buffer yet.
  ///
  /// \param[out] data      Buffer to fill with the read samples.
  /// \param[in]  timestamp Timestamp of the first sample to read.
  void read(baseband_gateway_buffer_writer& data, baseband_gateway_timestamp timestamp);

  /// \brief Sets the internal buffer state to expect a read requested with the given timestamp.
  ///
  /// It must be called once before writing and reading from the buffer, and must not be called again.
  void set_first_expected_rx_timestamp(baseband_gateway_timestamp timestamp);

private:
  /// Converts any given timestamp into a circular buffer index.
  unsigned get_index(baseband_gateway_timestamp timestamp) { return timestamp % buffer.get_nof_samples(); }

  /// Internal sample buffer.
  baseband_gateway_buffer_dynamic buffer;

  /// Last transmit timestamp. It is set to the last sample written into the buffer.
  std::atomic<baseband_gateway_timestamp> last_tx_timestamp;
  /// Last receive timestamp. It is set to the last sample read from the buffer.
  std::atomic<baseband_gateway_timestamp> last_rx_timestamp;

  /// Radio session logger.
  ocudulog::basic_logger& logger;
};

} // namespace ocudu
