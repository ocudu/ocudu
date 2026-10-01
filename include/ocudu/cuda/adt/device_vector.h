// SPDX-FileCopyrightText: Copyright (C) 2021-2026 DeepSig Inc
// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/cuda/adt/cuda_error.h"
#include <cstddef>
#include <utility>

namespace ocudu {
namespace cuda {

namespace detail {

/// Allocates \c size bytes of device memory returning the pointer or a description of the error.
cuda_expected<void*> device_allocate(std::size_t size);

/// Releases device memory obtained from \ref device_allocate().
void device_deallocate(void* ptr);

} // namespace detail

/// \brief Owning (RAII) contiguous block of device memory.
///
/// The memory is only addressable by the device: the host cannot dereference \ref data(). Transfers
/// between host and device memory are performed with the helpers in \c cuda_copy.h.
///
/// The block is move-only, so a device allocation always has exactly one owner. It is not
/// resizable: the accelerated paths allocate for the largest configuration they will process and
/// keep the block for their lifetime rather than reallocating per transmission.
template <typename T>
class device_vector
{
public:
  /// \brief Allocates a block holding \c size elements.
  ///
  /// \param[in] size Number of elements. A size of zero produces a valid empty block.
  /// \return The block or a description of the error.
  static cuda_expected<device_vector<T>> create(std::size_t size)
  {
    if (size == 0) {
      return device_vector<T>();
    }

    cuda_expected<void*> ptr = detail::device_allocate(size * sizeof(T));
    if (!ptr.has_value()) {
      return make_unexpected(ptr.error());
    }

    return device_vector<T>(static_cast<T*>(ptr.value()), size);
  }

  device_vector() = default;

  ~device_vector() { detail::device_deallocate(ptr); }

  device_vector(const device_vector&)            = delete;
  device_vector& operator=(const device_vector&) = delete;

  device_vector(device_vector&& other) noexcept :
    ptr(std::exchange(other.ptr, nullptr)), nof_elements(std::exchange(other.nof_elements, 0))
  {
  }

  device_vector& operator=(device_vector&& other) noexcept
  {
    if (this != &other) {
      detail::device_deallocate(ptr);
      ptr          = std::exchange(other.ptr, nullptr);
      nof_elements = std::exchange(other.nof_elements, 0);
    }
    return *this;
  }

  /// \brief Returns the device address of the block.
  ///
  /// \remark The address is only valid on the device. Dereferencing it on the host is undefined.
  T* data() { return ptr; }

  /// See the non-const overload.
  const T* data() const { return ptr; }

  /// Returns the number of elements in the block.
  std::size_t size() const { return nof_elements; }

  /// Returns the size of the block in bytes.
  std::size_t size_bytes() const { return nof_elements * sizeof(T); }

  /// Returns true if the block holds no elements.
  bool empty() const { return nof_elements == 0; }

private:
  device_vector(T* ptr_, std::size_t nof_elements_) : ptr(ptr_), nof_elements(nof_elements_) {}

  T*          ptr          = nullptr;
  std::size_t nof_elements = 0;
};

} // namespace cuda
} // namespace ocudu
