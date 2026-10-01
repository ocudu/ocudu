// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "uplink_request_processor_impl.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/phy/support/shared_resource_grid.h"
#include "ocudu/phy/upper/upper_phy_rx_symbol_request_notifier.h"
#include "ocudu/support/memory_pool/bounded_object_pool.h"

using namespace ocudu;

uplink_request_processor_impl::uplink_request_processor_impl(
    upper_phy_rx_symbol_request_notifier&       rx_symbol_request_notifier_,
    std::vector<std::unique_ptr<prach_buffer>>& prach_buffers,
    ocudulog::basic_logger&                     logger_) :
  rx_symbol_request_notifier(rx_symbol_request_notifier_), prach_pool(prach_buffers), logger(logger_)
{
}

void uplink_request_processor_impl::process_prach_request(const prach_buffer_context& context)
{
  // Grab a PRACH buffer from the pool.
  auto buffer = prach_pool.get();

  if (!buffer) {
    logger.warning("PRACH buffer pool depleted. Ignoring PRACH request");
    return;
  }

  // Notify the PRACH capture request event.
  rx_symbol_request_notifier.on_prach_capture_request(context, std::move(buffer));
}

void uplink_request_processor_impl::process_uplink_slot_request(const resource_grid_context& context,
                                                                const shared_resource_grid&  grid)
{
  rx_symbol_request_notifier.on_uplink_slot_request(context, grid);
}
