// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/adt/complex.h"
#include "ocudu/adt/span.h"
#include "ocudu/gateways/baseband/buffer/baseband_gateway_buffer_reader.h"
#include "ocudu/support/ocudu_assert.h"

namespace ocudu {

/// Implements a baseband gateway buffer reader view based on another reader.
class baseband_gateway_buffer_reader_view : public baseband_gateway_buffer_reader
{
public:
  /// \brief Constructs a baseband buffer reader view from a baseband buffer reader for a given offset and a size.
  /// \param[in] buffer_      Buffer that contains the samples.
  /// \param[in] offset_      Offset from the beginning of the buffer.
  /// \param[in] nof_samples_ Number of samples.
  baseband_gateway_buffer_reader_view(const baseband_gateway_buffer_reader& buffer_,
                                      unsigned                              offset_,
                                      unsigned                              nof_samples_) :
    buffer(buffer_), offset(offset_), nof_samples(nof_samples_)
  {
    ocudu_assert(buffer.get().get_nof_samples() >= offset + nof_samples,
                 "The offset (i.e., {}) plus the number of samples (i.e., {}) exceed the buffer size (i.e., {}).",
                 offset,
                 nof_samples,
                 buffer.get().get_nof_samples());
  }

  /// Default copy constructor.
  baseband_gateway_buffer_reader_view(const baseband_gateway_buffer_reader_view&) = default;

  /// Default move constructor.
  baseband_gateway_buffer_reader_view(baseband_gateway_buffer_reader_view&&) = default;

  /// Default copy assign operator.
  baseband_gateway_buffer_reader_view& operator=(const baseband_gateway_buffer_reader_view&) = default;

  /// Default move assign operator.
  baseband_gateway_buffer_reader_view& operator=(baseband_gateway_buffer_reader_view&&) = default;

  // See interface for documentation.
  unsigned get_nof_channels() const override { return buffer.get().get_nof_channels(); }

  // See interface for documentation.
  unsigned get_nof_samples() const override { return nof_samples; }

  // See interface for documentation.
  span<const ci16_t> get_channel_buffer(unsigned i_channel) const override
  {
    return buffer.get().get_channel_buffer(i_channel).subspan(offset, nof_samples);
  }

private:
  std::reference_wrapper<const baseband_gateway_buffer_reader> buffer;
  unsigned                                                     offset;
  unsigned                                                     nof_samples;
};

} // namespace ocudu
