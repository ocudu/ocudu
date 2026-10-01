// SPDX-FileCopyrightText: Copyright (C) 2021-2026 DeepSig Inc
// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/adt/span.h"
#include "ocudu/cuda/adt/cuda_error.h"
#include "ocudu/cuda/adt/cuda_stream.h"
#include "ocudu/cuda/adt/device_vector.h"

namespace ocudu {
namespace cuda {

namespace detail {

/// Copies \c size bytes between host and device memory blocking until the copy completes.
cuda_result copy_to_device(void* dst, const void* src, std::size_t size);

/// See \ref copy_to_device().
cuda_result copy_to_host(void* dst, const void* src, std::size_t size);

/// Queues a copy of \c size bytes from host to device memory in a stream.
cuda_result copy_to_device_async(void* dst, const void* src, std::size_t size, const cuda_stream& stream);

/// See \ref copy_to_device_async().
cuda_result copy_to_host_async(void* dst, const void* src, std::size_t size, const cuda_stream& stream);

/// See \ref device_zero_async().
cuda_result device_memset_async(void* dst, int value, std::size_t size, const cuda_stream& stream);

} // namespace detail

/// \brief Copies host data into a device block blocking until the copy completes.
///
/// \param[out] dst Destination device block.
/// \param[in]  src Source host data. It must not be larger than the destination.
/// \return A successful result or a description of the error.
template <typename T>
cuda_result copy_to_device(device_vector<T>& dst, span<const T> src)
{
  if (src.size() > dst.size()) {
    return make_unexpected(std::string("Source does not fit in the destination device block"));
  }
  if (src.empty()) {
    return {};
  }

  return detail::copy_to_device(dst.data(), src.data(), src.size() * sizeof(T));
}

/// \brief Copies a device block into host memory, blocking until the copy completes.
///
/// \param[out] dst Destination host data. Note: it must not be smaller than the source.
/// \param[in]  src Source device block.
/// \return A successful result, or a description of the error.
template <typename T>
cuda_result copy_to_host(span<T> dst, const device_vector<T>& src)
{
  if (src.size() > dst.size()) {
    return make_unexpected(std::string("Source device block does not fit in the destination"));
  }
  if (src.empty()) {
    return {};
  }

  return detail::copy_to_host(dst.data(), src.data(), src.size() * sizeof(T));
}

/// \brief Queues a copy of host data into a device block.
///
/// \param[out] dst    Destination device block.
/// \param[in]  src    Source host data. It must not be larger than the destination.
/// \param[in]  stream Stream in which the copy is queued.
/// \return A successful result, or a description of the error.
///
/// \remark The call returns before the copy has read the source. The source must stay unmodified
/// until the copy has completed, which is what \ref cuda_event is for. Reusing it earlier makes the
/// device observe data the host has since overwritten.
template <typename T>
cuda_result copy_to_device_async(device_vector<T>& dst, span<const T> src, const cuda_stream& stream)
{
  if (src.size() > dst.size()) {
    return make_unexpected(std::string("Source does not fit in the destination device block"));
  }
  if (src.empty()) {
    return {};
  }

  return detail::copy_to_device_async(dst.data(), src.data(), src.size() * sizeof(T), stream);
}

/// \brief Queues a copy of a device block into host memory.
///
/// \param[out] dst    Destination host data. It must not be smaller than the source.
/// \param[in]  src    Source device block.
/// \param[in]  stream Stream in which the copy is queued.
/// \return A successful result or a description of the error.
///
/// \remark The call returns before the copy has written the destination which must not be read
/// until the copy has completed.
template <typename T>
cuda_result copy_to_host_async(span<T> dst, const device_vector<T>& src, const cuda_stream& stream)
{
  if (src.size() > dst.size()) {
    return make_unexpected(std::string("Source device block does not fit in the destination"));
  }
  if (src.empty()) {
    return {};
  }

  return detail::copy_to_host_async(dst.data(), src.data(), src.size() * sizeof(T), stream);
}

/// \brief Queues a fill of a device block with zeros
///
/// \param[out] data   Device block to fill
/// \param[in]  stream Stream in which the fill is queued
/// \return A successful result, or a description of the error
///
/// \remark The call returns before the fill has completed, so the block must not be read until it
/// has, which is what \ref cuda_event is for
template <typename T>
cuda_result device_zero_async(device_vector<T>& data, const cuda_stream& stream)
{
  if (data.empty()) {
    return {};
  }

  return detail::device_memset_async(data.data(), 0, data.size_bytes(), stream);
}

} // namespace cuda
} // namespace ocudu
