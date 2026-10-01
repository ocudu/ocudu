// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/support/memory_pool/memory_pool_utils.h"
#include "ocudu/support/ocudu_assert.h"
#include <cstdint>

namespace ocudu {

/// \brief Linear allocator for a contiguous memory block.
///
/// This type of allocator doesn't provide a deallocation mechanism.
class linear_memory_allocator
{
public:
  linear_memory_allocator(void* memory_resource, std::size_t memory_resource_size) :
    mem_start(static_cast<uint8_t*>(memory_resource)), mem_res_size(memory_resource_size)
  {
    ocudu_sanity_check(mem_start != nullptr and mem_res_size != 0, "Invalid memory resource");
  }

  std::size_t size() const { return mem_res_size; }
  std::size_t nof_bytes_left() const { return mem_res_size - mem_offset; }
  std::size_t nof_bytes_allocated() const { return mem_offset; }

  void* memory_resource_start() const { return mem_start; }

  void* allocate(std::size_t sz, std::size_t al) noexcept
  {
    void* p    = align_next(mem_start + mem_offset, al);
    mem_offset = (static_cast<uint8_t*>(p) - mem_start) + sz;
    ocudu_sanity_check(mem_offset <= mem_res_size, "Out of memory");
    return p;
  }

private:
  uint8_t*    mem_start;
  std::size_t mem_res_size;
  std::size_t mem_offset = 0;
};

} // namespace ocudu
