// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/lower/lower_phy_timing_notifier.h"
#include "ocudu/phy/support/shared_resource_grid.h"
#include "ocudu/phy/upper/upper_phy_rx_symbol_handler.h"
#include <mutex>

namespace ocudu {

class rx_symbol_handler_example : public upper_phy_rx_symbol_handler
{
private:
  ocudulog::basic_logger& logger;
  std::mutex              mutex;

public:
  rx_symbol_handler_example(ocudulog::basic_levels log_level) : logger(ocudulog::fetch_basic_logger("RxSyHan"))
  {
    logger.set_level(log_level);
  }

  void
  handle_rx_symbol(const upper_phy_rx_symbol_context& context, const shared_resource_grid& grid, bool is_valid) override
  {
    std::unique_lock<std::mutex> lock(mutex);
    logger.debug(context.slot.sfn(),
                 context.slot.slot_index(),
                 "Rx symbol {} received for sector {}",
                 context.symbol,
                 context.sector);
  }

  void handle_rx_prach_window(const prach_buffer_context& context, shared_prach_buffer buffer) override
  {
    std::unique_lock<std::mutex> lock(mutex);
    logger.debug(context.slot.sfn(),
                 context.slot.slot_index(),
                 "PRACH symbol {} received for sector {}",
                 context.start_symbol,
                 context.sector);
  }
};

} // namespace ocudu
