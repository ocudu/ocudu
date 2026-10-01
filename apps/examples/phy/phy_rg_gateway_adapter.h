// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/lower/lower_phy_downlink_handler.h"
#include "ocudu/phy/support/shared_resource_grid.h"
#include "ocudu/phy/upper/upper_phy_rg_gateway.h"

namespace ocudu {

/// Implements a generic physical layer resource grid gateway adapter.
class phy_rg_gateway_adapter : public upper_phy_rg_gateway
{
private:
  lower_phy_downlink_handler* rg_handler = nullptr;

public:
  /// Connects the adaptor to the lower physical layer gateway.
  void connect(lower_phy_downlink_handler* handler) { rg_handler = handler; }

  // See interface for documentation.
  void send(const resource_grid_context& context, shared_resource_grid grid) override
  {
    report_fatal_error_if_not(rg_handler, "Adapter is not connected.");
    rg_handler->handle_resource_grid(context, grid);
  }
};

} // namespace ocudu
