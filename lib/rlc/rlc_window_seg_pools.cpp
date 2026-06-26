// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "rlc_window_seg_pools.h"
#include "rlc_rx_am_entity.h"
#include "rlc_rx_um_entity.h"
#include "rlc_tx_am_entity.h"

using namespace ocudu;

rlc_rx_am_um_shared_window_seg_pool& ocudu::get_rlc_rx_am_um_shared_window_seg_pool()
{
  static auto pool = std::make_unique<
      shared_map_segment_pool<uint32_t, rlc_rx_am_um_shared_window_seg_size, rlc_rx_am_sdu_info, rlc_rx_um_sdu_info>>(
      rlc_rx_am_um_shared_window_seg_pool_size);
  return *pool;
}

rlc_rx_am_window_seg_pool& ocudu::get_rlc_rx_am_window_seg_pool()
{
  return get_rlc_rx_am_um_shared_window_seg_pool().get_pool_of_type<rlc_rx_am_sdu_info>();
}

rlc_rx_um_window_seg_pool& ocudu::get_rlc_rx_um_window_seg_pool()
{
  return get_rlc_rx_am_um_shared_window_seg_pool().get_pool_of_type<rlc_rx_um_sdu_info>();
}

rlc_tx_am_window_seg_pool& ocudu::get_rlc_tx_am_window_seg_pool()
{
  static auto pool = std::make_unique<shared_map_segment_pool<uint32_t, rlc_tx_am_window_seg_size, rlc_tx_am_sdu_info>>(
      rlc_tx_am_window_seg_pool_size);
  return pool->get_pool_of_type<rlc_tx_am_sdu_info>();
}
