// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/rlc/rlc_window_seg_pool_factory.h"
#include "rlc_rx_am_entity.h"
#include "rlc_rx_um_entity.h"
#include "rlc_tx_am_entity.h"

using namespace ocudu;

template <>
void rlc_pool_deleter::operator()<rlc_drb_rx_window_seg_pool>(rlc_drb_rx_window_seg_pool* p) const
{
  delete p;
}

template <>
void rlc_pool_deleter::operator()<rlc_drb_tx_window_seg_pool>(rlc_drb_tx_window_seg_pool* p) const
{
  delete p;
}

std::unique_ptr<rlc_drb_rx_window_seg_pool, rlc_pool_deleter>
ocudu::make_rlc_drb_rx_window_seg_pool(size_t nof_segments)
{
  std::unique_ptr<rlc_drb_rx_window_seg_pool, rlc_pool_deleter> pool(new rlc_drb_rx_window_seg_pool(nof_segments));
  return pool;
}

std::unique_ptr<rlc_drb_tx_window_seg_pool, rlc_pool_deleter>
ocudu::make_rlc_drb_tx_window_seg_pool(size_t nof_segments)
{
  std::unique_ptr<rlc_drb_tx_window_seg_pool, rlc_pool_deleter> pool(new rlc_drb_tx_window_seg_pool(nof_segments));
  return pool;
}

std::unique_ptr<rlc_srb_rx_window_seg_pool, rlc_pool_deleter>
ocudu::make_rlc_srb_rx_window_seg_pool(size_t nof_segments)
{
  std::unique_ptr<rlc_srb_rx_window_seg_pool, rlc_pool_deleter> pool(new rlc_srb_rx_window_seg_pool(nof_segments));
  return pool;
}

std::unique_ptr<rlc_srb_tx_window_seg_pool, rlc_pool_deleter>
ocudu::make_rlc_srb_tx_window_seg_pool(size_t nof_segments)
{
  std::unique_ptr<rlc_srb_tx_window_seg_pool, rlc_pool_deleter> pool(new rlc_srb_tx_window_seg_pool(nof_segments));
  return pool;
}
