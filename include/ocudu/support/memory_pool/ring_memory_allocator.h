// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/support/memory_pool/heap_memory_resource.h"
#include "ocudu/support/ocudu_assert.h"

namespace ocudu {

/// \brief This memory allocator allocates chunks of a contiguous memory region in a circular fashion.
class ring_memory_allocator
{
public:
  explicit ring_memory_allocator(span<uint8_t> mem_block_) : mem_block(mem_block_) {}

  void* allocate(size_t sz) noexcept
  {
    ocudu_assert(sz <= mem_block.size(), "Invalid allocation size ({} > {})", sz, mem_block.size());

    if (next_offset + sz > mem_block.size()) {
      // Wrap-around.
      next_offset = sz;
      return mem_block.data();
    } else {
      uint8_t* ptr = mem_block.data() + next_offset;
      next_offset += sz;
      return ptr;
    }
  }

  void deallocate(void* p) noexcept
  {
    ocudu_assert(static_cast<uint8_t*>(p) - mem_block.data() < (ssize_t)mem_block.size(),
                 "Deallocating invalid memory pointer");
  }

private:
  size_t        next_offset = 0;
  span<uint8_t> mem_block;
};

} // namespace ocudu
