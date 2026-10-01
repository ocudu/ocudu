// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/support/shared_resource_grid.h"
#include "ocudu/phy/upper/uplink_slot_processor.h"
#include "ocudu/phy/upper/upper_phy_rx_symbol_handler.h"

namespace ocudu {

/// \brief Implementation of the upper PHY handler of receive symbols events.
///
/// In this implementation, handling the newly received symbol event takes the following steps.
///     1. Get an uplink processor from the pool.
///     2. Call the corresponding method from the uplink processor based on the type of the received symbol (PRACH, SRS,
///        PUSCH).
class upper_phy_rx_symbol_handler_impl : public upper_phy_rx_symbol_handler
{
public:
  upper_phy_rx_symbol_handler_impl(uplink_slot_processor_pool& ul_processor_pool_);

  // See interface for documentation.
  void handle_rx_symbol(const upper_phy_rx_symbol_context& context,
                        const shared_resource_grid&        grid,
                        bool                               is_valid) override;

  // See interface for documentation.
  void handle_rx_prach_window(const prach_buffer_context& context, shared_prach_buffer buffer) override;

private:
  /// Uplink processor pool.
  uplink_slot_processor_pool& ul_processor_pool;
};

} // namespace ocudu
