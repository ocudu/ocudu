// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/span.h"
#include "ocudu/adt/static_vector.h"
#include "ocudu/support/detail/type_list.h"
#include "ocudu/support/memory_pool/free_list_memory_pool.h"
#include "ocudu/support/ocudu_assert.h"
#include <algorithm>
#include <cstddef>
#include <iterator>
#include <utility>
#include <vector>

namespace ocudu {

/// Storage, views and builders of the PDU lists of a scheduler result, independent of the PDU types held in them.
namespace sched_pdu_detail {

/// Iterator that yields the PDU a handle points to.
template <typename T, typename Handle>
class pdu_handle_iterator
{
public:
  using value_type        = std::remove_const_t<T>;
  using difference_type   = std::ptrdiff_t;
  using reference         = T&;
  using pointer           = T*;
  using iterator_category = std::forward_iterator_tag;

  pdu_handle_iterator() = default;
  explicit pdu_handle_iterator(Handle* handle_) : handle(handle_) {}

  operator pdu_handle_iterator<const T, const Handle>() const
  {
    return pdu_handle_iterator<const T, const Handle>{handle};
  }

  reference operator*() const { return **handle; }
  pointer   operator->() const { return handle->get(); }

  pdu_handle_iterator& operator++()
  {
    ++handle;
    return *this;
  }
  pdu_handle_iterator operator++(int)
  {
    pdu_handle_iterator tmp = *this;
    ++handle;
    return tmp;
  }

  bool operator==(const pdu_handle_iterator& other) const { return handle == other.handle; }
  bool operator!=(const pdu_handle_iterator& other) const { return handle != other.handle; }

  /// Fetches the handle the iterator points at.
  Handle* base() const { return handle; }

private:
  Handle* handle = nullptr;
};

} // namespace sched_pdu_detail

/// \brief List that holds the PDUs of a given type in a pool shared by every slot of the grid.
///
/// The list stores one handle per PDU, so that the PDU itself lives in the pool and not in the slot. Access to the
/// PDUs is the same as in a list that holds them inline, except that they are not contiguous in memory.
template <typename T>
class pooled_pdu_list
{
public:
  using handle_type    = typename free_list_object_pool<T>::ptr;
  using handle_list    = std::vector<handle_type>;
  using value_type     = T;
  using iterator       = sched_pdu_detail::pdu_handle_iterator<T, handle_type>;
  using const_iterator = sched_pdu_detail::pdu_handle_iterator<const T, const handle_type>;

  [[nodiscard]] size_t size() const { return handle_lst.size(); }
  [[nodiscard]] bool   empty() const { return handle_lst.empty(); }
  [[nodiscard]] size_t capacity() const { return handle_lst.capacity(); }

  T&       operator[](size_t i) { return *handle_lst[i]; }
  const T& operator[](size_t i) const { return *handle_lst[i]; }
  T&       front() { return *handle_lst.front(); }
  const T& front() const { return *handle_lst.front(); }
  T&       back() { return *handle_lst.back(); }
  const T& back() const { return *handle_lst.back(); }

  iterator       begin() { return iterator{handle_lst.data()}; }
  iterator       end() { return iterator{handle_lst.data() + handle_lst.size()}; }
  const_iterator begin() const { return const_iterator{handle_lst.data()}; }
  const_iterator end() const { return const_iterator{handle_lst.data() + handle_lst.size()}; }

  /// Releases every PDU of the list back to its pool.
  void clear() { handle_lst.clear(); }

  /// Releases the PDU an iterator points at back to its pool.
  iterator erase(iterator it)
  {
    return iterator{handle_lst.data() +
                    (handle_lst.erase(handle_lst.begin() + (it.base() - handle_lst.data())) - handle_lst.begin())};
  }

  /// \brief Reserves room for the given number of PDUs.
  ///
  /// Called once, before any PDU is added, so that adding one never allocates.
  void reserve(size_t nof_pdus) { handle_lst.reserve(nof_pdus); }

  /// Fetches the handles of the PDUs the list holds.
  handle_list&       handles() { return handle_lst; }
  const handle_list& handles() const { return handle_lst; }

private:
  handle_list handle_lst;
};

namespace sched_pdu_detail {

/// Read-only view over a list of PDUs held in a pool shared by every slot of the grid.
template <typename T>
class pooled_list_view
{
  using handle_type = typename free_list_object_pool<T>::ptr;

public:
  using value_type = T;

  /// Iterator over the PDUs of the list.
  class const_iterator
  {
  public:
    using value_type        = T;
    using difference_type   = std::ptrdiff_t;
    using reference         = const T&;
    using pointer           = const T*;
    using iterator_category = std::forward_iterator_tag;

    const_iterator() = default;
    explicit const_iterator(const handle_type* handle_) : handle(handle_) {}

    reference operator*() const { return **handle; }
    pointer   operator->() const { return handle->get(); }

    const_iterator& operator++()
    {
      ++handle;
      return *this;
    }
    const_iterator operator++(int)
    {
      const_iterator tmp = *this;
      ++handle;
      return tmp;
    }

    bool operator==(const const_iterator& other) const { return handle == other.handle; }
    bool operator!=(const const_iterator& other) const { return handle != other.handle; }

  private:
    const handle_type* handle = nullptr;
  };

  pooled_list_view() = default;
  explicit pooled_list_view(span<const handle_type> handles_) : handles(handles_) {}

  pooled_list_view(const pooled_pdu_list<T>& list) : handles(list.handles()) {}

  [[nodiscard]] size_t size() const { return handles.size(); }
  [[nodiscard]] bool   empty() const { return handles.empty(); }

  const T& operator[](size_t i) const { return *handles[i]; }
  const T& front() const { return *handles.front(); }
  const T& back() const { return *handles.back(); }

  /// Returns a view over the last \c n PDUs of the list.
  pooled_list_view last(size_t n) const { return pooled_list_view{handles.last(n)}; }

  const_iterator begin() const { return const_iterator{handles.begin()}; }
  const_iterator end() const { return const_iterator{handles.end()}; }

private:
  span<const handle_type> handles;
};

/// Maps the storage of a PDU list onto the view that reads it.
template <typename Storage>
struct get_list_view {
  static_assert(sizeof(Storage) == 0, "Type is not the storage of a scheduler result PDU list.");
};

template <typename T, size_t N>
struct get_list_view<static_vector<T, N>> {
  using type = span<const T>;
};

template <typename T>
struct get_list_view<pooled_pdu_list<T>> {
  using type = pooled_list_view<T>;
};

/// Handle to build a list of scheduler result PDUs held inline in the slot.
template <typename T, size_t N>
class static_list_builder
{
public:
  using value_type   = T;
  using storage_type = static_vector<T, N>;
  using iterator     = typename storage_type::iterator;

  /// The pool set is ignored, as the PDUs of this list live in the slot.
  template <typename PoolSet>
  static_list_builder(storage_type& list_, PoolSet& /* unused */) : list(&list_)
  {
  }

  [[nodiscard]] bool   empty() const { return list->empty(); }
  [[nodiscard]] bool   full() const { return space_left() == 0; }
  [[nodiscard]] size_t size() const { return list->size(); }

  /// Number of PDUs the list holds at most.
  [[nodiscard]] size_t capacity() const { return list->capacity(); }

  /// Number of PDUs that still fit in the list.
  [[nodiscard]] size_t space_left() const { return list->capacity() - list->size(); }

  /// Constructs a PDU at the end of the list. Returns nullptr if no further PDU fits.
  template <typename... Args>
  T* emplace_back(Args&&... args)
  {
    if (full()) {
      return nullptr;
    }
    return &list->emplace_back(std::forward<Args>(args)...);
  }

  /// Removes the last PDU of the list.
  void pop_back() { list->pop_back(); }

  /// Removes the PDU an iterator points at, and returns an iterator to the one that followed it.
  iterator erase(iterator it) { return list->erase(it); }

  T& operator[](size_t i) { return (*list)[i]; }
  T& back() { return list->back(); }

  iterator begin() { return list->begin(); }
  iterator end() { return list->end(); }

  /// Returns a read-only view over the PDUs built so far.
  span<const T> view() const { return *list; }

private:
  storage_type* list;
};

/// \brief Handle to build a list of scheduler result PDUs held in a pool shared by every slot of the grid.
///
/// The slot holds one handle per PDU, which releases the PDU back to the pool when the list drops it.
template <typename T>
class pooled_list_builder
{
  using handle_type = typename free_list_object_pool<T>::ptr;

public:
  using value_type   = T;
  using storage_type = pooled_pdu_list<T>;

  using iterator = typename storage_type::iterator;

  /// The pool set provides the pool of the PDU type through a \c get() member template.
  template <typename PoolSet>
  pooled_list_builder(storage_type& list_, PoolSet& pools) : list(&list_), pool(&pools.template get<T>())
  {
  }

  [[nodiscard]] bool   empty() const { return list->empty(); }
  [[nodiscard]] bool   full() const { return space_left() == 0; }
  [[nodiscard]] size_t size() const { return list->size(); }

  /// Number of PDUs the list holds at most.
  [[nodiscard]] size_t capacity() const { return list->capacity(); }

  /// Number of PDUs that still fit in the list, limited by the PDUs the pool has left.
  [[nodiscard]] size_t space_left() const
  {
    return std::min(list->capacity() - list->size(), pool->nof_objects_available());
  }

  /// Constructs a PDU at the end of the list. Returns nullptr if the list is full or the pool is exhausted.
  template <typename... Args>
  T* emplace_back(Args&&... args)
  {
    if (full()) {
      return nullptr;
    }
    handle_type handle = pool->get(std::forward<Args>(args)...);
    if (handle == nullptr) {
      return nullptr;
    }
    T* pdu = handle.get();
    list->handles().push_back(std::move(handle));
    return pdu;
  }

  /// Removes the last PDU of the list, releasing it back to the pool.
  void pop_back() { list->handles().pop_back(); }

  /// Removes the PDU an iterator points at, and returns an iterator to the one that followed it.
  iterator erase(iterator it) { return list->erase(it); }

  T& operator[](size_t i) { return (*list)[i]; }
  T& back() { return list->back(); }

  iterator begin() { return list->begin(); }
  iterator end() { return list->end(); }

  /// Returns a read-only view over the PDUs built so far.
  pooled_list_view<T> view() const { return pooled_list_view<T>{span<const handle_type>{list->handles()}}; }

private:
  storage_type*             list;
  free_list_object_pool<T>* pool;
};

/// Maps the storage of a PDU list onto the builder that fills it.
template <typename Storage>
struct get_list_builder {
  static_assert(sizeof(Storage) == 0, "Type is not the storage of a scheduler result PDU list.");
};

template <typename T, size_t N>
struct get_list_builder<static_vector<T, N>> {
  using type = static_list_builder<T, N>;
};

template <typename T>
struct get_list_builder<pooled_pdu_list<T>> {
  using type = pooled_list_builder<T>;
};

/// \brief Finds, among the storages of a slot, the one that holds the PDUs of a given type.
///
/// The PDU type must be held by exactly one of the listed storages.
template <typename T, typename StorageList>
struct find_list_storage;

template <typename T, typename... Storages>
struct find_list_storage<T, type_list<Storages...>> {
private:
  template <typename Storage>
  using value_type_of = typename get_list_builder<Storage>::type::value_type;

  static constexpr size_t nof_matches = (static_cast<size_t>(std::is_same_v<T, value_type_of<Storages>>) + ...);
  static_assert(nof_matches != 0, "Type is not a scheduler result PDU.");
  static_assert(nof_matches == 1, "More than one scheduler result list holds PDUs of this type.");

public:
  using type = type_list_helper::type_at_t<type_list_helper::type_index_v<T, value_type_of<Storages>...>,
                                           type_list<Storages...>>;
};

} // namespace sched_pdu_detail

} // namespace ocudu
