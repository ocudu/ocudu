// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/span.h"
#include <flatbuffers/flatbuffers.h>

namespace ocudu::schedtrace {

/// \brief Reusable FlatBufferBuilder with a preallocated buffer, used to build cell events without mallocs.
///
/// The buffer is allocated eagerly on construction. clear() retains it, so steady-state event building performs no
/// allocations. If an event outgrows the buffer, the builder grows it once (one malloc) and keeps the larger buffer
/// from then on. Allocations are tracked to detect such growth.
class event_builder
{
public:
  explicit event_builder(size_t initial_size) : builder(initial_size, &alloc)
  {
    // Force the lazy initial buffer allocation now so it does not happen on the producer (RT) thread.
    const uint8_t dummy = 0;
    builder.PushBytes(&dummy, 1);
    builder.Clear();
  }
  event_builder(const event_builder&)            = delete;
  event_builder(event_builder&&)                 = delete;
  event_builder& operator=(const event_builder&) = delete;
  event_builder& operator=(event_builder&&)      = delete;

  flatbuffers::FlatBufferBuilder&       fbb() { return builder; }
  const flatbuffers::FlatBufferBuilder& fbb() const { return builder; }

  /// Returns the finished buffer. Only valid after Finish*() has been called on the builder.
  span<const uint8_t> finished() const { return {builder.GetBufferPointer(), builder.GetSize()}; }

  /// Discards the event being built, retaining the allocated buffer.
  void clear() { builder.Clear(); }

  /// Current buffer size. Larger than the initial size if some event did not fit and forced a growth.
  size_t buffer_size() const { return max_allocated_size; }

  /// Number of allocations performed by this builder since construction. Stays at one while running malloc-free.
  size_t nof_allocations() const { return nof_allocs; }

private:
  /// Allocator that behaves like the default one but records the enclosing builder's allocation metrics.
  class tracking_allocator : public flatbuffers::Allocator
  {
  public:
    explicit tracking_allocator(event_builder& parent_) : parent(parent_) {}

    uint8_t* allocate(size_t size) override
    {
      parent.max_allocated_size = std::max(parent.max_allocated_size, size);
      ++parent.nof_allocs;
      return new uint8_t[size];
    }

    void deallocate(uint8_t* p, size_t /*size*/) override { delete[] p; }

  private:
    event_builder& parent;
  };

  size_t                         max_allocated_size = 0;
  size_t                         nof_allocs         = 0;
  tracking_allocator             alloc{*this};
  flatbuffers::FlatBufferBuilder builder;
};

} // namespace ocudu::schedtrace
