// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "rlc_window_seg_pools.h"
#include "rlc_rx_am_entity.h"
#include "rlc_rx_um_entity.h"
#include "rlc_tx_am_entity.h"
#include "ocudu/support/error_handling.h"

using namespace ocudu;

namespace {

std::size_t                                                                                       rx_pool_size_val = 0;
std::unique_ptr<rlc_rx_am_um_shared_window_seg_pool>                                              rx_pool_inst;
std::size_t                                                                                       tx_pool_size_val = 0;
std::unique_ptr<shared_map_segment_pool<uint32_t, rlc_tx_am_window_seg_size, rlc_tx_am_sdu_info>> tx_pool_inst;

} // namespace

void ocudu::init_rlc_window_seg_pools(std::size_t rx_pool_size, std::size_t tx_pool_size)
{
  if (rx_pool_inst == nullptr) {
    rx_pool_size_val = rx_pool_size;
    rx_pool_inst     = std::make_unique<rlc_rx_am_um_shared_window_seg_pool>(rx_pool_size);
  } else {
    report_fatal_error_if_not(
        rx_pool_size_val >= rx_pool_size,
        "RLC RX window segment pool already initialized with size {} which is less than the requested {}",
        rx_pool_size_val,
        rx_pool_size);
  }

  if (tx_pool_inst == nullptr) {
    tx_pool_size_val = tx_pool_size;
    tx_pool_inst = std::make_unique<shared_map_segment_pool<uint32_t, rlc_tx_am_window_seg_size, rlc_tx_am_sdu_info>>(
        tx_pool_size);
  } else {
    report_fatal_error_if_not(
        tx_pool_size_val >= tx_pool_size,
        "RLC TX window segment pool already initialized with size {} which is less than the requested {}",
        tx_pool_size_val,
        tx_pool_size);
  }
}

rlc_rx_am_um_shared_window_seg_pool& ocudu::get_rlc_rx_am_um_shared_window_seg_pool()
{
  if (rx_pool_inst == nullptr) {
    rx_pool_size_val = rlc_rx_am_um_shared_window_seg_pool_size;
    rx_pool_inst     = std::make_unique<rlc_rx_am_um_shared_window_seg_pool>(rx_pool_size_val);
  }
  return *rx_pool_inst;
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
  if (tx_pool_inst == nullptr) {
    tx_pool_size_val = rlc_tx_am_window_seg_pool_size;
    tx_pool_inst = std::make_unique<shared_map_segment_pool<uint32_t, rlc_tx_am_window_seg_size, rlc_tx_am_sdu_info>>(
        tx_pool_size_val);
  }
  return tx_pool_inst->get_pool_of_type<rlc_tx_am_sdu_info>();
}
