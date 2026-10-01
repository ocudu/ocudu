// SPDX-FileCopyrightText: Copyright (C) 2021-2026 DeepSig Inc
// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/cuda/adt/cuda_stream.h"
#include "cuda_wait.h"
#include <cuda_runtime.h>
#include <utility>

using namespace ocudu;
using namespace cuda;

cuda_expected<cuda_stream> cuda_stream::create(cuda_stream_priority priority)
{
  ::cudaStream_t stream = nullptr;

  if (priority == cuda_stream_priority::high) {
    int least_priority    = 0;
    int greatest_priority = 0;
    if (::cudaDeviceGetStreamPriorityRange(&least_priority, &greatest_priority) == cudaSuccess) {
      ::cudaError_t status = ::cudaStreamCreateWithPriority(&stream, cudaStreamNonBlocking, greatest_priority);
      if (status == cudaSuccess) {
        return cuda_stream(stream);
      }
      // Fall through to a default priority stream: the priority is an optimization, not a
      // requirement, and a device that does not support it must not fail here.
      (void)::cudaGetLastError();
    }
  }

  cuda_result result = check_cuda_error(::cudaStreamCreateWithFlags(&stream, cudaStreamNonBlocking), "stream creation");
  if (!result.has_value()) {
    return make_unexpected(result.error());
  }

  return cuda_stream(stream);
}

cuda_stream::~cuda_stream()
{
  if (handle != nullptr) {
    ::cudaStreamDestroy(static_cast<::cudaStream_t>(handle));
  }
}

cuda_stream::cuda_stream(cuda_stream&& other) noexcept : handle(std::exchange(other.handle, nullptr)) {}

cuda_stream& cuda_stream::operator=(cuda_stream&& other) noexcept
{
  if (this != &other) {
    if (handle != nullptr) {
      ::cudaStreamDestroy(static_cast<::cudaStream_t>(handle));
    }
    handle = std::exchange(other.handle, nullptr);
  }
  return *this;
}

cuda_result cuda_stream::synchronize() const
{
  if (handle == nullptr) {
    return make_unexpected(std::string("CUDA stream synchronization failed: the stream is not valid"));
  }

  ::cudaStream_t stream = static_cast<::cudaStream_t>(handle);
  return check_cuda_error(ocudu::cuda::detail::wait_yielding([stream]() { return ::cudaStreamQuery(stream); }),
                          "stream synchronization");
}

bool cuda_stream::is_completed() const
{
  if (handle == nullptr) {
    return true;
  }

  return ::cudaStreamQuery(static_cast<::cudaStream_t>(handle)) == cudaSuccess;
}
