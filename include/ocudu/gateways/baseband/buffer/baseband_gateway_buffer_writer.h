// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/adt/complex.h"
#include "ocudu/adt/span.h"

namespace ocudu {

/// \brief Read-write baseband buffer interface.
///
/// It comprises get_nof_channels() channels of get_nof_samples() samples each.
class baseband_gateway_buffer_writer
{
public:
  virtual ~baseband_gateway_buffer_writer() = default;

  /// Gets the number of channels.
  virtual unsigned get_nof_channels() const = 0;

  /// Gets the number of samples.
  virtual unsigned get_nof_samples() const = 0;

  /// \brief Gets a channel buffer.
  /// \param[in] channel_idx Indicates the channel index.
  /// \return A view to a baseband buffer.
  /// \note An invalid channel index causes undefined behaviour.
  virtual span<ci16_t> get_channel_buffer(unsigned channel_idx) = 0;

  /// See get_channel_buffer() documentation.
  span<ci16_t> operator[](unsigned channel_index) { return get_channel_buffer(channel_index); }
};

} // namespace ocudu
