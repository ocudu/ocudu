// SPDX-FileCopyrightText: Copyright (C) 2021-2026 DeepSig Inc
// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/adt/span.h"
#include "ocudu/cuda/adt/cuda_error.h"
#include "ocudu/cuda/adt/cuda_stream.h"
#include <cstddef>
#include <utility>

namespace ocudu {
namespace cuda {

/// Side of the system that managed memory pages are placed on.
enum class memory_location {
  /// The pages are placed in host memory.
  host,
  /// The pages are placed in the memory of the CUDA device.
  device
};

namespace detail {

/// Allocates \c size bytes of managed memory returning the pointer or a description of the error.
cuda_expected<void*> managed_allocate(std::size_t size);

/// Releases managed memory obtained from \ref managed_allocate().
void managed_deallocate(void* ptr);

/// Moves the pages of a managed allocation to \c location queued in a stream.
cuda_result
managed_prefetch(void* ptr, std::size_t size, memory_location location, int device_id, const cuda_stream& stream);

/// Tells the driver which side accesses a managed allocation most so that it places the pages there.
cuda_result managed_advise_preferred_location(void* ptr, std::size_t size, memory_location location, int device_id);

/// Tells the driver that a side accesses a managed allocation so that it maps the pages there.
cuda_result managed_advise_accessed_by(void* ptr, std::size_t size, memory_location location, int device_id);

} // namespace detail

/// \brief Moves the pages behind a view of managed memory to one side of the system, queued in a
/// stream.
///
/// \tparam T Element type of the view.
/// \param[in] data      View of managed memory, such as one obtained from
///                      \ref ocudu::prach_buffer::get_buffer().
/// \param[in] location  Side the pages are moved to.
/// \param[in] device_id Identifier of the CUDA device.
/// \param[in] stream    Stream the move is queued on.
/// \return A successful result, or a description of the failure.
///
/// \remark For a consumer, which reads a view handed to it rather than the allocation behind it. An
/// owner calls \ref managed_vector::prefetch() on its own storage instead.
template <typename T>
cuda_result prefetch(span<const T> data, memory_location location, int device_id, const cuda_stream& stream)
{
  if (data.empty()) {
    return {};
  }
  return detail::managed_prefetch(const_cast<T*>(data.data()), data.size() * sizeof(T), location, device_id, stream);
}

/// \brief Owning (RAII) block of memory addressable by both the host and the device.
///
/// The driver migrates the pages between host and device on demand so the same address is valid on
/// either side. That migration is what makes the block convenient and also what makes it slow when
/// it happens at the wrong moment, which is why \ref prefetch() exists: it moves the pages while the
/// consumer is not waiting on them.
///
/// The block is move-only so a managed allocation always has exactly one owner. It is not
/// resizable, matching \ref device_vector.
template <typename T>
class managed_vector
{
public:
  /// \brief Allocates a block holding \c size elements.
  ///
  /// \param[in] size Number of elements. A size of zero produces a valid empty block.
  /// \return The block or a description of the error.
  static cuda_expected<managed_vector<T>> create(std::size_t size)
  {
    if (size == 0) {
      return managed_vector<T>();
    }

    cuda_expected<void*> ptr = detail::managed_allocate(size * sizeof(T));
    if (!ptr.has_value()) {
      return make_unexpected(ptr.error());
    }

    return managed_vector<T>(static_cast<T*>(ptr.value()), size);
  }

  managed_vector() = default;

  ~managed_vector() { detail::managed_deallocate(ptr); }

  // Prevent copyability, make the class move-only.
  managed_vector(const managed_vector&)            = delete;
  managed_vector& operator=(const managed_vector&) = delete;

  managed_vector(managed_vector&& other) noexcept :
    ptr(std::exchange(other.ptr, nullptr)), nof_elements(std::exchange(other.nof_elements, 0))
  {
  }

  managed_vector& operator=(managed_vector&& other) noexcept
  {
    if (this != &other) {
      detail::managed_deallocate(ptr);
      ptr          = std::exchange(other.ptr, nullptr);
      nof_elements = std::exchange(other.nof_elements, 0);
    }
    return *this;
  }

  /// \brief Returns the address of the block, valid on the host and on the device.
  ///
  /// \remark Reading it from one side while the other is still writing is a data race that the
  /// driver does not detect. Order the two with \ref cuda_event.
  T* data() { return ptr; }

  /// See the non-const overload.
  const T* data() const { return ptr; }

  /// Returns the number of elements in the block.
  std::size_t size() const { return nof_elements; }

  /// Returns the size of the block in bytes.
  std::size_t size_bytes() const { return nof_elements * sizeof(T); }

  /// Returns true if the block holds no elements.
  bool empty() const { return nof_elements == 0; }

  /// \brief Moves the pages to \c location, queued in a stream.
  ///
  /// \param[in] location  Side the pages are moved to.
  /// \param[in] device_id Device that the pages are moved to, ignored when moving them to the host.
  /// \param[in] stream    Stream the migration is queued in.
  /// \return A successful result, or a description of the error.
  ///
  /// \remark Prefetching is an optimization: the pages migrate on first touch anyway. A device that
  /// does not support it reports success and leaves the pages where they are.
  cuda_result prefetch(memory_location location, int device_id, const cuda_stream& stream) const
  {
    if (ptr == nullptr) {
      return {};
    }
    return detail::managed_prefetch(ptr, size_bytes(), location, device_id, stream);
  }

  /// \brief Tells the driver which side accesses the block most.
  ///
  /// \param[in] location  Side that accesses the block most.
  /// \param[in] device_id Device that accesses the block most, ignored for the host.
  /// \return A successful result or a description of the error.
  cuda_result advise_preferred_location(memory_location location, int device_id) const
  {
    if (ptr == nullptr) {
      return {};
    }
    return detail::managed_advise_preferred_location(ptr, size_bytes(), location, device_id);
  }

  /// \brief Tells the driver that a side accesses the block so that it maps the pages there.
  ///
  /// \param[in] location  Side that accesses the block.
  /// \param[in] device_id Device that accesses the block. Ignored for the host.
  /// \return A successful result or a description of the error.
  cuda_result advise_accessed_by(memory_location location, int device_id) const
  {
    if (ptr == nullptr) {
      return {};
    }
    return detail::managed_advise_accessed_by(ptr, size_bytes(), location, device_id);
  }

private:
  managed_vector(T* ptr_, std::size_t nof_elements_) : ptr(ptr_), nof_elements(nof_elements_) {}

  T*          ptr          = nullptr;
  std::size_t nof_elements = 0;
};

} // namespace cuda
} // namespace ocudu
