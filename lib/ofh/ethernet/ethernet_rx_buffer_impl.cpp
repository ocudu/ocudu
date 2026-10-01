// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ethernet_rx_buffer_impl.h"
#include "ethernet_rx_buffer_pool.h"
#include <utility>

using namespace ocudu;
using namespace ether;

ethernet_rx_buffer_impl::ethernet_rx_buffer_impl(ethernet_rx_buffer_pool& pool_, unsigned id_) : pool(pool_), id(id_)
{
  size = pool.get_data(id).size();
}

ethernet_rx_buffer_impl::ethernet_rx_buffer_impl(ethernet_rx_buffer_impl&& other) noexcept :
  pool(other.pool), id(std::exchange(other.id, -1)), size(std::exchange(other.size, 0))
{
}

ethernet_rx_buffer_impl& ethernet_rx_buffer_impl::operator=(ethernet_rx_buffer_impl&& other) noexcept
{
  id         = other.id;
  size       = other.size;
  other.id   = -1;
  other.size = 0;

  return *this;
}

span<const uint8_t> ethernet_rx_buffer_impl::data() const
{
  ocudu_assert(id >= 0, "Invalid Ethernet rx buffer accessed");
  return pool.get_data(id).first(size);
}

span<uint8_t> ethernet_rx_buffer_impl::storage()
{
  ocudu_assert(id >= 0, "Invalid Ethernet rx buffer accessed");
  return pool.get_data(id);
}

void ethernet_rx_buffer_impl::resize(unsigned used_size)
{
  ocudu_assert(id >= 0, "Invalid Ethernet rx buffer accessed");
  ocudu_assert(used_size <= pool.get_data(id).size(),
               "The size of buffer can not be bigger than the allocated size of {} bytes",
               pool.get_data(id).size());
  size = used_size;
}

ethernet_rx_buffer_impl::~ethernet_rx_buffer_impl()
{
  if (id >= 0) {
    pool.free(id);
  }
}
