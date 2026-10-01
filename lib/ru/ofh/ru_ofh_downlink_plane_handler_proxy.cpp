// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ru_ofh_downlink_plane_handler_proxy.h"
#include "ocudu/ofh/transmitter/ofh_downlink_handler.h"
#include "ocudu/phy/support/resource_grid_context.h"
#include "ocudu/phy/support/shared_resource_grid.h"
#include <memory>

using namespace ocudu;

void ru_downlink_plane_handler_proxy::handle_dl_data(const resource_grid_context& context,
                                                     const shared_resource_grid&  grid)
{
  ocudu_assert(context.sector < sectors.size(), "Invalid sector id '{}'", context.sector);

  auto& sector = sectors[context.sector];
  sector->handle_dl_data(context, grid);
}
