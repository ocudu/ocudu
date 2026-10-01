// SPDX-FileCopyrightText: Copyright (C) 2021-2026 DeepSig Inc
// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/cuda/adt/managed_vector.h"
#include <cuda_runtime.h>

using namespace ocudu;
using namespace ocudu::cuda;

// CUDA 13 replaced the device ordinal taken by cudaMemPrefetchAsync() and cudaMemAdvise() with a
// cudaMemLocation, and added a flags argument to the prefetch. Both spellings are wrapped here so
// that the accelerated blocks never carry the version check themselves.
#if CUDART_VERSION >= 13000

/// Describes a side of the system in the form the CUDA runtime expects.
static ::cudaMemLocation to_cuda_location(memory_location location, int device_id)
{
  if (location == memory_location::host) {
    return ::cudaMemLocation{cudaMemLocationTypeHost, 0};
  }
  return ::cudaMemLocation{cudaMemLocationTypeDevice, device_id};
}

static ::cudaError_t prefetch_async(void* ptr, std::size_t size, memory_location location, int device_id, void* stream)
{
  return ::cudaMemPrefetchAsync(
      ptr, size, to_cuda_location(location, device_id), 0, static_cast<::cudaStream_t>(stream));
}

static ::cudaError_t
advise(void* ptr, std::size_t size, ::cudaMemoryAdvise kind, memory_location location, int device_id)
{
  return ::cudaMemAdvise(ptr, size, kind, to_cuda_location(location, device_id));
}

#else

/// Describes a side of the system in the form the CUDA runtime expects.
static int to_cuda_location(memory_location location, int device_id)
{
  return (location == memory_location::host) ? cudaCpuDeviceId : device_id;
}

static ::cudaError_t prefetch_async(void* ptr, std::size_t size, memory_location location, int device_id, void* stream)
{
  return ::cudaMemPrefetchAsync(ptr, size, to_cuda_location(location, device_id), static_cast<::cudaStream_t>(stream));
}

static ::cudaError_t
advise(void* ptr, std::size_t size, ::cudaMemoryAdvise kind, memory_location location, int device_id)
{
  return ::cudaMemAdvise(ptr, size, kind, to_cuda_location(location, device_id));
}

#endif

cuda_expected<void*> ocudu::cuda::detail::managed_allocate(std::size_t size)
{
  void*       ptr    = nullptr;
  cuda_result result = check_cuda_error(::cudaMallocManaged(&ptr, size), "managed allocation");
  if (!result.has_value()) {
    return make_unexpected(result.error());
  }

  return ptr;
}

void ocudu::cuda::detail::managed_deallocate(void* ptr)
{
  if (ptr == nullptr) {
    return;
  }

  // The result of cudaFree() is ignored on purpose: this runs from a destructor which has no way
  // to report an error, and a failure here means the CUDA context is already unusable.
  ::cudaFree(ptr);
}

cuda_result ocudu::cuda::detail::managed_prefetch(void*              ptr,
                                                  std::size_t        size,
                                                  memory_location    location,
                                                  int                device_id,
                                                  const cuda_stream& stream)
{
  if (!stream.is_valid()) {
    return make_unexpected(std::string("CUDA managed prefetch failed: the stream is not valid"));
  }

  ::cudaError_t status = prefetch_async(ptr, size, location, device_id, stream.native());

  // Prefetching is an optimization and not every device offers it. Where it is missing the pages
  // migrate on first touch instead, which is correct but slower, so the caller is not failed.
  if (status == cudaErrorNotSupported) {
    (void)::cudaGetLastError();
    return {};
  }

  return check_cuda_error(status, "managed prefetch");
}

cuda_result ocudu::cuda::detail::managed_advise_preferred_location(void*           ptr,
                                                                   std::size_t     size,
                                                                   memory_location location,
                                                                   int             device_id)
{
  ::cudaError_t status = advise(ptr, size, cudaMemAdviseSetPreferredLocation, location, device_id);
  if (status == cudaErrorNotSupported) {
    (void)::cudaGetLastError();
    return {};
  }

  return check_cuda_error(status, "managed preferred location advice");
}

cuda_result
ocudu::cuda::detail::managed_advise_accessed_by(void* ptr, std::size_t size, memory_location location, int device_id)
{
  ::cudaError_t status = advise(ptr, size, cudaMemAdviseSetAccessedBy, location, device_id);
  if (status == cudaErrorNotSupported) {
    (void)::cudaGetLastError();
    return {};
  }

  return check_cuda_error(status, "managed accessed-by advice");
}
