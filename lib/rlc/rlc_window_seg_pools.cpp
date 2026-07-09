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

std::size_t                                 drb_rx_pool_size_val = 0;
std::unique_ptr<rlc_drb_rx_window_seg_pool> drb_rx_pool_inst;
std::size_t                                 drb_tx_pool_size_val = 0;
std::unique_ptr<rlc_drb_tx_window_seg_pool> drb_tx_pool_inst;

std::size_t                                 srb_rx_pool_size_val = 0;
std::unique_ptr<rlc_srb_rx_window_seg_pool> srb_rx_pool_inst;
std::size_t                                 srb_tx_pool_size_val = 0;
std::unique_ptr<rlc_srb_tx_window_seg_pool> srb_tx_pool_inst;

} // namespace

void ocudu::init_rlc_window_seg_pools(std::size_t drb_rx_pool_size,
                                      std::size_t drb_tx_pool_size,
                                      std::size_t srb_rx_pool_size,
                                      std::size_t srb_tx_pool_size)
{
  if (drb_rx_pool_inst == nullptr) {
    drb_rx_pool_size_val = drb_rx_pool_size;
    drb_rx_pool_inst     = std::make_unique<rlc_drb_rx_window_seg_pool>(drb_rx_pool_size);
  } else {
    report_fatal_error_if_not(
        drb_rx_pool_size_val >= drb_rx_pool_size,
        "RLC RX window segment pool already initialized with size {} which is less than the requested {}",
        drb_rx_pool_size_val,
        drb_rx_pool_size);
  }

  if (drb_tx_pool_inst == nullptr) {
    drb_tx_pool_size_val = drb_tx_pool_size;
    drb_tx_pool_inst     = std::make_unique<rlc_drb_tx_window_seg_pool>(drb_tx_pool_size);
  } else {
    report_fatal_error_if_not(
        drb_tx_pool_size_val >= drb_tx_pool_size,
        "RLC TX window segment pool already initialized with size {} which is less than the requested {}",
        drb_tx_pool_size_val,
        drb_tx_pool_size);
  }

  if (srb_rx_pool_inst == nullptr) {
    srb_rx_pool_size_val = srb_rx_pool_size;
    srb_rx_pool_inst     = std::make_unique<rlc_srb_rx_window_seg_pool>(srb_rx_pool_size);
  } else {
    report_fatal_error_if_not(
        srb_rx_pool_size_val >= srb_rx_pool_size,
        "RLC RX window segment pool already initialized with size {} which is less than the requested {}",
        srb_rx_pool_size_val,
        srb_rx_pool_size);
  }

  if (srb_tx_pool_inst == nullptr) {
    srb_tx_pool_size_val = srb_tx_pool_size;
    srb_tx_pool_inst     = std::make_unique<rlc_srb_tx_window_seg_pool>(srb_tx_pool_size);
  } else {
    report_fatal_error_if_not(
        srb_tx_pool_size_val >= srb_tx_pool_size,
        "RLC TX window segment pool already initialized with size {} which is less than the requested {}",
        srb_tx_pool_size_val,
        srb_tx_pool_size);
  }
}

rlc_drb_rx_window_seg_pool& ocudu::get_rlc_drb_rx_window_seg_pool()
{
  if (drb_rx_pool_inst == nullptr) {
    drb_rx_pool_size_val = rlc_drb_rx_window_seg_pool_size;
    drb_rx_pool_inst     = std::make_unique<rlc_drb_rx_window_seg_pool>(drb_rx_pool_size_val);
  }
  return *drb_rx_pool_inst;
}

rlc_drb_am_rx_window_seg_pool& ocudu::get_rlc_drb_am_rx_window_seg_pool()
{
  return get_rlc_drb_rx_window_seg_pool().get_pool_of_type<rlc_rx_am_sdu_info>();
}

rlc_drb_um_rx_window_seg_pool& ocudu::get_rlc_drb_um_rx_window_seg_pool()
{
  return get_rlc_drb_rx_window_seg_pool().get_pool_of_type<rlc_rx_um_sdu_info>();
}

rlc_drb_am_tx_window_seg_pool& ocudu::get_rlc_drb_am_tx_window_seg_pool()
{
  if (drb_tx_pool_inst == nullptr) {
    drb_tx_pool_size_val = rlc_drb_tx_window_seg_pool_size;
    drb_tx_pool_inst     = std::make_unique<rlc_drb_tx_window_seg_pool>(drb_tx_pool_size_val);
  }
  return drb_tx_pool_inst->get_pool_of_type<rlc_tx_am_sdu_info>();
}

rlc_srb_rx_window_seg_pool& ocudu::get_rlc_srb_rx_window_seg_pool()
{
  if (srb_rx_pool_inst == nullptr) {
    srb_rx_pool_size_val = rlc_srb_rx_window_seg_pool_size;
    srb_rx_pool_inst     = std::make_unique<rlc_srb_rx_window_seg_pool>(srb_rx_pool_size_val);
  }
  return *srb_rx_pool_inst;
}

rlc_srb_am_rx_window_seg_pool& ocudu::get_rlc_srb_am_rx_window_seg_pool()
{
  return get_rlc_srb_rx_window_seg_pool().get_pool_of_type<rlc_rx_am_sdu_info>();
}

rlc_srb_um_rx_window_seg_pool& ocudu::get_rlc_srb_um_rx_window_seg_pool()
{
  return get_rlc_srb_rx_window_seg_pool().get_pool_of_type<rlc_rx_um_sdu_info>();
}

rlc_srb_am_tx_window_seg_pool& ocudu::get_rlc_srb_am_tx_window_seg_pool()
{
  if (srb_tx_pool_inst == nullptr) {
    srb_tx_pool_size_val = rlc_srb_tx_window_seg_pool_size;
    srb_tx_pool_inst     = std::make_unique<rlc_srb_tx_window_seg_pool>(srb_tx_pool_size_val);
  }
  return srb_tx_pool_inst->get_pool_of_type<rlc_tx_am_sdu_info>();
}
