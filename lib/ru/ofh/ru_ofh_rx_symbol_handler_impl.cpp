// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ru_ofh_rx_symbol_handler_impl.h"
#include "ocudu/phy/support/shared_resource_grid.h"

using namespace ocudu;

void ru_ofh_rx_symbol_handler_impl::on_new_uplink_symbol(const ofh::uplane_rx_symbol_context& context,
                                                         shared_resource_grid                 grid,
                                                         bool                                 is_valid)
{
  ru_uplink_rx_symbol_context ru_context;
  ru_context.sector    = context.sector;
  ru_context.slot      = context.slot;
  ru_context.symbol_id = context.symbol;

  notifier.on_new_uplink_symbol(ru_context, grid, is_valid);
}

void ru_ofh_rx_symbol_handler_impl::on_new_prach_window_data(const prach_buffer_context& context,
                                                             shared_prach_buffer         buffer)
{
  notifier.on_new_prach_window_data(context, std::move(buffer));
}
