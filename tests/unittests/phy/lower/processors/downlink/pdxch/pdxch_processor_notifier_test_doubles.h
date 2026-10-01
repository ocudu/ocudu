// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/lower/lower_phy_rx_symbol_context.h"
#include "ocudu/phy/lower/processors/downlink/pdxch/pdxch_processor_notifier.h"
#include "ocudu/phy/support/resource_grid_context.h"

namespace ocudu {

class pdxch_processor_notifier_spy : public pdxch_processor_notifier
{
public:
  struct rx_symbol_entry {
    const resource_grid_reader* grid;
    lower_phy_rx_symbol_context context;
  };

  void on_pdxch_request_late(const resource_grid_context& context) override { request_late.emplace_back(context); }

  const std::vector<resource_grid_context>& get_request_late() const { return request_late; }

  unsigned get_nof_notifications() const { return request_late.size(); }

  void clear_notifications() { request_late.clear(); }

private:
  std::vector<resource_grid_context> request_late;
};

} // namespace ocudu
