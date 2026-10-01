// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/lower/lower_phy_uplink_request_handler.h"
#include "ocudu/phy/upper/upper_phy_rx_symbol_request_notifier.h"
#include "ocudu/support/error_handling.h"

namespace ocudu {

/// Implements a generic physical layer adapter for symbol capture event requests.
class phy_rx_symbol_request_adapter : public upper_phy_rx_symbol_request_notifier
{
  lower_phy_uplink_request_handler* rx_symbol_request_handler = nullptr;

public:
  /// Connects the adaptor to the lower physical layer handler.
  void connect(lower_phy_uplink_request_handler* lower_handler) { rx_symbol_request_handler = lower_handler; }

  // See interface for documentation.
  void on_prach_capture_request(const prach_buffer_context& context, shared_prach_buffer buffer) override
  {
    report_fatal_error_if_not(rx_symbol_request_handler, "Adapter is not connected.");
    rx_symbol_request_handler->request_prach_window(context, std::move(buffer));
  }

  // See interface for documentation.
  void on_uplink_slot_request(const resource_grid_context& context, const shared_resource_grid& grid) override
  {
    report_fatal_error_if_not(rx_symbol_request_handler, "Adapter is not connected.");

    rx_symbol_request_handler->request_uplink_slot(context, grid);
  }
};

} // namespace ocudu
