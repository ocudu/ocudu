// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/rlc/rlc_window_seg_pools.h"
#include <memory>

namespace ocudu {

/// Deleter for RLC window segment pools
struct rlc_pool_deleter {
  template <typename PoolType>
  void operator()(PoolType* p) const;
};

std::unique_ptr<rlc_drb_rx_window_seg_pool, rlc_pool_deleter> make_rlc_drb_rx_window_seg_pool(size_t nof_segments);
std::unique_ptr<rlc_drb_tx_window_seg_pool, rlc_pool_deleter> make_rlc_drb_tx_window_seg_pool(size_t nof_segments);
std::unique_ptr<rlc_srb_rx_window_seg_pool, rlc_pool_deleter> make_rlc_srb_rx_window_seg_pool(size_t nof_segments);
std::unique_ptr<rlc_srb_tx_window_seg_pool, rlc_pool_deleter> make_rlc_srb_tx_window_seg_pool(size_t nof_segments);

} // namespace ocudu
