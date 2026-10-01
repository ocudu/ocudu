// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/lower/lower_phy_rx_symbol_context.h"
#include "ocudu/phy/lower/lower_phy_rx_symbol_notifier.h"
#include "ocudu/phy/upper/upper_phy_rx_symbol_handler.h"

namespace ocudu {

/// Implements a generic physical layer receive symbol adapter.
class phy_rx_symbol_adapter : public lower_phy_rx_symbol_notifier
{
private:
  upper_phy_rx_symbol_handler* rx_symbol_handler = nullptr;

public:
  /// Connects the adaptor to the upper physical layer handler.
  void connect(upper_phy_rx_symbol_handler* upper_handler) { rx_symbol_handler = upper_handler; }

  // See interface for documentation.
  void
  on_rx_symbol(const lower_phy_rx_symbol_context& context, const shared_resource_grid& grid, bool is_valid) override
  {
    report_fatal_error_if_not(rx_symbol_handler, "Adapter is not connected.");
    upper_phy_rx_symbol_context upper_context;
    upper_context.slot   = context.slot;
    upper_context.sector = context.sector;
    upper_context.symbol = context.nof_symbols;
    rx_symbol_handler->handle_rx_symbol(upper_context, grid, is_valid);
  }

  // See interface for documentation.
  void on_rx_prach_window(const prach_buffer_context& context, shared_prach_buffer buffer) override
  {
    report_fatal_error_if_not(rx_symbol_handler, "Adapter is not connected.");
    rx_symbol_handler->handle_rx_prach_window(context, std::move(buffer));
  }
};

} // namespace ocudu
