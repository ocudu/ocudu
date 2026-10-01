// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "processor_handler_adaptor.h"
#include "ocudu/phy/support/shared_resource_grid.h"

using namespace ocudu;

void processor_handler_adaptor::downlink_handler_adaptor ::handle_resource_grid(const resource_grid_context& context,
                                                                                const shared_resource_grid&  grid)
{
  pdxch_handler.handle_request(grid, context);
}

void processor_handler_adaptor::uplink_request_handler_adaptor::request_prach_window(
    const prach_buffer_context& context,
    shared_prach_buffer         buffer)
{
  prach_handler.handle_request(std::move(buffer), context);
}

void processor_handler_adaptor::uplink_request_handler_adaptor::request_uplink_slot(
    const resource_grid_context& context,
    const shared_resource_grid&  grid)
{
  puxch_handler.handle_request(grid, context);
}
