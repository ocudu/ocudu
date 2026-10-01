// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/lower/lower_phy_rx_symbol_context.h"
#include "ocudu/phy/lower/processors/uplink/puxch/puxch_processor_notifier.h"
#include "ocudu/phy/support/resource_grid_context.h"
#include "ocudu/phy/support/shared_resource_grid.h"

namespace ocudu {

class puxch_processor_notifier_spy : public puxch_processor_notifier
{
public:
  struct rx_symbol_entry {
    const resource_grid_reader* grid;
    lower_phy_rx_symbol_context context;
  };

  void on_puxch_request_late(const resource_grid_context& context) override { request_late.emplace_back(context); }

  void
  on_rx_symbol(const shared_resource_grid& grid, const lower_phy_rx_symbol_context& context, bool is_valid) override
  {
    rx_symbol_entry entry = rx_symbol_entry{&grid.get_reader(), context};
    rx_symbol.emplace_back(entry);
  }

  const std::vector<resource_grid_context>& get_request_late() const { return request_late; }

  const std::vector<rx_symbol_entry>& get_rx_symbol() const { return rx_symbol; }

  unsigned get_nof_notifications() const { return request_late.size() + rx_symbol.size(); }

  void clear_notifications()
  {
    request_late.clear();
    rx_symbol.clear();
  }

private:
  std::vector<resource_grid_context> request_late;
  std::vector<rx_symbol_entry>       rx_symbol;
};

} // namespace ocudu
