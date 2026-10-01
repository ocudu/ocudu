// SPDX-FileCopyrightText: Copyright (C) 2021-2026 DeepSig Inc
// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/cuda/adt/cuda_copy.h"
#include <cuda_runtime.h>

using namespace ocudu;
using namespace ocudu::cuda;

cuda_result ocudu::cuda::detail::copy_to_device(void* dst, const void* src, std::size_t size)
{
  return check_cuda_error(::cudaMemcpy(dst, src, size, cudaMemcpyHostToDevice), "host to device copy");
}

cuda_result ocudu::cuda::detail::copy_to_host(void* dst, const void* src, std::size_t size)
{
  return check_cuda_error(::cudaMemcpy(dst, src, size, cudaMemcpyDeviceToHost), "device to host copy");
}

cuda_result
ocudu::cuda::detail::copy_to_device_async(void* dst, const void* src, std::size_t size, const cuda_stream& stream)
{
  if (!stream.is_valid()) {
    return make_unexpected(std::string("CUDA host to device copy failed: the stream is not valid"));
  }

  return check_cuda_error(
      ::cudaMemcpyAsync(dst, src, size, cudaMemcpyHostToDevice, static_cast<::cudaStream_t>(stream.native())),
      "asynchronous host to device copy");
}

cuda_result
ocudu::cuda::detail::copy_to_host_async(void* dst, const void* src, std::size_t size, const cuda_stream& stream)
{
  if (!stream.is_valid()) {
    return make_unexpected(std::string("CUDA device to host copy failed: the stream is not valid"));
  }

  return check_cuda_error(
      ::cudaMemcpyAsync(dst, src, size, cudaMemcpyDeviceToHost, static_cast<::cudaStream_t>(stream.native())),
      "asynchronous device to host copy");
}

cuda_result ocudu::cuda::detail::device_memset_async(void* dst, int value, std::size_t size, const cuda_stream& stream)
{
  if (!stream.is_valid()) {
    return make_unexpected(std::string("CUDA device fill failed: the stream is not valid"));
  }

  return check_cuda_error(::cudaMemsetAsync(dst, value, size, static_cast<::cudaStream_t>(stream.native())),
                          "asynchronous device fill");
}
