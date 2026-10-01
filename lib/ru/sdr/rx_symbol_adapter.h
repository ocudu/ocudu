// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/lower/lower_phy_rx_symbol_context.h"
#include "ocudu/phy/lower/lower_phy_rx_symbol_notifier.h"
#include "ocudu/ru/ru_uplink_plane.h"

namespace ocudu {

/// Implements a lower physical layer to Radio Unit receive symbol adapter.
class ru_lower_phy_rx_symbol_adapter : public lower_phy_rx_symbol_notifier
{
public:
  explicit ru_lower_phy_rx_symbol_adapter(ru_uplink_plane_rx_symbol_notifier& rx_symbol_handler_) :
    rx_symbol_handler(rx_symbol_handler_)
  {
  }

  // See interface for documentation.
  void
  on_rx_symbol(const lower_phy_rx_symbol_context& context, const shared_resource_grid& grid, bool is_valid) override
  {
    rx_symbol_handler.on_new_uplink_symbol(
        ru_uplink_rx_symbol_context{.slot = context.slot, .sector = context.sector, .symbol_id = context.nof_symbols},
        grid,
        is_valid);
  }

  // See interface for documentation.
  void on_rx_prach_window(const prach_buffer_context& context, shared_prach_buffer buffer) override
  {
    rx_symbol_handler.on_new_prach_window_data(context, std::move(buffer));
  }

private:
  ru_uplink_plane_rx_symbol_notifier& rx_symbol_handler;
};

} // namespace ocudu
