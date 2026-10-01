// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/adt/span.h"
#include "ocudu/adt/strong_type.h"
#include "ocudu/support/ocudu_assert.h"
#include <limits>
#include <vector>

namespace ocudu {

/// Type representing an ID of a stable_id_map.
struct stable_id_tag;
using stable_id_t = strong_type<uint32_t, stable_id_tag, strong_equality, strong_comparison>;

/// \brief List of objects that have a stable ID and are represented contiguously in memory, but their address in the
/// internal storage may change.
template <typename T>
class stable_id_map
{
  struct sentinel {};

  /// Iterator of objects. The iterator is not sorted by stable ID.
  template <typename TableType>
  class iter_impl
  {
    constexpr static bool is_const = std::is_const_v<TableType>;

  public:
    using value_type        = T;
    using difference_type   = std::ptrdiff_t;
    using reference         = std::conditional_t<is_const, const T&, T&>;
    using pointer           = std::conditional_t<is_const, const T*, T*>;
    using iterator_category = std::forward_iterator_tag;

    iter_impl() = default;
    iter_impl(TableType& table_, size_t offset_) : parent(&table_), offset(offset_) {}

    operator iter_impl<const stable_id_map>() const noexcept { return iter_impl<const stable_id_map>{*parent, offset}; }

    reference       operator*() { return parent->objs[offset]; }
    const reference operator*() const { return parent->objs[offset]; }
    pointer         operator->() { return &parent->objs[offset]; }
    const pointer   operator->() const { return &parent->objs[offset]; }

    iter_impl& operator++()
    {
      ++offset;
      return *this;
    }
    iter_impl operator++(int)
    {
      iter_impl tmp = *this;
      ++offset;
      return tmp;
    }

    bool        operator==(const iter_impl& other) const { return offset == other.offset and parent == other.parent; }
    bool        operator!=(const iter_impl& other) const { return not(*this == other); }
    bool        operator==(sentinel /* unused */) const { return offset == parent->size(); }
    bool        operator!=(sentinel /* unused */) const { return offset != parent->size(); }
    friend bool operator==(sentinel /* unused */, const iter_impl& rhs) { return rhs.offset == rhs.parent->size(); }
    friend bool operator!=(sentinel /* unused */, const iter_impl& rhs) { return rhs.offset != rhs.parent->size(); }

  private:
    friend class stable_id_map<T>;

    TableType* parent = nullptr;
    unsigned   offset = 0;
  };

public:
  using value_type     = T;
  using iterator       = iter_impl<stable_id_map>;
  using const_iterator = iter_impl<const stable_id_map>;

  /// Retrieves the number of objects currently stored.
  [[nodiscard]] size_t size() const { return index_reverse_map.size(); }

  /// Checks if the table contains any objects.
  [[nodiscard]] bool empty() const { return index_reverse_map.empty(); }

  /// Gets pre-reserved space.
  [[nodiscard]] size_t capacity() const { return index_reverse_map.capacity(); }

  /// Checks whether the table contains object with provided ID.
  [[nodiscard]] bool contains(stable_id_t id) const
  {
    if (id.value() >= index_map.size()) {
      return false;
    }
    const unsigned offset = index_map[id.value()];
    if (offset >= index_reverse_map.size()) {
      return false;
    }
    return index_reverse_map[offset] == id;
  }

  /// Retrieves an object with the provided ID.
  T& at(stable_id_t id)
  {
    ocudu_assert(contains(id), "Invalid stable ID");
    return objs[index_map[id.value()]];
  }
  T& operator[](stable_id_t id)
  {
    ocudu_assert(contains(id), "Invalid stable ID");
    return objs[index_map[id.value()]];
  }
  const T& at(stable_id_t id) const
  {
    ocudu_assert(contains(id), "Invalid stable ID");
    return objs[index_map[id.value()]];
  }
  const T& operator[](stable_id_t id) const
  {
    ocudu_assert(contains(id), "Invalid stable ID");
    return objs[index_map[id.value()]];
  }

  /// Get the internal offset of the object with given ID.
  unsigned get_offset(stable_id_t id) const
  {
    ocudu_assert(contains(id), "Invalid stable_id");
    return index_map[id.value()];
  }

  template <typename... Args>
  stable_id_t emplace(Args&&... args)
  {
    static_assert((std::is_constructible_v<T, Args&&...>),
                  "Argument types must be constructible into corresponding vector::value_type.");
    unsigned offset    = index_reverse_map.size();
    unsigned chosen_id = free_list_head;
    if (chosen_id < index_map.size()) {
      free_list_head       = index_map[chosen_id];
      index_map[chosen_id] = offset;
    } else {
      ocudu_sanity_check(chosen_id == index_map.size(), "Invalid free_list_head");
      index_map.push_back(offset);
      free_list_head = chosen_id + 1;
    }
    index_reverse_map.emplace_back(chosen_id);
    objs.emplace_back(std::forward<Args>(args)...);
    return stable_id_t{chosen_id};
  }

  template <typename U>
  stable_id_t insert(U&& obj)
  {
    return emplace(std::forward<U>(obj));
  }

  bool erase(stable_id_t rem_id)
  {
    if (rem_id.value() >= index_map.size()) {
      return false;
    }
    const unsigned offset = index_map[rem_id.value()];
    if (offset >= index_reverse_map.size()) {
      return false;
    }
    ocudu_sanity_check(index_reverse_map[offset] == rem_id, "Invalid stable_id");

    if (offset != index_reverse_map.size() - 1) {
      std::swap(objs[offset], objs.back());
      index_reverse_map[offset]                    = index_reverse_map.back();
      index_map[index_reverse_map[offset].value()] = offset;
    }
    objs.pop_back();
    index_reverse_map.pop_back();
    index_map[rem_id.value()] = free_list_head;
    free_list_head            = rem_id.value();
    return true;
  }

  void erase(const_iterator it)
  {
    ocudu_assert(it.parent == this, "Iterator does not belong to this table");
    ocudu_assert(it.offset < size(), "Iterator out of range");
    erase(index_reverse_map[it.offset]);
  }

  void clear()
  {
    objs.clear();
    index_map.clear();
    index_reverse_map.clear();
    free_list_head = 0;
  }

  void reserve(unsigned size)
  {
    objs.reserve(size);
    index_map.reserve(size);
    index_reverse_map.reserve(size);
  }

  span<const T> unsorted() const { return objs; }
  span<T>       unsorted() { return objs; }

  /// Get iterator of elements pointing to the beginning. Note: this iterator is not sorted by stable_id.
  iterator       begin() { return iterator{*this, 0}; }
  const_iterator begin() const { return const_iterator{*this, 0}; }
  const_iterator cbegin() const { return const_iterator{*this, 0}; }
  sentinel       end() const { return {}; }
  sentinel       cend() const { return {}; }

private:
  /// List of objects in contiguous memory storage.
  std::vector<T> objs;
  /// This vector serves two purposes:
  /// - for stable_ids under use, it maps the stable_id (the index of index_map) to an index/offset in the objs vector
  /// - for stable_ids not under use, it serves as the next stable_id in an intrusive linked list of free stable_ids.
  std::vector<unsigned> index_map;
  /// Map from offset/index of the objs vector back to its respective stable_id.
  std::vector<stable_id_t> index_reverse_map;
  /// Intrusive linked list of free stable_ids (using holes of index_map as the node next stable_id element).
  unsigned free_list_head{0};
};

/// Intrusive forward linked list of elements of a stable_id_map.
template <auto NextMember>
class stable_id_intrusive_list
{
  /// Metafunction to deduce node type from the member pointer. Only matches stable_id_t T::*.
  template <typename T>
  struct member_ptr_class;
  template <typename T>
  struct member_ptr_class<stable_id_t T::*> {
    using type = T;
  };
  constexpr static stable_id_t invalid_stable_id{stable_id_t{std::numeric_limits<uint32_t>::max()}};

  struct sentinel {};

public:
  using value_type = typename member_ptr_class<decltype(NextMember)>::type;
  using map_type   = stable_id_map<value_type>;

private:
  template <typename Map>
  struct iter_impl {
    static constexpr bool is_const = std::is_const_v<Map>;
    using difference_type          = std::ptrdiff_t;
    using reference                = std::conditional_t<is_const, const value_type&, value_type&>;
    using pointer                  = std::conditional_t<is_const, const value_type*, value_type*>;
    using iterator_category        = std::forward_iterator_tag;

    iter_impl() = default;
    iter_impl(Map& map_, stable_id_t cur_id_) : map(&map_), cur_id(cur_id_) {}

    operator iter_impl<const map_type>() const noexcept { return iter_impl<const map_type>{*map, cur_id}; }

    const value_type* operator->() const { return &map->at(cur_id); }
    const value_type& operator*() const { return map->at(cur_id); }

    iter_impl& operator++()
    {
      cur_id = map->at(cur_id).*NextMember;
      return *this;
    }
    iter_impl operator++(int)
    {
      iter_impl tmp = *this;
      ++(*this);
      return tmp;
    }

    stable_id_t id() const { return cur_id; }

    bool        operator==(const iter_impl& other) const { return cur_id == other.cur_id and map == other.map; }
    bool        operator!=(const iter_impl& other) const { return not(*this == other); }
    bool        operator==(sentinel /* unused */) const { return cur_id == invalid_stable_id; }
    bool        operator!=(sentinel /* unused */) const { return cur_id != invalid_stable_id; }
    friend bool operator==(sentinel /* unused */, const iter_impl& rhs) { return rhs.cur_id == invalid_stable_id; }
    friend bool operator!=(sentinel /* unused */, const iter_impl& rhs) { return rhs.cur_id != invalid_stable_id; }

  private:
    Map*        map    = nullptr;
    stable_id_t cur_id = invalid_stable_id;
  };

public:
  /// Mutable view of the intrusive list bound to a map. Supports push/pop and iteration.
  struct range {
    using iterator = iter_impl<map_type>;

    range(map_type& map_, stable_id_t& head_) : map(&map_), head(&head_) {}

    /// Iterator to the element before the head. Only valid as argument to erase_after and insert_after.
    iterator before_begin() const { return iterator{*map, invalid_stable_id}; }
    iterator begin() const { return iterator{*map, *head}; }
    sentinel end() const { return sentinel{}; }
    bool     empty() const { return *head == invalid_stable_id; }

    /// Prepend an element to the front of the list.
    void push_front(stable_id_t id)
    {
      (*map)[id].*NextMember = *head;
      *head                  = id;
    }

    /// Remove and return the front element of the list.
    stable_id_t pop_front()
    {
      ocudu_assert(*head != invalid_stable_id, "Cannot pop from empty list");
      stable_id_t id = *head;
      *head          = (*map)[id].*NextMember;
      return id;
    }

    /// Erase an element by stable ID. Returns true if the element was found and removed.
    bool erase(stable_id_t id)
    {
      if (*head == id) {
        *head = (*map)[id].*NextMember;
        return true;
      }
      for (stable_id_t prev = *head; prev != invalid_stable_id; prev = (*map)[prev].*NextMember) {
        if ((*map)[prev].*NextMember == id) {
          (*map)[prev].*NextMember = (*map)[id].*NextMember;
          return true;
        }
      }
      return false;
    }

    /// Erase the element pointed to by an iterator. Returns true if the element was found and removed.
    bool erase(iterator it) { return erase(it.id()); }

    /// Insert an element after the given position. Use before_begin() to insert at the head.
    void insert_after(iterator prev, stable_id_t id)
    {
      if (prev.id() == invalid_stable_id) {
        push_front(id);
      } else {
        (*map)[id].*NextMember        = (*map)[prev.id()].*NextMember;
        (*map)[prev.id()].*NextMember = id;
      }
    }

    /// Erase the element after prev (O(1)). Returns the stable ID of the erased element.
    /// Use before_begin() as prev to erase the head.
    stable_id_t erase_after(iterator prev)
    {
      stable_id_t erased_id;
      if (prev.id() == invalid_stable_id) {
        erased_id = *head;
        *head     = (*map)[erased_id].*NextMember;
      } else {
        erased_id                     = (*map)[prev.id()].*NextMember;
        (*map)[prev.id()].*NextMember = (*map)[erased_id].*NextMember;
      }
      return erased_id;
    }

  private:
    map_type*    map;
    stable_id_t* head;
  };

  /// Read-only view of the intrusive list bound to a const map.
  struct const_range {
    using const_iterator = iter_impl<const map_type>;

    const_range(const map_type& map_, const stable_id_t& head_) : map(&map_), head(head_) {}

    const_iterator begin() const { return const_iterator{*map, head}; }
    sentinel       end() const { return sentinel{}; }
    bool           empty() const { return head == invalid_stable_id; }

  private:
    const map_type* map;
    stable_id_t     head;
  };

  /// Whether the linked list is empty.
  bool empty() const { return free_list_head == invalid_stable_id; }

  range       get_list(map_type& map) { return range{map, free_list_head}; }
  const_range get_list(const map_type& map) const { return const_range{map, free_list_head}; }

private:
  /// Head element stable ID of the forward linked list.
  stable_id_t free_list_head = invalid_stable_id;
};

} // namespace ocudu
