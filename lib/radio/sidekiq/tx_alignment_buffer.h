// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/gateways/baseband/baseband_gateway_timestamp.h"
#include "ocudu/gateways/baseband/buffer/baseband_gateway_buffer_dynamic.h"
#include "ocudu/gateways/baseband/buffer/baseband_gateway_buffer_reader_view.h"
#include "ocudu/gateways/baseband/buffer/baseband_gateway_buffer_writer_view.h"
#include "ocudu/ocuduvec/copy.h"
#include "ocudu/ocuduvec/zero.h"

namespace ocudu {
namespace detail {

/// Buffer used to align TX samples from the Sidekiq RX blocks into the receive buffers.
class tx_alignment_buffer
{
public:
  tx_alignment_buffer() = delete;

  /// Constructs an alignment buffer from a number of ports and an initial number of samples.
  tx_alignment_buffer(unsigned nof_ports_, unsigned nof_samples) :
    buffer(nof_ports_, nof_samples), nof_ports(nof_ports_), empty(true), writing_index(0)
  {
  }

  /// Clears the alignment buffer, allowing it to be written.
  void clear()
  {
    // Mark buffer as empty.
    empty = true;
  }

  /// Resizes the alignment buffer into a new number of samples per port.
  void resize(unsigned nof_samples)
  {
    buffer.resize(nof_samples);

    // Invalidate buffer contents.
    empty = true;
  }

  /// \brief Provides read access into the alignment buffer.
  ///
  /// This method is used to read the contents of the alignment buffer. An assertion is thrown if the buffer has not
  /// been fully written.
  ///
  /// \param[out] out_timestamp Timestamp of the first sample in the buffer.
  /// \return A read-only view into the alignment buffer.
  const baseband_gateway_buffer_reader& read(baseband_gateway_timestamp& out_timestamp) const
  {
    ocudu_assert(is_full(), "Attempting to read from non-full buffer.");

    out_timestamp = buffer_start_timestamp;

    return buffer.get_reader();
  }

  /// \brief Appends samples into the alignment buffer.
  ///
  /// The samples in \c input are copied into the alignment buffer for all ports. If the size of the alignment buffer is
  /// smaller than the input buffer, samples are copied until the alignment buffer is full. If the size of the alignment
  /// buffer is larger than the input buffer, all samples are copied into the alignment buffer, and subsequent calls to
  /// this method will write into the remaining alignment buffer samples until the buffer is full. If the timestamp of
  /// the input samples does not align with the timestamp of previously written samples, zeros are added to fill the
  /// gap. If the timestamp of the input samples is in the past, the samples are not written into the buffer.
  ///
  /// \param[in] input     Samples to write into the alignment buffer.
  /// \param[in] timestamp Timestamp of the input samples.
  /// \return The number of written samples from \c input.
  unsigned append(const baseband_gateway_buffer_reader& input, baseband_gateway_timestamp timestamp)
  {
    ocudu_assert(input.get_nof_samples() > 0, "Input size cannot be zero.");
    ocudu_assert(input.get_nof_channels() == nof_ports,
                 "The number of input ports (i.e., {}) does not match the number of buffer ports (i.e. {}).",
                 input.get_nof_channels(),
                 nof_ports);
    ocudu_assert(!is_full(), "Attempting to append samples to a full buffer.");

    // If the buffer is flagged as empty, overwrite its contents with new writes.
    if (is_empty()) {
      buffer_start_timestamp = timestamp;
      empty                  = false;
      writing_index          = 0;
    }

    // Do not append samples in the past.
    if (timestamp < buffer_start_timestamp + writing_index) {
      return 0;
    }

    // Check if the input data is continuous in time to the stored data.
    bool continuous_data = buffer_start_timestamp + writing_index == timestamp;

    // If it is not, add zeros to the buffer samples in the discontinuity jump.
    if (!continuous_data) {
      // Determine the number of samples to add padding to.
      unsigned padding_count = timestamp - buffer_start_timestamp + writing_index;
      padding_count          = std::min(padding_count, buffer.get_nof_samples() - writing_index);

      // Add zeros and increment writing index.
      baseband_gateway_buffer_writer_view padding_writer(buffer.get_writer(), writing_index, padding_count);
      for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
        ocuduvec::zero(padding_writer.get_channel_buffer(i_port));
      }
      writing_index += padding_count;
    }

    // At this point, the writing index is set to the timestamp of the input samples. Write as many samples as possible.
    unsigned write_count = std::min(buffer.get_nof_samples() - writing_index, input.get_nof_samples());
    baseband_gateway_buffer_writer_view writer(buffer.get_writer(), writing_index, write_count);
    baseband_gateway_buffer_reader_view reader(input, 0, write_count);
    for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
      ocuduvec::copy(writer.get_channel_buffer(i_port), reader.get_channel_buffer(i_port));
    }
    writing_index += write_count;

    // Return the number of written samples from the input.
    return write_count;
  }

  /// Returns \c true if the buffer is empty, \c false otherwise.
  bool is_empty() const { return empty; }

  /// Returns \c true if the buffer is full, \c false otherwise.
  bool is_full() const { return !empty && (buffer.get_nof_samples() == writing_index); }

private:
  /// Storage of baseband samples.
  baseband_gateway_buffer_dynamic buffer;
  /// Timestamp of the first baseband sample stored in the alignment buffer.
  baseband_gateway_timestamp buffer_start_timestamp;
  /// Number of ports of the buffer.
  unsigned nof_ports;
  /// Indicates if the buffer contents are outdated and can be overwritten.
  bool empty;
  /// Buffer writing position.
  unsigned writing_index;
};

} // namespace detail

} // namespace ocudu
