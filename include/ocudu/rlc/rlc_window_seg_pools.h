// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/segmented_circular_map.h"

namespace ocudu {

/// Default DRB receive window segment size.
constexpr unsigned rlc_drb_rx_window_seg_size = 256;
/// Default DRB receive window segment pool size.
constexpr unsigned rlc_drb_rx_window_seg_pool_size = 2048;
/// Default DRB transmit window segment size.
constexpr unsigned rlc_drb_tx_window_seg_size = 256;
/// Default DRB transmit window segment pool size.
constexpr unsigned rlc_drb_tx_window_seg_pool_size = 2048;

/// Default SRB receive window segment size.
constexpr unsigned rlc_srb_rx_window_seg_size = 256; // TODO: Should be 8.
/// Default SRB receive window segment pool size.
constexpr unsigned rlc_srb_rx_window_seg_pool_size = 2048;
/// Default SRB transmit window segment size.
constexpr unsigned rlc_srb_tx_window_seg_size = 256; // TODO: Should be 8.
/// Default SRB transmit window segment pool size.
constexpr unsigned rlc_srb_tx_window_seg_pool_size = 2048;

struct rlc_rx_am_sdu_info;
struct rlc_rx_um_sdu_info;
struct rlc_tx_am_sdu_info;

using rlc_drb_rx_window_seg_pool =
    shared_map_segment_pool<uint32_t, rlc_drb_rx_window_seg_size, rlc_rx_am_sdu_info, rlc_rx_um_sdu_info>;

using rlc_drb_am_rx_window_seg_pool =
    map_segment_pool_interface<uint32_t, rlc_rx_am_sdu_info, rlc_drb_rx_window_seg_size>;

using rlc_drb_um_rx_window_seg_pool =
    map_segment_pool_interface<uint32_t, rlc_rx_um_sdu_info, rlc_drb_rx_window_seg_size>;

using rlc_drb_tx_window_seg_pool = shared_map_segment_pool<uint32_t, rlc_drb_tx_window_seg_size, rlc_tx_am_sdu_info>;

using rlc_drb_am_tx_window_seg_pool =
    map_segment_pool_interface<uint32_t, rlc_tx_am_sdu_info, rlc_drb_tx_window_seg_size>;

using rlc_srb_rx_window_seg_pool =
    shared_map_segment_pool<uint32_t, rlc_srb_rx_window_seg_size, rlc_rx_am_sdu_info, rlc_rx_um_sdu_info>;

using rlc_srb_am_rx_window_seg_pool =
    map_segment_pool_interface<uint32_t, rlc_rx_am_sdu_info, rlc_srb_rx_window_seg_size>;

using rlc_srb_um_rx_window_seg_pool =
    map_segment_pool_interface<uint32_t, rlc_rx_um_sdu_info, rlc_srb_rx_window_seg_size>;

using rlc_srb_tx_window_seg_pool = shared_map_segment_pool<uint32_t, rlc_srb_tx_window_seg_size, rlc_tx_am_sdu_info>;

using rlc_srb_am_tx_window_seg_pool =
    map_segment_pool_interface<uint32_t, rlc_tx_am_sdu_info, rlc_srb_tx_window_seg_size>;

} // namespace ocudu
