// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/support/shared_resource_grid.h"
#include "ocudu/phy/upper/upper_phy_rx_symbol_request_notifier.h"

namespace ocudu {

class upper_phy_rx_symbol_request_notifier_spy : public upper_phy_rx_symbol_request_notifier
{
  bool prach_capture_request_notified = false;

public:
  void on_prach_capture_request(const prach_buffer_context& context, shared_prach_buffer buffer) override
  {
    prach_capture_request_notified = true;
  }
  void on_uplink_slot_request(const resource_grid_context& context, const shared_resource_grid& grid) override {}

  bool has_prach_result_been_notified() const { return prach_capture_request_notified; }
};

} // namespace ocudu
