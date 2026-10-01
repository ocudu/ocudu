// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/ofh/ethernet/ethernet_unique_buffer.h"
#include "ocudu/support/ocudu_assert.h"

namespace ocudu {
namespace ether {

class ethernet_rx_buffer_pool;

/// Receive buffer wrapper dedicated for socket-based Ethernet receiver.
class ethernet_rx_buffer_impl : public rx_buffer
{
public:
  /// Constructor receives the reference to the pool, from which one buffer is requested.
  ethernet_rx_buffer_impl(ethernet_rx_buffer_pool& pool_, unsigned id);

  /// Destructor frees this buffer in the pool.
  ~ethernet_rx_buffer_impl() override;

  /// Copy constructor is deleted.
  ethernet_rx_buffer_impl(const ethernet_rx_buffer_impl& other) = delete;

  /// Copy assignment operator is deleted.
  ethernet_rx_buffer_impl& operator=(const ethernet_rx_buffer_impl& other) = delete;

  /// Move constructor.
  ethernet_rx_buffer_impl(ethernet_rx_buffer_impl&& other) noexcept;

  /// Move assignment operator.
  ethernet_rx_buffer_impl& operator=(ethernet_rx_buffer_impl&& other) noexcept;

  // See interface for documentation.
  span<const uint8_t> data() const override;

  /// Returns span of bytes for writing.
  span<uint8_t> storage();

  /// Sets the actually used size.
  void resize(unsigned used_size);

private:
  ethernet_rx_buffer_pool& pool;
  int                      id = -1;
  /// Using size instead of a span here to save space and to enable using this class inside a \c unique_task
  /// implementation exploiting small buffer vtable.
  unsigned size = 0;
};

} // namespace ether
} // namespace ocudu
