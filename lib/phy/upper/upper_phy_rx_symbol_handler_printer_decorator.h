// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "rx_resource_grid_printer_backend.h"
#include "ocudu/phy/support/prach_buffer_context.h"
#include "ocudu/phy/upper/upper_phy_rx_symbol_handler.h"
#include "ocudu/support/executors/task_worker.h"
#include <fstream>

namespace ocudu {

class upper_phy_rx_symbol_handler_printer_decorator : public upper_phy_rx_symbol_handler
{
public:
  upper_phy_rx_symbol_handler_printer_decorator(std::unique_ptr<upper_phy_rx_symbol_handler>      handler_,
                                                std::shared_ptr<rx_resource_grid_printer_backend> backend_) :
    handler(std::move(handler_)), backend(std::move(backend_))
  {
  }

  void
  handle_rx_symbol(const upper_phy_rx_symbol_context& context, const shared_resource_grid& grid, bool is_valid) override
  {
    // Handle PRACH buffer to the backend first for storing the buffer before any upper PHY result is notified.
    backend->handle_rx_symbol(context, grid);

    // Forward handle Rx symbol to the base handler.
    handler->handle_rx_symbol(context, grid, is_valid);
  }

  void handle_rx_prach_window(const prach_buffer_context& context, shared_prach_buffer buffer) override
  {
    // Store PRACH buffer in the backend for potential later writing.
    backend->handle_rx_prach_window(context, buffer.clone());

    handler->handle_rx_prach_window(context, buffer.clone());
  }

private:
  std::unique_ptr<upper_phy_rx_symbol_handler>      handler;
  std::shared_ptr<rx_resource_grid_printer_backend> backend;
};
} // namespace ocudu
