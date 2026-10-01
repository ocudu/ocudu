// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/gateways/baseband/baseband_gateway_base.h"
#include "ocudu/gateways/baseband/baseband_gateway_transmitter_metadata.h"

namespace ocudu {

class baseband_gateway_buffer_reader;

/// Baseband gateway - transmission interface.
class baseband_gateway_transmitter : public baseband_gateway_base
{
public:
  /// \brief Transmits a set of baseband samples at the time instant provided in the metadata.
  /// \param[in] data     Buffer of baseband samples to transmit.
  /// \param[in] metadata Additional parameters for transmission.
  /// \remark The data buffers must have the same number of channels as the stream.
  virtual void transmit(const baseband_gateway_buffer_reader&        data,
                        const baseband_gateway_transmitter_metadata& metadata) = 0;
};

} // namespace ocudu
