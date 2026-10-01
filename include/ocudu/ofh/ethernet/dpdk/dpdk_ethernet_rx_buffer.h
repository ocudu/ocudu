// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/ofh/ethernet/ethernet_unique_buffer.h"
#include "ocudu/support/ocudu_assert.h"
#include <rte_ethdev.h>

namespace ocudu {
namespace ether {

/// Receive buffer wrapper dedicated for DPDK Ethernet receiver.
class dpdk_rx_buffer_impl : public rx_buffer
{
public:
  explicit dpdk_rx_buffer_impl(::rte_mbuf* mbuf_) : mbuf(mbuf_)
  {
    ocudu_assert(mbuf, "Invalid DPDK mbuf was passed to dpdk_rx_buffer_impl");
  }

  /// Copy constructor is deleted.
  dpdk_rx_buffer_impl(dpdk_rx_buffer_impl& /**/) = delete;

  /// Move constructor.
  dpdk_rx_buffer_impl(dpdk_rx_buffer_impl&& other) noexcept
  {
    mbuf       = other.mbuf;
    other.mbuf = nullptr;
  }

  /// Move assignment operator.
  dpdk_rx_buffer_impl& operator=(dpdk_rx_buffer_impl&& other) noexcept
  {
    // Free stored mbuf.
    ::rte_pktmbuf_free(mbuf);

    mbuf       = other.mbuf;
    other.mbuf = nullptr;
    return *this;
  }

  ~dpdk_rx_buffer_impl() { ::rte_pktmbuf_free(mbuf); }

  // See interface for documentation.
  span<const uint8_t> data() const override
  {
    ocudu_assert(mbuf != nullptr, "Invalid dpdk_rx_buffer_impl accessed");

    ::uint8_t* eth_data = rte_pktmbuf_mtod(mbuf, uint8_t*);
    return span<const uint8_t>(eth_data, mbuf->pkt_len);
  }

private:
  ::rte_mbuf* mbuf;
};

} // namespace ether
} // namespace ocudu
