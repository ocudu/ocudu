// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/support/shared_prach_buffer.h"

namespace ocudu {

struct prach_buffer_context;
struct resource_grid_context;
class shared_resource_grid;

/// \brief Describes the lower physical layer uplink request handler.
///
/// Handles incoming uplink requests and notifies their completion through the \c phy_rx_symbol_notifier interface.
class lower_phy_uplink_request_handler
{
public:
  /// Default destructor.
  virtual ~lower_phy_uplink_request_handler() = default;

  /// \brief Requests the lower PHY to capture a PRACH window.
  ///
  /// The lower PHY must capture a PHY window identified by \c context. The capture must start at slot \c context.slot
  /// and symbol \c context.start_symbol. The capture must finish once \c buffer.is_full() returns true.
  ///
  /// \param[in] context PRACH window context.
  /// \param[in] buffer  PRACH buffer used to write the PRACH window.
  virtual void request_prach_window(const prach_buffer_context& context, shared_prach_buffer buffer) = 0;

  /// \brief Requests the lower PHY to provide an uplink slot.
  ///
  /// The lower PHY must process the slot described by \c context and notify the demodulation per symbol basis of the
  /// requested slot.
  ///
  /// The notification contains the exact context and grid.
  ///
  /// \param[in] context Resource grid context.
  /// \param[in] grid    Resource grid to store the processed slot.
  virtual void request_uplink_slot(const resource_grid_context& context, const shared_resource_grid& grid) = 0;
};

} // namespace ocudu
