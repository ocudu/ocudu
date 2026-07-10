// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/segmented_circular_map.h"
#include <cstddef>

namespace ocudu {

struct rlc_rx_am_sdu_info;
struct rlc_rx_um_sdu_info;

static constexpr unsigned rlc_rx_am_um_shared_window_seg_size      = 256;
static constexpr unsigned rlc_rx_am_um_shared_window_seg_pool_size = 2048;

using rlc_rx_am_um_shared_window_seg_pool =
    shared_map_segment_pool<uint32_t, rlc_rx_am_um_shared_window_seg_size, rlc_rx_am_sdu_info, rlc_rx_um_sdu_info>;

rlc_rx_am_um_shared_window_seg_pool& get_rlc_rx_am_um_shared_window_seg_pool();

using rlc_rx_am_window_seg_pool =
    map_segment_pool_interface<uint32_t, rlc_rx_am_sdu_info, rlc_rx_am_um_shared_window_seg_size>;
rlc_rx_am_window_seg_pool& get_rlc_rx_am_window_seg_pool();

using rlc_rx_um_window_seg_pool =
    map_segment_pool_interface<uint32_t, rlc_rx_um_sdu_info, rlc_rx_am_um_shared_window_seg_size>;
rlc_rx_um_window_seg_pool& get_rlc_rx_um_window_seg_pool();

struct rlc_tx_am_sdu_info;

static constexpr unsigned rlc_tx_am_window_seg_size      = 256;
static constexpr unsigned rlc_tx_am_window_seg_pool_size = 2048;

using rlc_tx_am_window_seg_pool = map_segment_pool_interface<uint32_t, rlc_tx_am_sdu_info, rlc_tx_am_window_seg_size>;
rlc_tx_am_window_seg_pool& get_rlc_tx_am_window_seg_pool();

/// \brief Initializes the static RLC window segment pools application-wide.
///
/// Initializes the application-wide static RLC window segment pools with given sizes.
/// If this function was not called explicitly before first access to any pool,
/// it will be invoked automatically using the defined default values.
///
/// \param rx_pool_size Size of the window segment pool for RLC RX side (AM/UM).
/// \param tx_pool_size Size of the window segment pool for RLC TX side (AM).
void init_rlc_window_seg_pools(std::size_t rx_pool_size, std::size_t tx_pool_size);

} // namespace ocudu
