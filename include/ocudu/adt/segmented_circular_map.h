// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/adt/expected.h"
#include "ocudu/support/ocudu_assert.h"
#include <algorithm>
#include <array>
#include <cstdint>
#include <optional>
#include <tuple>
#include <type_traits>
#include <vector>

namespace ocudu {

/// \brief Fixed-size segment used by segmented_circular_map to store key-value pairs.
///
/// \tparam K Key type.
/// \tparam V Mapped value type.
/// \tparam L Number of slots in the segment.
template <typename K, typename V, size_t L>
struct map_segment {
  /// Hand rolled pair-like type that is trivially copyable.
  template <typename F, typename S>
  struct kv_obj {
    template <typename A, typename B>
    kv_obj(A&& first_, B&& second_) : first(std::forward<A>(first_)), second(std::forward<B>(second_))
    {
    }

    template <typename A, typename... ArgsB>
    kv_obj(A&& first_, ArgsB&&... args_) : first(std::forward<A>(first_)), second(std::forward<ArgsB>(args_)...)
    {
    }

    F first;
    S second;
  };

  using obj_t = kv_obj<K, V>;

  void clear()
  {
    for (auto& e : data) {
      e = std::nullopt;
    }
  }

  std::array<std::optional<obj_t>, L> data;
};

/// \brief Entry in the primary table of segmented_circular_map.
///
/// Lazily holds a pointer to a segment and tracks the number of occupied slots in it.
/// A null segment pointer indicates that no segment has been acquired from the pool yet.
template <typename K, typename V, size_t L>
struct segment_entry {
  map_segment<K, V, L>* segment = nullptr;
  size_t                count   = 0;
};

/// \brief Abstract interface for a shared pool of map_segment objects.
///
/// Implementations manage the lifetime of map_segment instances. get_segment() returns nullptr
/// when the pool is exhausted. Segments returned via return_segment() must have been previously
/// acquired from the same pool.
template <typename K, typename V, size_t L>
class map_segment_pool_interface
{
public:
  virtual ~map_segment_pool_interface() = default;

  /// Acquire a segment from the pool. Returns nullptr if the pool is exhausted.
  virtual map_segment<K, V, L>* get_segment() = 0;

  /// Return a previously acquired segment to the pool.
  virtual void return_segment(map_segment<K, V, L>* seg) = 0;
};

/// \brief Shared memory pool for map_segment objects across multiple value types.
///
/// Maintains a single raw-memory pool where each slot is sized and aligned to hold the
/// largest map_segment<K, V, L> among all V in Vs. Any of the registered value types can
/// draw from the same underlying allocation, keeping total memory usage bounded.
///
/// Use get_map_segment_pool<V>() to obtain a map_segment_pool_interface<K, V, L> suitable
/// for constructing a segmented_circular_map<K, V, L>. The call fails to compile if V was
/// not listed in Vs, guaranteeing at compile time that every user type fits in a pool slot.
///
/// The pool is non-copyable and non-movable; construct it in a stable location (heap or
/// class member) before handing out interface references.
///
/// \tparam K   Key type.
/// \tparam L   Number of slots per segment.
/// \tparam Vs  Value types sharing this pool (non-empty, pairwise distinct).
template <typename K, size_t L, typename... Vs>
class shared_map_segment_pool
{
  static_assert(sizeof...(Vs) > 0, "shared_map_segment_pool requires at least one value type");

  static constexpr size_t slot_size  = std::max({sizeof(map_segment<K, Vs, L>)...});
  static constexpr size_t slot_align = std::max({alignof(map_segment<K, Vs, L>)...});

  struct alignas(slot_align) storage_slot {
    uint8_t data[slot_size];
  };

  template <typename V>
  class typed_adapter : public map_segment_pool_interface<K, V, L>
  {
  public:
    explicit typed_adapter(shared_map_segment_pool& p) noexcept : parent(p) {}

    map_segment<K, V, L>* get_segment() override
    {
      if (parent.free_list.empty()) {
        return nullptr;
      }
      uint8_t* raw = parent.free_list.back();
      parent.free_list.pop_back();
      return ::new (raw) map_segment<K, V, L>();
    }

    void return_segment(map_segment<K, V, L>* seg) override
    {
      seg->~map_segment<K, V, L>();
      parent.free_list.push_back(reinterpret_cast<uint8_t*>(seg));
    }

  private:
    shared_map_segment_pool& parent;
  };

  std::vector<storage_slot>        slots;
  std::vector<uint8_t*>            free_list;
  std::tuple<typed_adapter<Vs>...> adapters;

public:
  explicit shared_map_segment_pool(size_t num_slots) : slots(num_slots), adapters(typed_adapter<Vs>(*this)...)
  {
    free_list.reserve(num_slots);
    for (auto& s : slots) {
      free_list.push_back(s.data);
    }
  }

  ~shared_map_segment_pool() = default;

  shared_map_segment_pool(const shared_map_segment_pool&)            = delete;
  shared_map_segment_pool& operator=(const shared_map_segment_pool&) = delete;
  shared_map_segment_pool(shared_map_segment_pool&&)                 = delete;
  shared_map_segment_pool& operator=(shared_map_segment_pool&&)      = delete;

  /// Returns the pool interface for value type V. Fails to compile if V is not in Vs.
  template <typename V>
  map_segment_pool_interface<K, V, L>& get_pool_of_type()
  {
    static_assert((std::is_same_v<V, Vs> || ...), "Type V was not registered in this shared pool");
    return std::get<typed_adapter<V>>(adapters);
  }
};

/// \brief Contiguous circular map with lazy per-segment allocation from a shared pool.
///
/// Functionally equivalent to circular_map, but storage is split into fixed-size segments of
/// length L that are obtained on-demand from a shared pool and returned when they become empty.
/// This lets many map instances share a common memory pool without per-map pre-allocation.
///
/// Key mapping: flat = K % (num_segments * L), primary_idx = flat / L, slot_idx = flat % L.
/// Collision semantics are identical to circular_map: no resolution, insertion fails on collision.
/// There is no pointer or iterator invalidation.
///
/// \tparam K Key type (must be an unsigned integer).
/// \tparam V Mapped value type.
/// \tparam L Number of slots per segment.
template <typename K, typename V, size_t L>
class segmented_circular_map
{
  static_assert(std::is_integral_v<K> and std::is_unsigned_v<K>, "Container key must be an unsigned integer");
  static_assert(L > 0, "Segment length L must be greater than zero");

public:
  using key_type        = K;
  using mapped_type     = V;
  using value_type      = std::pair<K, V>;
  using difference_type = std::ptrdiff_t;
  using obj_t           = typename map_segment<K, V, L>::obj_t;

  /// Forward iterator that automatically skips absent slots, accelerating over null segments.
  class iterator
  {
  public:
    using iterator_category = std::forward_iterator_tag;
    using value_type        = std::pair<K, V>;
    using difference_type   = std::ptrdiff_t;
    using pointer           = value_type*;
    using reference         = value_type&;

    constexpr iterator() = default;

    constexpr iterator(segmented_circular_map* map, size_t flat_idx_) : ptr(map), flat_idx(flat_idx_)
    {
      if (flat_idx < ptr->total_capacity() and not ptr->is_occupied(flat_idx)) {
        ++(*this);
      }
    }

    constexpr iterator& operator++()
    {
      ++flat_idx;
      while (flat_idx < ptr->total_capacity()) {
        size_t p = flat_idx / L;
        if (ptr->entries[p].segment == nullptr) {
          flat_idx = (p + 1) * L;
        } else if (not ptr->entries[p].segment->data[flat_idx % L]) {
          ++flat_idx;
        } else {
          break;
        }
      }
      return *this;
    }

    constexpr obj_t& operator*()
    {
      ocudu_assert(
          flat_idx < ptr->total_capacity(), "Iterator out-of-bounds ({} >= {})", flat_idx, ptr->total_capacity());
      return ptr->get_obj(flat_idx / L, flat_idx % L);
    }

    constexpr const obj_t& operator*() const
    {
      ocudu_assert(
          flat_idx < ptr->total_capacity(), "Iterator out-of-bounds ({} >= {})", flat_idx, ptr->total_capacity());
      return ptr->get_obj(flat_idx / L, flat_idx % L);
    }

    constexpr obj_t* operator->()
    {
      ocudu_assert(
          flat_idx < ptr->total_capacity(), "Iterator out-of-bounds ({} >= {})", flat_idx, ptr->total_capacity());
      return &ptr->get_obj(flat_idx / L, flat_idx % L);
    }

    constexpr const obj_t* operator->() const
    {
      ocudu_assert(
          flat_idx < ptr->total_capacity(), "Iterator out-of-bounds ({} >= {})", flat_idx, ptr->total_capacity());
      return &ptr->get_obj(flat_idx / L, flat_idx % L);
    }

    constexpr bool operator==(const iterator& other) const { return ptr == other.ptr and flat_idx == other.flat_idx; }

    constexpr bool operator!=(const iterator& other) const { return not(*this == other); }

  private:
    friend class segmented_circular_map;
    segmented_circular_map* ptr      = nullptr;
    size_t                  flat_idx = 0;
  };

  /// Const forward iterator that automatically skips absent slots.
  class const_iterator
  {
  public:
    constexpr const_iterator() = default;

    constexpr const_iterator(const segmented_circular_map* map, size_t flat_idx_) : ptr(map), flat_idx(flat_idx_)
    {
      if (flat_idx < ptr->total_capacity() and not ptr->is_occupied(flat_idx)) {
        ++(*this);
      }
    }

    constexpr const_iterator& operator++()
    {
      ++flat_idx;
      while (flat_idx < ptr->total_capacity()) {
        size_t p = flat_idx / L;
        if (ptr->entries[p].segment == nullptr) {
          flat_idx = (p + 1) * L;
        } else if (not ptr->entries[p].segment->data[flat_idx % L]) {
          ++flat_idx;
        } else {
          break;
        }
      }
      return *this;
    }

    constexpr const obj_t& operator*() const { return ptr->get_obj(flat_idx / L, flat_idx % L); }

    constexpr const obj_t* operator->() const { return &ptr->get_obj(flat_idx / L, flat_idx % L); }

    constexpr bool operator==(const const_iterator& other) const
    {
      return ptr == other.ptr and flat_idx == other.flat_idx;
    }

    constexpr bool operator!=(const const_iterator& other) const { return not(*this == other); }

  private:
    friend class segmented_circular_map;
    const segmented_circular_map* ptr      = nullptr;
    size_t                        flat_idx = 0;
  };

  segmented_circular_map(size_t num_segments, map_segment_pool_interface<K, V, L>& pool_) :
    entries(num_segments), pool(pool_)
  {
    ocudu_assert(num_segments > 0, "segmented_circular_map requires at least one segment slot");
  }

  ~segmented_circular_map() { clear(); }

  segmented_circular_map(const segmented_circular_map&)            = delete;
  segmented_circular_map& operator=(const segmented_circular_map&) = delete;

  segmented_circular_map(segmented_circular_map&& other) noexcept :
    entries(std::move(other.entries)), pool(other.pool), elem_count(std::exchange(other.elem_count, 0U))
  {
  }

  segmented_circular_map& operator=(segmented_circular_map&&) = delete;

  /// Checks if there is an element with the given key in the container.
  constexpr bool contains(K key) const noexcept
  {
    size_t      flat = get_flat_idx(key);
    size_t      p    = flat / L;
    size_t      s    = flat % L;
    const auto* seg  = entries[p].segment;
    return seg != nullptr and seg->data[s] and seg->data[s]->first == key;
  }

  /// Inserts a new element constructed in-place with the given key if no collision is detected.
  /// Returns false on collision or pool exhaustion.
  template <typename... Args>
  constexpr bool emplace(K key, Args&&... args)
  {
    static_assert(std::is_constructible_v<V, Args...>, "Invalid argument types");
    size_t flat = get_flat_idx(key);
    size_t p    = flat / L;
    size_t s    = flat % L;
    if (entries[p].segment == nullptr) {
      entries[p].segment = pool.get_segment();
      if (entries[p].segment == nullptr) {
        return false;
      }
    } else if (entries[p].segment->data[s]) {
      return false;
    }
    entries[p].segment->data[s].emplace(key, std::forward<Args>(args)...);
    ++entries[p].count;
    ++elem_count;
    return true;
  }

  /// Inserts a new element with the given key if no collision is detected (lvalue version).
  /// Returns false on collision or pool exhaustion.
  constexpr bool insert(K key, const V& obj)
  {
    size_t flat = get_flat_idx(key);
    size_t p    = flat / L;
    size_t s    = flat % L;
    if (entries[p].segment == nullptr) {
      entries[p].segment = pool.get_segment();
      if (entries[p].segment == nullptr) {
        return false;
      }
    } else if (entries[p].segment->data[s]) {
      return false;
    }
    entries[p].segment->data[s].emplace(key, obj);
    ++entries[p].count;
    ++elem_count;
    return true;
  }

  /// Inserts a new element with the given key if no collision is detected (rvalue version).
  /// Returns an iterator to the inserted element or the object back on failure.
  constexpr expected<iterator, V> insert(K key, V&& obj)
  {
    size_t flat = get_flat_idx(key);
    size_t p    = flat / L;
    size_t s    = flat % L;
    if (entries[p].segment == nullptr) {
      entries[p].segment = pool.get_segment();
      if (entries[p].segment == nullptr) {
        return make_unexpected(std::move(obj));
      }
    } else if (entries[p].segment->data[s]) {
      return make_unexpected(std::move(obj));
    }
    entries[p].segment->data[s].emplace(key, std::move(obj));
    ++entries[p].count;
    ++elem_count;
    return iterator(this, flat);
  }

  /// Inserts a new element with the given key, overwriting any existing occupant at that slot.
  template <typename U>
  constexpr void overwrite(K key, U&& obj)
  {
    size_t flat = get_flat_idx(key);
    size_t p    = flat / L;
    size_t s    = flat % L;
    if (entries[p].segment != nullptr and entries[p].segment->data[s]) {
      erase(get_obj(p, s).first);
    }
    insert(key, std::forward<U>(obj));
  }

  /// Removes the element with the given key. Returns false if not found.
  constexpr bool erase(K key) noexcept
  {
    if (not contains(key)) {
      return false;
    }
    size_t flat = get_flat_idx(key);
    size_t p    = flat / L;
    size_t s    = flat % L;
    entries[p].segment->data[s].reset();
    --entries[p].count;
    --elem_count;
    maybe_return_segment(p);
    return true;
  }

  /// Removes the element pointed to by the iterator. Returns the next iterator.
  constexpr iterator erase(iterator it) noexcept
  {
    ocudu_assert(it.flat_idx < total_capacity() and it.ptr == this,
                 "Iterator out-of-bounds ({} >= {})",
                 it.flat_idx,
                 total_capacity());
    iterator next = it;
    ++next;
    size_t p = it.flat_idx / L;
    size_t s = it.flat_idx % L;
    entries[p].segment->data[s].reset();
    --entries[p].count;
    --elem_count;
    maybe_return_segment(p);
    return next;
  }

  /// Erases all elements and returns all segments to the pool.
  constexpr void clear() noexcept
  {
    for (auto& entry : entries) {
      if (entry.segment != nullptr) {
        entry.segment->clear();
        pool.return_segment(entry.segment);
        entry.segment = nullptr;
        entry.count   = 0;
      }
    }
    elem_count = 0;
  }

  /// Returns a reference to the value mapped to the given key. Asserts if not present.
  constexpr V& operator[](K key) noexcept
  {
    ocudu_assert(contains(key), "Accessing non-existent ID={}", (size_t)key);
    size_t flat = get_flat_idx(key);
    return get_obj(flat / L, flat % L).second;
  }

  /// Returns a const reference to the value mapped to the given key. Asserts if not present.
  constexpr const V& operator[](K key) const noexcept
  {
    ocudu_assert(contains(key), "Accessing non-existent ID={}", (size_t)key);
    size_t flat = get_flat_idx(key);
    return get_obj(flat / L, flat % L).second;
  }

  /// Returns the number of elements in the container.
  constexpr size_t size() const noexcept { return elem_count; }

  /// Checks if the container has no elements.
  constexpr bool empty() const noexcept { return elem_count == 0; }

  /// Checks if the container has reached its maximum capacity.
  constexpr bool full() const noexcept { return elem_count == capacity(); }

  /// Returns the maximum capacity of the container.
  constexpr size_t capacity() const noexcept { return total_capacity(); }

  /// Checks if the slot for the given key is free (either segment absent or slot unoccupied).
  constexpr bool has_space(K key) const noexcept
  {
    size_t flat = get_flat_idx(key);
    size_t p    = flat / L;
    size_t s    = flat % L;
    return entries[p].segment == nullptr or not entries[p].segment->data[s];
  }

  constexpr iterator       begin() { return iterator(this, 0); }
  constexpr iterator       end() { return iterator(this, total_capacity()); }
  constexpr const_iterator begin() const { return const_iterator(this, 0); }
  constexpr const_iterator end() const { return const_iterator(this, total_capacity()); }

  /// Finds an element with the given key.
  constexpr iterator find(K key)
  {
    if (contains(key)) {
      return iterator(this, get_flat_idx(key));
    }
    return end();
  }

  /// Finds an element with the given key.
  constexpr const_iterator find(K key) const
  {
    if (contains(key)) {
      return const_iterator(this, get_flat_idx(key));
    }
    return end();
  }

private:
  size_t total_capacity() const noexcept { return entries.size() * L; }
  size_t get_flat_idx(K key) const noexcept { return static_cast<size_t>(key) % total_capacity(); }

  obj_t&       get_obj(size_t primary, size_t slot) { return *entries[primary].segment->data[slot]; }
  const obj_t& get_obj(size_t primary, size_t slot) const { return *entries[primary].segment->data[slot]; }

  bool is_occupied(size_t flat) const noexcept
  {
    size_t p = flat / L;
    size_t s = flat % L;
    return entries[p].segment != nullptr and entries[p].segment->data[s].has_value();
  }

  void maybe_return_segment(size_t primary) noexcept
  {
    if (entries[primary].count == 0) {
      entries[primary].segment->clear();
      pool.return_segment(entries[primary].segment);
      entries[primary].segment = nullptr;
    }
  }

  std::vector<segment_entry<K, V, L>>  entries;
  map_segment_pool_interface<K, V, L>& pool;
  size_t                               elem_count = 0;
};

} // namespace ocudu
