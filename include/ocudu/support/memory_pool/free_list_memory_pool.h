// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/support/ocudu_assert.h"
#include <algorithm>
#include <cstddef>
#include <cstring>
#include <limits>
#include <memory>
#include <new>

namespace ocudu {

/// \brief Unsynchronised pool of memory chunks of equal size and alignment.
///
/// The chunks are allocated once, on pool creation. The free chunks form an intrusive list, where each of them holds
/// the index of the next free chunk.
class free_list_memory_pool
{
  using chunk_index = uint32_t;

  /// Index that marks the end of the free list.
  static constexpr chunk_index invalid_chunk_index = std::numeric_limits<chunk_index>::max();

public:
  /// \brief Creates a pool with \c nof_chunks chunks, each of at least \c chunk_size bytes and \c chunk_alignment
  /// alignment.
  free_list_memory_pool(size_t nof_chunks_, size_t chunk_size_, size_t chunk_alignment_) :
    chunk_align(chunk_alignment_),
    chunk_stride(align_up(std::max(chunk_size_, sizeof(chunk_index)), chunk_alignment_)),
    nof_chunks(nof_chunks_),
    mem_block(allocate_block(chunk_stride * nof_chunks_, chunk_alignment_)),
    nof_available(nof_chunks_)
  {
    ocudu_assert(chunk_size_ > 0, "Chunk size must be positive");
    ocudu_assert(nof_chunks_ < invalid_chunk_index, "Number of chunks is too large");

    // Chain all the chunks together, with the last one closing the list.
    for (size_t i = 0; i != nof_chunks; ++i) {
      write_next_index(i, i + 1 == nof_chunks ? invalid_chunk_index : static_cast<chunk_index>(i + 1));
    }
    free_head = nof_chunks == 0 ? invalid_chunk_index : 0;
  }
  free_list_memory_pool(const free_list_memory_pool&)            = delete;
  free_list_memory_pool(free_list_memory_pool&&)                 = delete;
  free_list_memory_pool& operator=(const free_list_memory_pool&) = delete;
  free_list_memory_pool& operator=(free_list_memory_pool&&)      = delete;

  /// Size in bytes of each memory block of the pool.
  size_t memory_block_size() const { return chunk_stride; }

  /// Alignment of each memory block of the pool.
  size_t alignment() const { return chunk_align; }

  /// Number of memory blocks held by the pool.
  size_t nof_memory_blocks() const { return nof_chunks; }

  /// Number of memory blocks currently available for allocation.
  size_t nof_memory_blocks_available() const { return nof_available; }

  /// Whether all the memory blocks of the pool are allocated.
  bool full() const { return free_head == invalid_chunk_index; }

  /// Allocates a memory block from the pool. Returns nullptr if none is available.
  void* allocate() noexcept
  {
    if (free_head == invalid_chunk_index) {
      return nullptr;
    }
    const chunk_index idx = free_head;
    free_head             = read_next_index(idx);
    --nof_available;
    return static_cast<void*>(chunk_address(idx));
  }

  /// Returns a previously allocated memory block back to the pool.
  void deallocate(void* p) noexcept
  {
    ocudu_assert(p != nullptr, "Deallocated chunks must have a valid address");
    ocudu_assert(is_chunk_of_pool(p), "Deallocated chunk does not belong to this pool");
    ocudu_assert(nof_available < nof_chunks, "Deallocating more chunks than the pool holds");

    const auto idx = static_cast<chunk_index>((static_cast<uint8_t*>(p) - mem_block.get()) / chunk_stride);
    write_next_index(idx, free_head);
    free_head = idx;
    ++nof_available;
  }

private:
  struct block_deleter {
    void   operator()(uint8_t* p) const { ::operator delete(static_cast<void*>(p), std::align_val_t{align}); }
    size_t align;
  };
  using memory_block = std::unique_ptr<uint8_t[], block_deleter>;

  static size_t align_up(size_t value, size_t align)
  {
    ocudu_assert(align > 0 and (align & (align - 1)) == 0, "Chunk alignment must be a power of 2");
    return (value + align - 1) / align * align;
  }

  static memory_block allocate_block(size_t nof_bytes, size_t align)
  {
    return memory_block{static_cast<uint8_t*>(::operator new(nof_bytes, std::align_val_t{align})),
                        block_deleter{align}};
  }

  uint8_t* chunk_address(size_t idx) const { return mem_block.get() + idx * chunk_stride; }

  // The list link is accessed with memcpy, so that it does not depend on the alignment requested for the chunks.
  void write_next_index(size_t idx, chunk_index next) { std::memcpy(chunk_address(idx), &next, sizeof(next)); }

  chunk_index read_next_index(size_t idx) const
  {
    chunk_index next;
    std::memcpy(&next, chunk_address(idx), sizeof(next));
    return next;
  }

  bool is_chunk_of_pool(const void* p) const
  {
    const auto* byte_ptr = static_cast<const uint8_t*>(p);
    return byte_ptr >= mem_block.get() and byte_ptr < mem_block.get() + chunk_stride * nof_chunks and
           (byte_ptr - mem_block.get()) % chunk_stride == 0;
  }

  const size_t chunk_align;
  const size_t chunk_stride;
  const size_t nof_chunks;

  // Storage of all the chunks of the pool.
  memory_block mem_block;

  // Index of the first free chunk.
  chunk_index free_head = invalid_chunk_index;

  // Number of chunks in the free list.
  size_t nof_available;
};

/// \brief Unsynchronised pool of objects of type \c T, backed by a \c free_list_memory_pool.
template <typename T>
class free_list_object_pool
{
  struct pool_deleter {
    pool_deleter() = default;
    explicit pool_deleter(free_list_memory_pool& parent_) : parent(&parent_) {}

    void operator()(T* p) const
    {
      if (p != nullptr) {
        p->~T();
        parent->deallocate(p);
      }
    }

    free_list_memory_pool* parent = nullptr;
  };

public:
  using ptr = std::unique_ptr<T, pool_deleter>;

  /// Creates a pool able to hold \c nof_objects objects.
  explicit free_list_object_pool(size_t nof_objects) : mem_pool(nof_objects, sizeof(T), alignof(T)) {}

  /// Number of objects held by the pool.
  size_t nof_objects() const { return mem_pool.nof_memory_blocks(); }

  /// Number of objects that can still be created.
  size_t nof_objects_available() const { return mem_pool.nof_memory_blocks_available(); }

  /// Whether all the objects of the pool are in use.
  bool full() const { return mem_pool.full(); }

  /// Creates an object in the pool. Returns nullptr if the pool has no object available.
  template <typename... Args>
  ptr get(Args&&... args)
  {
    void* chunk = mem_pool.allocate();
    if (chunk == nullptr) {
      return ptr{nullptr, pool_deleter{mem_pool}};
    }
    return ptr{new (chunk) T(std::forward<Args>(args)...), pool_deleter{mem_pool}};
  }

private:
  free_list_memory_pool mem_pool;
};

} // namespace ocudu
