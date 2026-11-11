// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief Arrays, copy_ptr and CHOICE storage of the ASN.1 types.

#pragma once

#include "ocudu/asn1/asn1_bit_ref.h"
#include "ocudu/support/ocudu_assert.h"
#include <algorithm>
#include <any>
#include <array>
#include <cstddef>
#include <new>
#include <type_traits>
#include <utility>

namespace asn1 {

template <class Integer>
constexpr Integer ceil_frac(Integer n, Integer d)
{
  return (n + (d - 1)) / d;
}

namespace detail {

constexpr std::size_t max_size_impl(std::size_t lhs, std::size_t rhs)
{
  return lhs < rhs ? rhs : lhs;
}

template <typename ItBegin, typename ItEnd>
constexpr std::size_t max_array_impl(ItBegin b, ItEnd e)
{
  return b + 1 == e ? *b : max_size_impl(*b, max_array_impl(b + 1, e));
}

} // namespace detail

/// \brief Compute maximum value of an array of std::size.
template <std::size_t N>
constexpr std::size_t max_size(const std::size_t (&arr)[N])
{
  return detail::max_array_impl(arr, arr + N);
}

/*********************
  arrays
*********************/
namespace detail {

/// Element operations of a dyn_array. A null operation means that the element type is trivial for it.
struct dyn_array_ops {
  uint32_t elem_size;
  /// Value-initializes elements.
  void (*construct)(void* dst, uint32_t n);
  /// Copy-constructs elements.
  void (*copy)(void* dst, const void* src, uint32_t n);
  /// Move-constructs elements and destroys their sources. Ranges may overlap when dst precedes src.
  void (*relocate)(void* dst, void* src, uint32_t n);
  /// Destroys elements.
  void (*destroy)(void* ptr, uint32_t n);
};

/// Storage and growth of a dyn_array, independent of the element type.
class dyn_array_base
{
public:
  uint32_t size() const { return size_; }
  uint32_t capacity() const { return cap_; }

protected:
  dyn_array_base() = default;
  dyn_array_base(dyn_array_base&& other) noexcept : data_(other.data_), size_(other.size_), cap_(other.cap_)
  {
    other.data_ = nullptr;
    other.size_ = 0;
    other.cap_  = 0;
  }
  dyn_array_base(const dyn_array_base&)            = delete;
  dyn_array_base& operator=(const dyn_array_base&) = delete;
  dyn_array_base& operator=(dyn_array_base&&)      = delete;
  ~dyn_array_base()                                = default;

  void assign_impl(const dyn_array_ops& ops, const void* src, uint32_t nof_items);
  void move_impl(const dyn_array_ops& ops, dyn_array_base& other);
  void reserve_impl(const dyn_array_ops& ops, uint32_t new_cap);
  void resize_impl(const dyn_array_ops& ops, uint32_t new_size, uint32_t new_cap);
  void erase_impl(const dyn_array_ops& ops, uint32_t idx);
  void release_impl(const dyn_array_ops& ops);

  void*    data_ = nullptr;
  uint32_t size_ = 0;
  uint32_t cap_  = 0;

private:
  void reallocate(const dyn_array_ops& ops, uint32_t new_cap);
};

} // namespace detail

template <class T>
class dyn_array : private detail::dyn_array_base
{
  static_assert(alignof(T) <= __STDCPP_DEFAULT_NEW_ALIGNMENT__, "Over-aligned dyn_array elements are not supported");

public:
  typedef T value_type;
  using iterator       = T*;
  using const_iterator = const T*;

  dyn_array() = default;
  explicit dyn_array(uint32_t new_size) { resize_impl(ops, new_size, 0); }
  dyn_array(const dyn_array<T>& other) : dyn_array(other.data(), other.size()) {}
  dyn_array(dyn_array<T>&& other) noexcept : detail::dyn_array_base(std::move(other)) {}
  dyn_array(const T* ptr, uint32_t nof_items) { assign_impl(ops, ptr, nof_items); }
  ~dyn_array() { release_impl(ops); }

  using detail::dyn_array_base::capacity;
  using detail::dyn_array_base::size;

  T& operator[](uint32_t idx)
  {
    ocudu_assert(idx < size(), "out-of-bounds access to dyn_array ({} >= {})", idx, size());
    return data()[idx];
  }
  const T& operator[](uint32_t idx) const
  {
    ocudu_assert(idx < size(), "out-of-bounds access to dyn_array ({} >= {})", idx, size());
    return data()[idx];
  }
  dyn_array<T>& operator=(const dyn_array<T>& other)
  {
    if (this != &other) {
      assign_impl(ops, other.data(), other.size());
    }
    return *this;
  }
  dyn_array<T>& operator=(dyn_array<T>&& other) noexcept
  {
    if (this != &other) {
      move_impl(ops, other);
    }
    return *this;
  }
  void     reserve(uint32_t new_cap) { reserve_impl(ops, new_cap); }
  void     resize(uint32_t new_size, uint32_t new_cap = 0) { resize_impl(ops, new_size, new_cap); }
  iterator erase(iterator it)
  {
    if (it < begin() or it >= end()) {
      log_warning("Trying to access out-of-bounds iterator.");
      return end();
    }
    uint32_t idx = it - begin();
    erase_impl(ops, idx);
    return begin() + idx;
  }
  void push_back(const T& elem)
  {
    if (size_ == cap_) {
      // The element may live in the storage that is about to be reallocated.
      T tmp(elem);
      reserve_impl(ops, size_ * 2 > size_ + 1 ? size_ * 2 : size_ + 1);
      new (data() + size_) T(std::move(tmp));
    } else {
      new (data() + size_) T(elem);
    }
    ++size_;
  }
  void push_back(T&& elem)
  {
    if (size_ == cap_) {
      // The element may live in the storage that is about to be reallocated.
      T tmp(std::move(elem));
      reserve_impl(ops, size_ * 2 > size_ + 1 ? size_ * 2 : size_ + 1);
      new (data() + size_) T(std::move(tmp));
    } else {
      new (data() + size_) T(std::move(elem));
    }
    ++size_;
  }
  void           clear() { resize_impl(ops, 0, 0); }
  T&             back() { return (*this)[size() - 1]; }
  const T&       back() const { return (*this)[size() - 1]; }
  T*             data() { return static_cast<T*>(data_); }
  const T*       data() const { return static_cast<const T*>(data_); }
  iterator       begin() { return data(); }
  iterator       end() { return data() + size(); }
  const_iterator begin() const { return data(); }
  const_iterator end() const { return data() + size(); }

private:
  static void construct_elems(void* dst, uint32_t n)
  {
    T* d = static_cast<T*>(dst);
    for (uint32_t i = 0; i != n; ++i) {
      new (d + i) T();
    }
  }
  static void copy_elems(void* dst, const void* src, uint32_t n)
  {
    T*       d = static_cast<T*>(dst);
    const T* s = static_cast<const T*>(src);
    for (uint32_t i = 0; i != n; ++i) {
      new (d + i) T(s[i]);
    }
  }
  static void relocate_elems(void* dst, void* src, uint32_t n)
  {
    T* d = static_cast<T*>(dst);
    T* s = static_cast<T*>(src);
    for (uint32_t i = 0; i != n; ++i) {
      new (d + i) T(std::move(s[i]));
      s[i].~T();
    }
  }
  static void destroy_elems(void* ptr, uint32_t n)
  {
    T* p = static_cast<T*>(ptr);
    for (uint32_t i = 0; i != n; ++i) {
      p[i].~T();
    }
  }

  /// Operations on the elements of this array type.
  static const detail::dyn_array_ops ops;
};

template <class T>
const detail::dyn_array_ops dyn_array<T>::ops = {
    sizeof(T),
    std::is_trivially_default_constructible<T>::value ? nullptr : &dyn_array<T>::construct_elems,
    std::is_trivially_copyable<T>::value ? nullptr : &dyn_array<T>::copy_elems,
    std::is_trivially_copyable<T>::value ? nullptr : &dyn_array<T>::relocate_elems,
    std::is_trivially_destructible<T>::value ? nullptr : &dyn_array<T>::destroy_elems};

template <class T>
bool operator==(const dyn_array<T>& lhs, const dyn_array<T>& rhs)
{
  return lhs.size() == rhs.size() and std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <class T, uint32_t MAX_N>
class bounded_array
{
public:
  using value_type     = T;
  using iterator       = T*;
  using const_iterator = const T*;

  bounded_array() = default;
  explicit bounded_array(uint32_t size_) : current_size(size_) {}
  bounded_array(const bounded_array& other) noexcept : current_size(other.size())
  {
    std::copy(other.data(), other.data() + other.size(), data());
  }
  bounded_array& operator=(const bounded_array& other) noexcept
  {
    if (this != &other) {
      current_size = other.size();
      std::copy(other.data(), other.data() + other.size(), data());
    }
    return *this;
  }
  static uint32_t capacity() { return MAX_N; }
  uint32_t        size() const { return current_size; }
  T&              operator[](uint32_t idx) { return data_[idx]; }
  const T&        operator[](uint32_t idx) const { return data_[idx]; }
  bool            operator==(const bounded_array<T, MAX_N>& other) const
  {
    return std::equal(data(), data() + size(), other.data(), other.data() + other.size());
  }
  void resize(uint32_t new_size) { current_size = new_size; }
  void clear() { resize(0); }
  void push_back(const T& elem)
  {
    if (current_size >= MAX_N) {
      log_error("Maximum size {} achieved for bounded_array.", MAX_N);
      return;
    }
    data_[current_size++] = elem;
  }
  T&             back() { return data_[current_size - 1]; }
  const T&       back() const { return data_[current_size - 1]; }
  T*             data() { return data_.data(); }
  const T*       data() const { return data_.data(); }
  iterator       begin() { return data_.data(); }
  iterator       end() { return data_.data() + size(); }
  const_iterator begin() const { return data_.data(); }
  const_iterator end() const { return data_.data() + size(); }

private:
  std::array<T, MAX_N> data_{};
  uint32_t             current_size{0};
};

/**
 * This array does small buffer optimization. The array has a small stack (Nthres elements) to store elements. Once
 * the number of elements exceeds this stack, the array allocs on the heap.
 * @tparam T
 * @tparam Nthres number of elements T that can be stored in the stack
 */
template <class T, uint32_t Nthres = ceil_frac((size_t)16, sizeof(T))>
class ext_array
{
public:
  static const uint32_t small_buffer_size = Nthres;
  ext_array() : size_(0), head(&small_buffer.data[0]) {}
  explicit ext_array(uint32_t new_size) : ext_array() { resize(new_size); }
  ext_array(const ext_array<T, Nthres>& other) : ext_array(other.size_)
  {
    std::copy(other.head, other.head + other.size_, head);
  }
  ext_array(ext_array<T, Nthres>&& other) noexcept
  {
    size_ = other.size();
    if (other.is_in_small_buffer()) {
      head = &small_buffer.data[0];
      std::copy(other.data(), other.data() + other.size(), head);
    } else {
      head              = other.head;
      small_buffer.cap_ = other.small_buffer.cap_;
      other.head        = &other.small_buffer.data[0];
      other.size_       = 0;
    }
  }
  ~ext_array()
  {
    if (not is_in_small_buffer()) {
      delete[] head;
    }
  }
  ext_array<T, Nthres>& operator=(const ext_array<T, Nthres>& other)
  {
    if (this != &other) {
      resize(other.size());
      std::copy(other.data(), other.data() + other.size(), head);
    }
    return *this;
  }

  uint32_t size() const { return size_; }
  uint32_t capacity() const { return is_in_small_buffer() ? Nthres : small_buffer.cap_; }
  T&       operator[](uint32_t index) { return head[index]; }
  const T& operator[](uint32_t index) const { return head[index]; }
  T*       data() { return &head[0]; }
  const T* data() const { return &head[0]; }
  T&       back() { return head[size() - 1]; }
  const T& back() const { return head[size() - 1]; }
  bool     operator==(const ext_array<T, Nthres>& other) const
  {
    return other.size() == size() and std::equal(other.data(), other.data() + other.size(), data());
  }
  void push_back(const T& elem)
  {
    resize(size() + 1);
    head[size() - 1] = elem;
  }
  void resize(uint32_t new_size)
  {
    if (new_size == size_) {
      return;
    }
    if (capacity() >= new_size) {
      size_ = new_size;
      return;
    }
    T*       old_data = head;
    uint32_t newcap   = new_size + 5;
    head              = new T[newcap];
    std::copy(old_data, old_data + size_, head);
    size_ = new_size;
    if (old_data != &small_buffer.data[0]) {
      delete[] old_data;
    }
    small_buffer.cap_ = newcap;
  }
  bool is_in_small_buffer() const { return head == &small_buffer.data[0]; }

private:
  union {
    T        data[Nthres];
    uint32_t cap_;
  } small_buffer;
  uint32_t size_;
  T*       head;
};

/*********************
      copy_ptr
*********************/

template <class T>
class copy_ptr
{
public:
  copy_ptr() : ptr(nullptr) {}
  explicit copy_ptr(T* ptr_) : ptr(ptr_) {}
  copy_ptr(copy_ptr<T>&& other) noexcept : ptr(other.ptr) { other.ptr = nullptr; }
  copy_ptr(const copy_ptr<T>& other) { ptr = (other.ptr == nullptr) ? nullptr : new T(*other.ptr); }
  ~copy_ptr() { destroy_(); }
  copy_ptr<T>& operator=(const copy_ptr<T>& other)
  {
    if (this != &other) {
      reset((other.ptr == nullptr) ? nullptr : new T(*other.ptr));
    }
    return *this;
  }
  copy_ptr<T>& operator=(copy_ptr<T>&& other) noexcept
  {
    if (this != &other) {
      reset(other.release());
    }
    return *this;
  }
  bool     operator==(const copy_ptr<T>& other) const { return *ptr == *other; }
  T*       operator->() { return ptr; }
  const T* operator->() const { return ptr; }
  T&       operator*() { return *ptr; }       // like pointers, don't call this if ptr==NULL
  const T& operator*() const { return *ptr; } // like pointers, don't call this if ptr==NULL
  T*       get() { return ptr; }
  const T* get() const { return ptr; }
  T*       release()
  {
    T* ret = ptr;
    ptr    = nullptr;
    return ret;
  }
  void reset(T* ptr_ = nullptr)
  {
    destroy_();
    ptr = ptr_;
  }
  void set_present(bool flag = true)
  {
    if (flag) {
      reset(new T());
    } else {
      reset();
    }
  }
  bool is_present() const { return get() != nullptr; }

private:
  void destroy_()
  {
    if (ptr != NULL) {
      delete ptr;
    }
  }
  T* ptr;
};

template <class T>
copy_ptr<typename std::decay<T>::type> make_copy_ptr(T&& t)
{
  using T2 = typename std::decay<T>::type;
  return copy_ptr<T2>(new T2(std::forward<T>(t)));
}

/*********************
     choice utils
*********************/

template <typename Enumerated>
void assert_choice_type(typename Enumerated::options access_type, Enumerated& current_type, const char* choice_type)
{
  if (OCUDU_UNLIKELY(current_type.value != access_type)) {
    log_error("Invalid field access for choice type \"{}\" (\"{}\"!=\"{}\")",
              choice_type,
              Enumerated(access_type).to_string(),
              current_type.to_string());
  }
}

union alignment_t {
  char        c;
  float       f;
  uint32_t    i;
  uint64_t    i2;
  double      d;
  long double d2;
  uint32_t*   ptr;
};

template <std::size_t Size, std::size_t Align>
struct choice_buffer_base_t {
  static const std::size_t data_size  = Size;
  static const std::size_t data_align = Align;
  using buffer_t                      = typename std::aligned_storage<data_size, data_align>::type;
  buffer_t buffer;

  choice_buffer_base_t() : buffer() {}

  template <typename T>
  T& get()
  {
    return *std::launder(reinterpret_cast<T*>(&buffer));
  }
  template <typename T>
  const T& get() const
  {
    return *std::launder(reinterpret_cast<const T*>(&buffer));
  }
};

template <typename... Ts>
struct choice_buffer_t : public choice_buffer_base_t<max_size({sizeof(alignment_t), sizeof(Ts)...}),
                                                     max_size({alignof(alignment_t), alignof(Ts)...})> {};

using pod_choice_buffer_t = choice_buffer_t<>;

namespace table {

struct storage_ops;

/// Storage of a CHOICE with choice_buffer_t storage: the operations of each alternative, null for alternatives without
/// a value.
struct choice_storage_desc {
  const storage_ops* const* alternatives;
  uint32_t                  nof_types;
  /// Name of the CHOICE type, for the log of an invalid type.
  const char* name;
};

/// Destroys the value of the alternative of the given type.
void storage_destroy_choice(void* storage, uint32_t type, const choice_storage_desc& desc);
/// Stores a default value of the alternative of the given type in the empty storage.
void storage_init_choice(void* storage, uint32_t type, const choice_storage_desc& desc);
/// Stores a copy of the value of other, of the given type, in the empty storage.
void storage_copy_choice(void* storage, const void* other, uint32_t type, const choice_storage_desc& desc);

/// Active alternative and its value of a CHOICE of type Owner, whose static member storage_desc describes the
/// alternatives.
template <class Owner, class Types, class Buffer>
struct choice_storage {
  choice_storage() = default;
  choice_storage(const choice_storage& other) : type(other.type)
  {
    storage_copy_choice(&value, &other.value, type, Owner::storage_desc);
  }
  choice_storage& operator=(const choice_storage& other)
  {
    if (this != &other) {
      storage_destroy_choice(&value, type, Owner::storage_desc);
      type = other.type;
      storage_copy_choice(&value, &other.value, type, Owner::storage_desc);
    }
    return *this;
  }
  ~choice_storage() { storage_destroy_choice(&value, type, Owner::storage_desc); }

  void set(typename Types::options e)
  {
    storage_destroy_choice(&value, type, Owner::storage_desc);
    type = e;
    storage_init_choice(&value, e, Owner::storage_desc);
  }
  template <typename T>
  T& get()
  {
    return value.template get<T>();
  }
  template <typename T>
  const T& get() const
  {
    return value.template get<T>();
  }

  Types  type;
  Buffer value;
};

} // namespace table

struct choice_buffer_ptr {
public:
  choice_buffer_ptr() = default;
  template <typename T>
  choice_buffer_ptr(T&& to_store) : obj(std::forward<T>(to_store))
  {
  }

  template <typename T, std::enable_if_t<not std::is_same<std::decay_t<T>, choice_buffer_ptr>::value, int> = 0>
  choice_buffer_ptr& operator=(T&& to_store)
  {
    obj = std::forward<T>(to_store);
    return *this;
  }

  bool has_value() const { return obj.has_value(); }

  template <typename T>
  T& get()
  {
    T* ret = std::any_cast<T>(&obj);
    ocudu_assert(ret != nullptr, "Invalid choice type");
    return *ret;
  }

  template <typename T>
  const T& get() const
  {
    const T* ret = std::any_cast<T>(&obj);
    ocudu_assert(ret != nullptr, "Invalid choice type");
    return *ret;
  }

  void reset() { obj.reset(); }

private:
  std::any obj;
};

} // namespace asn1
