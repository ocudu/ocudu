// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief Interface of the upper-PHY handler in charge of uplink OFDM symbols.

#pragma once

#include "ocudu/phy/support/shared_prach_buffer.h"
#include "ocudu/ran/slot_point.h"

namespace ocudu {

struct prach_buffer_context;
class shared_resource_grid;

/// Describes the context of a newly received symbol.
struct upper_phy_rx_symbol_context {
  /// Identifier of the sector the symbol is received from.
  unsigned sector;
  /// Describes the current slot.
  slot_point slot;
  /// Symbol index within the slot.
  unsigned symbol;
};

/// \brief Interface of the upper-PHY handler in charge of processing uplink OFDM symbols.
class upper_phy_rx_symbol_handler
{
public:
  /// Default destructor.
  virtual ~upper_phy_rx_symbol_handler() = default;

  /// \brief Handles the reception of an OFDM symbol.
  /// \param[in] context  Notification context: specifies sector, slot and symbol.
  /// \param[in] grid     Resource grid for the current slot (encompasses all receive antenna ports).
  /// \param[in] is_valid Set it to true if the resource grid data contains valid information. Otherwise, set it to
  /// false.
  virtual void
  handle_rx_symbol(const upper_phy_rx_symbol_context& context, const shared_resource_grid& grid, bool is_valid) = 0;

  /// \brief Handles the arrival of PRACH sequences.
  /// \param[in] context PRACH context: specifies sector, slot and window.
  /// \param[in] buffer  Read-only buffer containing the PRACH sequence.
  virtual void handle_rx_prach_window(const prach_buffer_context& context, shared_prach_buffer buffer) = 0;
};

} // namespace ocudu
