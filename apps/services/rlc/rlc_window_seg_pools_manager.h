// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "apps/units/flexible_o_du/o_du_high/du_high/du_high_config.h"
#include "lib/rlc/rlc_window_seg_pools.h"

namespace ocudu {

class rlc_window_seg_pools_manager
{
public:
  explicit rlc_window_seg_pools_manager(const du_high_unit_rlc_config& config)
  {
    init_rlc_window_seg_pools(config.drb_rx_window_seg_pool_size,
                              config.drb_tx_window_seg_pool_size,
                              config.srb_rx_window_seg_pool_size,
                              config.srb_tx_window_seg_pool_size);
  }
};

} // namespace ocudu
