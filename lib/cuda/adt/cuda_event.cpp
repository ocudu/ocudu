// SPDX-FileCopyrightText: Copyright (C) 2021-2026 DeepSig Inc
// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/cuda/adt/cuda_event.h"
#include "cuda_wait.h"
#include "ocudu/cuda/adt/cuda_stream.h"
#include <cuda_runtime.h>
#include <utility>

using namespace ocudu;
using namespace cuda;

cuda_expected<cuda_event> cuda_event::create()
{
  ::cudaEvent_t event = nullptr;

  // Timing collection is not needed and makes recording more expensive.
  cuda_result result = check_cuda_error(::cudaEventCreateWithFlags(&event, cudaEventDisableTiming), "event creation");

  if (!result.has_value()) {
    return make_unexpected(result.error());
  }

  return cuda_event(event);
}

cuda_event::~cuda_event()
{
  if (handle != nullptr) {
    ::cudaEventDestroy(static_cast<::cudaEvent_t>(handle));
  }
}

cuda_event::cuda_event(cuda_event&& other) noexcept : handle(std::exchange(other.handle, nullptr)) {}

cuda_event& cuda_event::operator=(cuda_event&& other) noexcept
{
  if (this != &other) {
    if (handle != nullptr) {
      ::cudaEventDestroy(static_cast<::cudaEvent_t>(handle));
    }
    handle = std::exchange(other.handle, nullptr);
  }
  return *this;
}

cuda_result cuda_event::record(const cuda_stream& stream)
{
  if (handle == nullptr) {
    return make_unexpected(std::string("CUDA event record failed: the event is not valid"));
  }
  if (!stream.is_valid()) {
    return make_unexpected(std::string("CUDA event record failed: the stream is not valid"));
  }

  return check_cuda_error(
      ::cudaEventRecord(static_cast<::cudaEvent_t>(handle), static_cast<::cudaStream_t>(stream.native())),
      "event record");
}

cuda_result cuda_event::synchronize() const
{
  if (handle == nullptr) {
    return make_unexpected(std::string("CUDA event synchronization failed: the event is not valid"));
  }

  // "poll-and-yield" wait replacing the blocking cudaEventSynchronize().
  ::cudaEvent_t event = static_cast<::cudaEvent_t>(handle);
  return check_cuda_error(ocudu::cuda::detail::wait_yielding([event]() { return ::cudaEventQuery(event); }),
                          "event synchronization");
}

bool cuda_event::is_completed() const
{
  if (handle == nullptr) {
    return true;
  }

  return ::cudaEventQuery(static_cast<::cudaEvent_t>(handle)) == cudaSuccess;
}
