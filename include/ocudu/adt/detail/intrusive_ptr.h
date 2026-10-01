// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include <atomic>

namespace ocudu {

/// \brief Atomic reference counter that can be used as a member of objects T of a intrusive_ptr<T>.
class intrusive_ptr_atomic_ref_counter
{
public:
  void inc_ref() { ref_count.fetch_add(1, std::memory_order_relaxed); }

  bool unique() const { return ref_count.load(std::memory_order_relaxed) == 1; }

  /// Whether there are no references to the object. Useful for pools that reuse an object's storage once unreferenced.
  bool is_unreferenced() const { return ref_count.load(std::memory_order_acquire) == 0; }

  bool dec_ref() { return ref_count.fetch_sub(1, std::memory_order_acq_rel) == 1; }

private:
  std::atomic<unsigned> ref_count{0};
};

/// \brief Smart pointer type where a reference counter for the managed object is stored in the object itself.
///
/// The advantage of this class over a shared_ptr<T> is its simplicity, avoiding two different memory regions for the
/// reference counter and the managed object.
template <typename T>
class intrusive_ptr
{
public:
  intrusive_ptr() noexcept = default;

  intrusive_ptr(T* ptr_, bool add_ref = true) noexcept : ptr(ptr_)
  {
    if (ptr != nullptr && add_ref) {
      intrusive_ptr_inc_ref(ptr);
    }
  }

  intrusive_ptr(const intrusive_ptr& other) noexcept : ptr(other.ptr)
  {
    if (ptr != nullptr) {
      intrusive_ptr_inc_ref(ptr);
    }
  }

  intrusive_ptr(intrusive_ptr&& other) noexcept : ptr(other.ptr) { other.ptr = nullptr; }

  ~intrusive_ptr()
  {
    if (ptr != nullptr) {
      intrusive_ptr_dec_ref(ptr);
    }
  }

  intrusive_ptr& operator=(const intrusive_ptr& other) noexcept
  {
    if (this == &other) {
      return *this;
    }
    intrusive_ptr{other}.swap(*this);
    return *this;
  }

  intrusive_ptr& operator=(intrusive_ptr&& other) noexcept
  {
    intrusive_ptr<T>{std::move(other)}.swap(*this);
    return *this;
  }

  T& operator*() const { return *ptr; }

  T* operator->() const { return ptr; }

  T* get() const { return ptr; }

  void reset() { intrusive_ptr<T>{}.swap(*this); }

  void reset(T* other) { intrusive_ptr<T>{other}.swap(*this); }

  void swap(intrusive_ptr& other) noexcept
  {
    T* temp   = ptr;
    ptr       = other.ptr;
    other.ptr = temp;
  }

  bool unique() const { return intrusive_ptr_is_unique(ptr); }

private:
  T* ptr = nullptr;
};

template <typename T>
inline void swap(intrusive_ptr<T>& lhs, intrusive_ptr<T>& rhs)
{
  lhs.swap(rhs);
}

template <typename T, typename U>
inline bool operator==(const intrusive_ptr<T>& lhs, const intrusive_ptr<U>& rhs)
{
  return (lhs.get() == rhs.get());
}

template <typename T, typename U>
inline bool operator==(const intrusive_ptr<T>& lhs, U* rhs)
{
  return (lhs.get() == rhs);
}

template <typename T>
inline bool operator==(const intrusive_ptr<T>& lhs, std::nullptr_t rhs)
{
  return (lhs.get() == rhs);
}

template <typename T, typename U>
inline bool operator!=(const intrusive_ptr<T>& lhs, const intrusive_ptr<U>& rhs)
{
  return (lhs.get() != rhs.get());
}

template <typename T, typename U>
inline bool operator!=(const intrusive_ptr<T>& lhs, U* rhs)
{
  return (lhs.get() != rhs);
}

template <typename T>
inline bool operator!=(const intrusive_ptr<T>& lhs, std::nullptr_t rhs)
{
  return (lhs.get() != rhs);
}

} // namespace ocudu
