// SPDX-FileCopyrightText: Copyright (C) 2021-2026 DeepSig Inc
// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/cuda/adt/device_vector.h"
#include <cuda_runtime.h>

using namespace ocudu;
using namespace ocudu::cuda;

cuda_expected<void*> ocudu::cuda::detail::device_allocate(std::size_t size)
{
  void*       ptr    = nullptr;
  cuda_result result = check_cuda_error(::cudaMalloc(&ptr, size), "device allocation");
  if (!result.has_value()) {
    return make_unexpected(result.error());
  }

  return ptr;
}

void ocudu::cuda::detail::device_deallocate(void* ptr)
{
  if (ptr == nullptr) {
    return;
  }

  // The result of cudaFree() is ignored on purpose: this runs from a destructor which has no way
  // to report an error, and a failure here means the CUDA context is already unusable.
  ::cudaFree(ptr);
}
