// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ru_lower_phy_downlink_handler_impl.h"
#include "ocudu/phy/lower/lower_phy_downlink_handler.h"
#include "ocudu/phy/support/shared_resource_grid.h"

using namespace ocudu;

void ru_lower_phy_downlink_handler_impl::handle_dl_data(const resource_grid_context& context,
                                                        const shared_resource_grid&  grid)
{
  ocudu_assert(context.sector < handlers.size(), "Invalid sector {}", context.sector);

  handlers[context.sector]->handle_resource_grid(context, grid);
}
