// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/cuda/adt/cuda_error.h"
#include "fmt/format.h"
#include <cuda_runtime.h>
#include <string_view>

using namespace ocudu;
using namespace cuda;

/// Builds the description of a failed CUDA operation.
static std::string make_description(::cudaError_t status, std::string_view operation)
{
  return fmt::format("CUDA {} failed: {} ({})", operation, ::cudaGetErrorString(status), static_cast<int>(status));
}

cuda_result ocudu::cuda::check_last_cuda_error(std::string_view operation)
{
  return check_cuda_error(static_cast<int>(::cudaGetLastError()), operation);
}

cuda_result ocudu::cuda::check_cuda_error(int status, std::string_view operation)
{
  ::cudaError_t error = static_cast<::cudaError_t>(status);
  if (error == cudaSuccess) {
    return {};
  }

  return make_unexpected(make_description(error, operation));
}

bool ocudu::cuda::is_cuda_context_lost(int status)
{
  switch (static_cast<::cudaError_t>(status)) {
    case cudaErrorIllegalAddress:
    case cudaErrorLaunchFailure:
    case cudaErrorLaunchTimeout:
    case cudaErrorHardwareStackError:
    case cudaErrorIllegalInstruction:
    case cudaErrorMisalignedAddress:
    case cudaErrorInvalidAddressSpace:
    case cudaErrorInvalidPc:
    case cudaErrorECCUncorrectable:
      return true;
    default:
      return false;
  }
}
