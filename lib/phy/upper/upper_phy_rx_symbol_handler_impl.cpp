// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "upper_phy_rx_symbol_handler_impl.h"
#include "upper_phy_rx_results_notifier_wrapper.h"
#include "ocudu/adt/format.h"
#include "ocudu/phy/support/prach_buffer_context.h"
#include "ocudu/phy/support/shared_resource_grid.h"
#include "ocudu/phy/upper/channel_coding/ldpc/ldpc.h"
#include "ocudu/phy/upper/channel_processors/pusch/formatters.h"
#include "ocudu/phy/upper/rx_buffer_pool.h"
#include "ocudu/phy/upper/unique_rx_buffer.h"
#include "ocudu/phy/upper/uplink_processor.h"
#include "ocudu/support/error_handling.h"
#include <utility>

using namespace ocudu;

upper_phy_rx_symbol_handler_impl::upper_phy_rx_symbol_handler_impl(uplink_slot_processor_pool& ul_processor_pool_) :
  ul_processor_pool(ul_processor_pool_)
{
}

void upper_phy_rx_symbol_handler_impl::handle_rx_symbol(const upper_phy_rx_symbol_context& context,
                                                        const shared_resource_grid&        grid,
                                                        bool                               is_valid)
{
  // Get uplink processor.
  uplink_slot_processor& ul_proc = ul_processor_pool.get_slot_processor(context.slot);

  // Notify Rx symbol.
  ul_proc.handle_rx_symbol(context.symbol, is_valid);
}

void upper_phy_rx_symbol_handler_impl::handle_rx_prach_window(const prach_buffer_context& context,
                                                              shared_prach_buffer         buffer)
{
  // Get uplink processor.
  uplink_slot_processor& ul_proc = ul_processor_pool.get_slot_processor(context.slot);

  // Process PRACH.
  ul_proc.process_prach(std::move(buffer), context);
}
