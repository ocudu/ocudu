// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/rlc/rlc_window_seg_pool_factory.h"
#include "ocudu/rlc/rlc_window_seg_pools.h"
#include <memory>

namespace ocudu::odu {

/// \brief Resources owned and managed by the DU manager, available to its procedures and sub-structures.
struct du_manager_mem_resources {
  struct rlc_resources {
    rlc_resources(std::unique_ptr<rlc_drb_rx_window_seg_pool, rlc_pool_deleter>              drb_rx_window_seg_pool_,
                  std::vector<std::unique_ptr<rlc_drb_tx_window_seg_pool, rlc_pool_deleter>> drb_tx_window_seg_pools_,
                  std::unique_ptr<rlc_srb_rx_window_seg_pool, rlc_pool_deleter>              srb_rx_window_seg_pool_,
                  std::vector<std::unique_ptr<rlc_srb_tx_window_seg_pool, rlc_pool_deleter>> srb_tx_window_seg_pools_) :
      drb_rx_window_seg_pool(std::move(drb_rx_window_seg_pool_)),
      drb_tx_window_seg_pools(std::move(drb_tx_window_seg_pools_)),
      srb_rx_window_seg_pool(std::move(srb_rx_window_seg_pool_)),
      srb_tx_window_seg_pools(std::move(srb_tx_window_seg_pools_))
    {
    }

    /// DRB RX window segment pool that is shared across all cells and all UEs.
    std::unique_ptr<rlc_drb_rx_window_seg_pool, rlc_pool_deleter> drb_rx_window_seg_pool;
    /// Vector of DRB TX window segment pools. One pool per cell due to concurrency. Shared across all UEs of one cell.
    std::vector<std::unique_ptr<rlc_drb_tx_window_seg_pool, rlc_pool_deleter>> drb_tx_window_seg_pools;
    /// SRB RX window segment pool that is shared across all cells and all UEs.
    std::unique_ptr<rlc_srb_rx_window_seg_pool, rlc_pool_deleter> srb_rx_window_seg_pool;
    /// Vector of SRB TX window segment pools. One pool per cell due to concurrency. Shared across all UEs of one cell.
    std::vector<std::unique_ptr<rlc_srb_tx_window_seg_pool, rlc_pool_deleter>> srb_tx_window_seg_pools;
  };

  rlc_resources rlc;
};

} // namespace ocudu::odu
