// SPDX-FileCopyrightText: Copyright (C) 2021-2026 DeepSig Inc
// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/cuda/adt/cuda_error.h"

namespace ocudu {
namespace cuda {

class cuda_stream;

/// \brief Owning (RAII) handle of a CUDA event.
///
/// Events mark a point in a stream so that the host can tell when the work queued up to that point
/// has completed. Their most common use is to bound the lifetime of a host buffer handed to an
/// asynchronous copy: the buffer may only be reused once the event recorded after the copy has
/// completed, otherwise the copy reads memory the host has already overwritten.
class cuda_event
{
public:
  /// \brief Creates an event.
  ///
  /// \return The event or a description of the error.
  ///
  /// \remark The event does not collect timing information which makes recording it cheaper.
  /// It is meant for ordering, not for measurement.
  static cuda_expected<cuda_event> create();

  cuda_event() = default;
  ~cuda_event();

  cuda_event(const cuda_event&)            = delete;
  cuda_event& operator=(const cuda_event&) = delete;

  cuda_event(cuda_event&& other) noexcept;
  cuda_event& operator=(cuda_event&& other) noexcept;

  /// Returns true if the object owns an event.
  bool is_valid() const { return handle != nullptr; }

  /// \brief Returns the underlying \c cudaEvent_t.
  ///
  /// \remark The returned handle is owned by this object and must not be destroyed by the caller.
  void* native() const { return handle; }

  /// \brief Records the event in a stream.
  ///
  /// \param[in] stream Stream in which the event is recorded.
  /// \return A successful result, or a description of the error.
  cuda_result record(const cuda_stream& stream);

  /// \brief Waits until the recorded work has completed.
  ///
  /// \return A successful result, or a description of the error.
  ///
  /// \remark This does not block inside the CUDA runtime. It checks whether the work has finished
  /// and hands the CPU to other threads in between checks, so a caller running at real-time
  /// priority cannot starve the driver threads that would report completion.
  cuda_result synchronize() const;

  /// \brief Returns true if the recorded work has completed, without waiting.
  bool is_completed() const;

private:
  explicit cuda_event(void* handle_) : handle(handle_) {}

  void* handle = nullptr;
};

} // namespace cuda
} // namespace ocudu
