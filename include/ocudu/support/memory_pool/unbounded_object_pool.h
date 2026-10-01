// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#ifdef ENABLE_TSAN
#include <sanitizer/tsan_interface.h>
#endif

#include "cameron314/concurrentqueue.h"
#include <memory>

namespace ocudu {

/// Unbounded, thread-safe object pool. Ideal for large objects. Use with caution.
template <typename T>
class unbounded_object_pool
{
  struct pool_deleter {
    pool_deleter() = default;
    pool_deleter(unbounded_object_pool& parent_) : parent(&parent_) {}
    void operator()(T* ptr)
    {
      if (ptr != nullptr) {
        std::unique_ptr<T> obj{ptr};
#ifdef ENABLE_TSAN
        __tsan_release((void*)obj.get());
#endif
        parent->objects.enqueue(std::move(obj));
      }
    }

    unbounded_object_pool* parent;
  };

public:
  using ptr = std::unique_ptr<T, pool_deleter>;

  unbounded_object_pool(unsigned initial_capacity) : objects(initial_capacity)
  {
    for (unsigned i = 0; i != initial_capacity; ++i) {
      objects.enqueue(std::make_unique<T>());
    }
  }
  unbounded_object_pool(unsigned initial_capacity, unsigned expected_nof_deallocation_contexts) :
    objects(initial_capacity, 0, expected_nof_deallocation_contexts)
  {
    for (unsigned i = 0; i != initial_capacity; ++i) {
      objects.enqueue(std::make_unique<T>());
    }
  }

  ptr get()
  {
    std::unique_ptr<T> popped;
    if (objects.try_dequeue(popped)) {
#ifdef ENABLE_TSAN
      __tsan_acquire((void*)popped.get());
#endif
      return ptr{popped.release(), pool_deleter{*this}};
    }
    return ptr{new T(), pool_deleter{*this}};
  }

  unsigned current_capacity_approx() const { return objects.size_approx(); }

private:
  moodycamel::ConcurrentQueue<std::unique_ptr<T>> objects;
};

} // namespace ocudu
