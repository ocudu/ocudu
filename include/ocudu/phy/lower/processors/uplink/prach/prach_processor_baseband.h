// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/complex.h"
#include "ocudu/adt/span.h"
#include "ocudu/ran/slot_point.h"

namespace ocudu {

class baseband_gateway_buffer_reader;

/// \brief Lower physical layer PRACH processor - Baseband interface.
///
/// Processes baseband samples with OFDM symbol granularity. The OFDM symbol size is inferred from the slot numerology.
class prach_processor_baseband
{
public:
  /// Default destructor.
  virtual ~prach_processor_baseband() = default;

  /// Collects the parameters that describe a symbol context.
  struct symbol_context {
    /// Current slot.
    slot_point slot;
    /// Current symbol index within the slot.
    unsigned symbol;
    /// Sector identifier.
    unsigned sector;
  };

  /// \brief Processes a baseband OFDM symbol.
  ///
  /// \param[in] samples Baseband samples to process.
  /// \param[in] context OFDM Symbol context.
  virtual void process_symbol(const baseband_gateway_buffer_reader& samples, const symbol_context& context) = 0;
};

} // namespace ocudu
