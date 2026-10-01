// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/gateways/baseband/baseband_gateway_base.h"
#include "ocudu/gateways/baseband/baseband_gateway_timestamp.h"

namespace ocudu {

class baseband_gateway_buffer_writer;

/// Baseband gateway - reception interface.
class baseband_gateway_receiver : public baseband_gateway_base
{
public:
  /// Receiver metadata.
  struct metadata {
    /// Timestamp of the received baseband signal.
    baseband_gateway_timestamp ts;
  };

  /// \brief Receives a number of baseband samples.
  /// \param[out,in] data Buffer of baseband samples.
  /// \return Receiver metadata.
  /// \note The \c data buffer provides the number of samples to receive through \ref
  ///       baseband_gateway_buffer::get_nof_samples.
  /// \note The \c data buffer must have the same number of channels as the stream.
  virtual metadata receive(baseband_gateway_buffer_writer& data) = 0;
};

} // namespace ocudu
