// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/adt/span.h"
#include "ocudu/support/ocudu_assert.h"
#include <memory>

namespace ocudu {

/// Represents a memory resource backed by the heap.
class heap_memory_resource
{
public:
  heap_memory_resource(size_t sz) : block(std::make_unique<uint8_t[]>(sz)), block_view(block.get(), sz)
  {
    ocudu_assert(block != nullptr, "Failed to allocate memory pool");
  }

  span<uint8_t> memory_block() const { return block_view; }

  size_t size() const { return block_view.size(); }

private:
  std::unique_ptr<uint8_t[]> block;
  span<uint8_t>              block_view;
};

} // namespace ocudu
