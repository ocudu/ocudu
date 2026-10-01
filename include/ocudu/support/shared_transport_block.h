// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/adt/span.h"
#include <atomic>
#include <utility>

namespace ocudu {

/// Manages the shared ownership of a Transport Block buffer giving access to a view of it.
class shared_transport_block
{
  span<const uint8_t>    buffer;
  std::atomic<unsigned>* ref_cnt = nullptr;

public:
  shared_transport_block() = default;

  explicit shared_transport_block(span<const uint8_t>    buffer_,
                                  std::atomic<unsigned>* ref_cnt_     = nullptr,
                                  bool                   do_increment = true) :
    buffer(buffer_), ref_cnt(ref_cnt_)
  {
    if (ref_cnt && do_increment) {
      ref_cnt->fetch_add(1, std::memory_order::memory_order_relaxed);
    }
  }

  ~shared_transport_block() { release(); }

  shared_transport_block(const shared_transport_block& other) : buffer(other.buffer), ref_cnt(other.ref_cnt)
  {
    if (ref_cnt) {
      ref_cnt->fetch_add(1, std::memory_order::memory_order_relaxed);
    }
  }

  shared_transport_block& operator=(const shared_transport_block& other) = delete;

  shared_transport_block(shared_transport_block&& other) noexcept :
    buffer(std::exchange(other.buffer, {})), ref_cnt(std::exchange(other.ref_cnt, nullptr))
  {
  }

  shared_transport_block& operator=(shared_transport_block&& other) noexcept
  {
    buffer = std::exchange(other.buffer, {});

    release();
    ref_cnt = std::exchange(other.ref_cnt, nullptr);
    return *this;
  }

  /// Releases the ownership of this buffer.
  void release()
  {
    if (ref_cnt) {
      ref_cnt->fetch_sub(1, std::memory_order::memory_order_acq_rel);
      ref_cnt = nullptr;
    }
  }

  /// Returns a view of the underlying buffer.
  span<const uint8_t> get_buffer() const { return buffer; }
};

} // namespace ocudu
