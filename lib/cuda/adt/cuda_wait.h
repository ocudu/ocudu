// SPDX-FileCopyrightText: Copyright (C) 2021-2026 DeepSig Inc
// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include <cuda_runtime.h>
#include <thread>

namespace ocudu {
namespace cuda {
namespace detail {

/// \brief Waits for a completion query to stop reporting work in progress yielding between polls.
///
/// \tparam QueryFunction Callable returning a \c cudaError_t, typically wrapping \c cudaEventQuery()
///                       or \c cudaStreamQuery().
/// \param[in] query Completion query polled until it reports something other than
///                  \c cudaErrorNotReady.
/// \return The status that ended the wait which is \c cudaSuccess when the work completed and the
///         reported error otherwise.
///
/// \remark The CUDA runtime spins inside a blocking wait rather than sleeping. On a thread running
/// at real-time priority that can invert priorities, as the driver threads that complete the work
/// are then unable to run, and the wait never ends. Yielding between polls keeps the calling thread
/// preemptible.
///
/// \remark The wait is not bounded: work that never completes is polled forever because
/// a blocking wait would also never return.
template <typename QueryFunction>
::cudaError_t wait_yielding(QueryFunction&& query)
{
  for (;;) {
    ::cudaError_t status = query();
    if (status != cudaErrorNotReady) {
      return status;
    }
    std::this_thread::yield();
  }
}

} // namespace detail
} // namespace cuda
} // namespace ocudu
