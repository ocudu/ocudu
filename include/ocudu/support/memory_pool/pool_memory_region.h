// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/adt/span.h"
#include "ocudu/support/memory_pool/heap_memory_resource.h"
#include "ocudu/support/memory_pool/hugepage_memory_resource.h"
#include <variant>

namespace ocudu {

/// \brief Memory region allocated for the \c fixed_size_memory_block_pool.
///
/// Allocates an aligned contiguous memory region with optional huge page backing.
class pool_memory_region
{
public:
  /// \brief Constructor of the memory region.
  ///
  /// If huge page backing is used, the total requested memory region is
  /// first aligned to the cache line boundary and then to the huge page boundary.
  ///
  /// \param size_bytes    Requested size in bytes.
  /// \param use_hugepages Flag to enable huge page backing.
  pool_memory_region(std::size_t size_bytes, bool use_hugepages = false);

  pool_memory_region(const pool_memory_region&)            = delete;
  pool_memory_region(pool_memory_region&&)                 = delete;
  pool_memory_region& operator=(const pool_memory_region&) = delete;
  pool_memory_region& operator=(pool_memory_region&&)      = delete;

  /// Provides writing access to the underlying memory.
  uint8_t* data() noexcept { return block_view.data(); }

  /// Provides reading access to the underlying memory.
  const uint8_t* data() const noexcept { return block_view.data(); }

  /// Get the number of allocated bytes.
  std::size_t size() const noexcept { return block_view.size(); }

private:
  /// Memory resource owning the allocated region.
  std::variant<heap_memory_resource, hugepage_memory_resource> storage;
  /// View over the region owned by \c storage, resolved once at construction.
  span<uint8_t> block_view;
};

} // namespace ocudu
