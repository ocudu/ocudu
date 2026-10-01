// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/support/ocudu_assert.h"
#include <cstddef>
#include <limits>
#include <memory>
#include <vector>

namespace ocudu {

/// \brief Unsynchronised pool of recycled objects of type \c T.
///
/// All the objects are constructed on pool creation and are never destroyed while the pool lives. An object handed out
/// by the pool keeps the state left by its previous holder, so the caller is responsible for reinitializing it. This
/// makes the pool suitable for objects that allocate memory on construction, as that memory is preserved across uses.
template <typename T>
class free_list_recycling_object_pool
{
  using object_index = uint32_t;

  struct pool_deleter {
    pool_deleter() = default;
    explicit pool_deleter(free_list_recycling_object_pool& parent_) : parent(&parent_) {}

    void operator()(T* p) const
    {
      if (p != nullptr) {
        parent->deallocate(p);
      }
    }

    free_list_recycling_object_pool* parent = nullptr;
  };

public:
  using ptr = std::unique_ptr<T, pool_deleter>;

  /// Creates a pool holding \c nof_objects default-constructed objects.
  explicit free_list_recycling_object_pool(size_t nof_objects) : objects(nof_objects) { build_free_list(); }

  /// \brief Creates a pool holding \c nof_objects objects, each passed to \c init after construction.
  /// \param init Callable invoked once per object, used to bring it to its reusable state.
  template <typename InitFn>
  free_list_recycling_object_pool(size_t nof_objects, const InitFn& init) : objects(nof_objects)
  {
    for (T& obj : objects) {
      init(obj);
    }
    build_free_list();
  }
  free_list_recycling_object_pool(const free_list_recycling_object_pool&)            = delete;
  free_list_recycling_object_pool(free_list_recycling_object_pool&&)                 = delete;
  free_list_recycling_object_pool& operator=(const free_list_recycling_object_pool&) = delete;
  free_list_recycling_object_pool& operator=(free_list_recycling_object_pool&&)      = delete;

  /// Number of objects held by the pool.
  size_t nof_objects() const { return objects.size(); }

  /// Number of objects that are not in use.
  size_t nof_objects_available() const { return free_list.size(); }

  /// Whether all the objects of the pool are in use.
  bool full() const { return free_list.empty(); }

  /// Takes an object from the pool. Returns nullptr if all its objects are in use.
  ptr get()
  {
    if (free_list.empty()) {
      return ptr{nullptr, pool_deleter{*this}};
    }
    const object_index idx = free_list.back();
    free_list.pop_back();
    return ptr{&objects[idx], pool_deleter{*this}};
  }

private:
  void build_free_list()
  {
    ocudu_assert(objects.size() < std::numeric_limits<object_index>::max(), "Number of objects is too large");

    free_list.reserve(objects.size());
    for (size_t i = objects.size(); i != 0; --i) {
      free_list.push_back(static_cast<object_index>(i - 1));
    }
  }

  void deallocate(T* p) noexcept
  {
    ocudu_assert(p >= objects.data() and p < objects.data() + objects.size(),
                 "Returned object does not belong to this pool");
    ocudu_assert(free_list.size() < objects.size(), "Returning more objects than the pool holds");
    free_list.push_back(static_cast<object_index>(p - objects.data()));
  }

  // Storage of all the objects of the pool. Never resized, so that the objects do not change address.
  std::vector<T> objects;

  // Indexes of the objects that are not in use.
  std::vector<object_index> free_list;
};

} // namespace ocudu
