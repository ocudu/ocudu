// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/adt/span.h"
#include <utility>

namespace ocudu {

/// Represents a memory resource backed by huge pages.
class hugepage_memory_resource
{
public:
  /// Constructor receives a pointer to the mapped memory block, its size, and the mapped size.
  hugepage_memory_resource(uint8_t* ptr, size_t sz, size_t mapped_sz);

  /// Destructor unmaps the reserved huge page region.
  ~hugepage_memory_resource();

  /// The backing region is set up once and never reassigned, so the move assignment is deleted.
  hugepage_memory_resource& operator=(hugepage_memory_resource&&) = delete;
  hugepage_memory_resource(const hugepage_memory_resource&)       = delete;

  hugepage_memory_resource(hugepage_memory_resource&& other) noexcept :
    block_view(std::exchange(other.block_view, span<uint8_t>())), mapped_size(std::exchange(other.mapped_size, 0))
  {
  }

  span<uint8_t> memory_block() const { return block_view; }

  size_t size() const { return block_view.size(); }

private:
  span<uint8_t> block_view;
  size_t        mapped_size = 0;
};

} // namespace ocudu
