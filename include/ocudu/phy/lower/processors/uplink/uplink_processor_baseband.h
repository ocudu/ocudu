// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/complex.h"
#include "ocudu/adt/span.h"
#include "ocudu/gateways/baseband/baseband_gateway_timestamp.h"

namespace ocudu {

class baseband_gateway_buffer_reader;

/// \brief Lower physical layer uplink processor - Baseband interface.
///
/// Processes baseband samples. It derives the symbol and slot timing from the number of processed samples.
class uplink_processor_baseband
{
public:
  /// Default destructor.
  virtual ~uplink_processor_baseband() = default;

  /// \brief Processes any number of baseband samples.
  ///
  /// \param[in] buffer    Baseband samples to process.
  /// \param[in] timestamp Time instant in which the first sample was captured.
  /// \remark The number of channels in \c buffer must be equal to the number of receive ports for the sector.
  virtual void process(const baseband_gateway_buffer_reader& buffer, baseband_gateway_timestamp timestamp) = 0;
};

} // namespace ocudu
