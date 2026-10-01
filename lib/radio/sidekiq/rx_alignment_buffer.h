// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/adt/bounded_bitset.h"
#include "ocudu/gateways/baseband/baseband_gateway_timestamp.h"
#include "ocudu/gateways/baseband/buffer/baseband_gateway_buffer_dynamic.h"
#include "ocudu/ocuduvec/copy.h"
#include <optional>

namespace ocudu {
namespace detail {

/// Buffer used to align RX samples from the Sidekiq RX blocks into the receive buffers.
class rx_alignment_buffer
{
public:
  rx_alignment_buffer() = delete;

  /// Constructs an alignment buffer from a number of ports and an initial number of samples.
  rx_alignment_buffer(unsigned nof_ports_, unsigned nof_samples) :
    buffer(nof_ports_, nof_samples), nof_ports(nof_ports_), empty_ports(nof_ports_)
  {
    clear();
  }

  /// Clears the alignment buffer, allowing it to be written.
  void clear()
  {
    // Mark ports as empty.
    empty_ports.fill(0, empty_ports.size(), true);

    // Reset timetamp.
    buffer_start_timestamp.reset();
    reading_index = 0;
  }

  /// Resizes the alignment buffer into a new number of samples per port.
  void resize(unsigned nof_samples)
  {
    buffer.resize(nof_samples);

    // Invalidate buffer contents.
    clear();
  }

  /// \brief Provides write access into the alignment buffer for a specific port.
  ///
  /// This method is used to fill the alignment buffer once a RX block is received. The implementation assumes that
  /// after calling this method, the contents of the alignment buffer for the specified port are fully written, and will
  /// be available for future reads once all ports have been written. An assertion is thrown if the input timestamp does
  /// not match the timestamp used when writing into a different port, or if the contents of the buffer have not been
  /// read or cleared.
  ///
  /// \param[in] i_port Port index for the view.
  /// \param[in] in_timestamp Timestamp of the first sample to write.
  /// \return A read-write view into the alignment buffer for the requested port.
  span<ci16_t> write_port(unsigned i_port, baseband_gateway_timestamp in_timestamp)
  {
    ocudu_assert(i_port < nof_ports, "The requested port exceeds the number of ports.");
    ocudu_assert(empty_ports.test(i_port), "Attempting to write to non-empty buffer.");

    empty_ports.set(i_port, false);

    if (!buffer_start_timestamp.has_value()) {
      buffer_start_timestamp = in_timestamp;
    }

    ocudu_assert(buffer_start_timestamp == in_timestamp,
                 "Input timestamp does not match previously set buffer timestamp.");

    return buffer.get_writer().get_channel_buffer(i_port);
  }

  /// \brief Reads the contents of the alignment buffer.
  ///
  /// The samples stored in the alignment buffer are copied into \c output for all ports. The number of copied samples
  /// is limited by the size of the alignment buffer and the size of the output buffer. If the size of the alignment
  /// buffer is smaller than the size of the output buffer, all samples in the alignment buffer will be copied, and the
  /// output will be partially full. If the size of the alignment buffer is larger than the size of the output buffer,
  /// samples will be copied until the output buffer is full. If this method is called again after a partial read, the
  /// output buffer will be filled with the remaining samples. An assertion will be triggered if this method is called
  /// after all samples have been read from the buffer.
  ///
  /// \param[out] output Output buffer where the samples are copied.
  /// \param[out] out_timestamp Timestamp of the first sample copied into the output buffer.
  /// \return The number of read samples.
  unsigned read(baseband_gateway_buffer_writer& output, baseband_gateway_timestamp& out_timestamp)
  {
    ocudu_assert(output.get_nof_samples() > 0, "Output size cannot be zero.");
    ocudu_assert(output.get_nof_channels() == nof_ports,
                 "The number of output ports (i.e., {}) does not match the number of buffer ports (i.e. {}).",
                 output.get_nof_channels(),
                 nof_ports);
    ocudu_assert(!is_empty(), "Attempting to read samples from an empty buffer.");
    ocudu_assert(reading_index < buffer.get_nof_samples(), "All samples have already been read from the buffer");

    // Select the minimum among the samples to process and the number of stored samples that have not been read.
    unsigned nof_available_samples = buffer.get_nof_samples() - reading_index;
    unsigned count                 = std::min(output.get_nof_samples(), nof_available_samples);

    // For each port, concatenate samples.
    for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
      // Select view of the temporary buffer.
      span<const ci16_t> temp_buffer_src = buffer[i_port].subspan(reading_index, count);

      // Select view of the output samples.
      span<ci16_t> temp_buffer_dst = output.get_channel_buffer(i_port).first(count);

      ocuduvec::copy(temp_buffer_dst, temp_buffer_src);
    }

    out_timestamp = buffer_start_timestamp.value() + static_cast<baseband_gateway_timestamp>(reading_index);

    reading_index += count;

    // Clear the buffer if all available samples have been read.
    if (count == nof_available_samples) {
      clear();
    }

    // Return the number of read samples.
    return count;
  }

  /// Returns \c true if the buffer is empty, \c false otherwise.
  bool is_empty() const { return empty_ports.all(); }

private:
  /// Storage of baseband samples.
  baseband_gateway_buffer_dynamic buffer;
  /// Timestamp of the first baseband sample stored in the alignment buffer.
  std::optional<baseband_gateway_timestamp> buffer_start_timestamp;
  /// Number of ports of the buffer.
  unsigned nof_ports;
  /// Indicates if the buffer contents are outdated and can be overwritten.
  bounded_bitset<64> empty_ports;
  /// Buffer reading position.
  unsigned reading_index;
};

} // namespace detail

} // namespace ocudu
