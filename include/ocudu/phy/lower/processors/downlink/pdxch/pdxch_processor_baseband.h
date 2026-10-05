// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/gateways/baseband/buffer/baseband_gateway_buffer_pool.h"
#include "ocudu/ran/slot_point.h"

namespace ocudu {

/// \brief Lower physical layer PDxCH processor - Baseband interface.
///
/// Processes baseband samples with slot granularity.
class pdxch_processor_baseband
{
public:
  /// Default destructor.
  virtual ~pdxch_processor_baseband() = default;

  /// Describes the context of a newly generated slot.
  struct slot_context {
    /// Slot context.
    slot_point slot;
    /// Radio sector identifier.
    unsigned sector;
  };

  /// \brief Processes a baseband OFDM slot.
  ///
  /// \param[in] context OFDM Symbol context.
  /// \return Slot downlink baseband buffer.
  virtual baseband_gateway_buffer_ptr process_slot(slot_context context) = 0;
};

} // namespace ocudu
