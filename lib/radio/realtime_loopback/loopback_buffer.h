// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/gateways/baseband/buffer/baseband_gateway_buffer_dynamic.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/ocuduvec/zero.h"
#include "ocudu/radio/radio_session.h"
#include "ocudu/support/synchronization/stop_event.h"

namespace ocudu {

/// \brief Single-writer, single-reader circular sample buffer for the realtime loopback radio.
///
/// The sample with timestamp t is stored at index t mod N, where N is the buffer size. A write ending at timestamp e is
/// accepted only if e <= r + N, where r is the timestamp following the last read sample.
class loopback_buffer
{
public:
  /// \brief Creates a loopback buffer with the given number of channels and samples.
  ///
  /// \param[in] nof_channels Number of channels in the buffer.
  /// \param[in] nof_samples  Number of samples in the buffer.
  loopback_buffer(unsigned nof_channels, unsigned nof_samples) :
    buffer(nof_channels, nof_samples),
    tx_end_timestamp(0),
    rx_end_timestamp(0),
    logger(ocudulog::fetch_basic_logger("RF"))
  {
    for (unsigned i_channel = 0; i_channel != nof_channels; ++i_channel) {
      ocuduvec::zero(buffer.get_writer().get_channel_buffer(i_channel));
    }
  }

  /// \brief Writes samples to the buffer with a given timestamp.
  ///
  /// The samples between the end of the previous write and \c timestamp are set to zero.
  ///
  /// An assertion is thrown if either of the following conditions are met:
  ///   - The number of channels in \c data does not match the number of channels of the buffer.
  ///   - The number of samples in \c data is larger than the buffer size.
  ///
  /// A fatal error is reported if the timestamp is in the past, i.e., it overlaps with previously written samples.
  ///
  /// A warning is issued and the samples are dropped if they would overwrite samples that have not been read yet.
  ///
  /// \param[in] data      Buffer containing the samples to write.
  /// \param[in] timestamp Timestamp of the first sample in the data.
  /// \return \c true if the samples are written, \c false if they are dropped.
  bool write(const baseband_gateway_buffer_reader& data, baseband_gateway_timestamp timestamp);

  /// \brief Reads samples from the buffer with a given timestamp.
  ///
  /// An assertion is thrown if either of the following conditions are met:
  ///   - The number of channels in \c data does not match the number of channels of the buffer.
  ///   - The number of samples in \c data is larger than the buffer size.
  ///
  /// A fatal error is reported if the timestamp is in the past, i.e., it overlaps with previously read samples.
  ///
  /// The samples that have not been written to the buffer yet are set to zero and a warning is issued.
  ///
  /// \param[out] data      Buffer to fill with the read samples.
  /// \param[in]  timestamp Timestamp of the first sample to read.
  /// \return The number of samples read from the buffer, the rest of \c data is set to zero.
  unsigned read(baseband_gateway_buffer_writer& data, baseband_gateway_timestamp timestamp);

  /// \brief Sets the timestamp of the first sample to read.
  ///
  /// Called once, before any write or read.
  void set_first_expected_rx_timestamp(baseband_gateway_timestamp timestamp);

private:
  /// Converts any given timestamp into a circular buffer index.
  unsigned get_index(baseband_gateway_timestamp timestamp) const { return timestamp % buffer.get_nof_samples(); }

  /// Sets to zero the buffer samples in the timestamp range [\c begin, \c end).
  void zero(baseband_gateway_timestamp begin, baseband_gateway_timestamp end);

  /// Internal sample buffer.
  baseband_gateway_buffer_dynamic buffer;

  /// Timestamp following the last sample written into the buffer. Zero if no sample has been written.
  std::atomic<baseband_gateway_timestamp> tx_end_timestamp;
  /// Timestamp following the last sample read from the buffer.
  std::atomic<baseband_gateway_timestamp> rx_end_timestamp;

  /// Radio session logger.
  ocudulog::basic_logger& logger;
};

} // namespace ocudu
