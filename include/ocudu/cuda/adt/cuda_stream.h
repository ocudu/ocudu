// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/cuda/adt/cuda_error.h"

namespace ocudu {
namespace cuda {

/// \brief Priority of the work submitted to a CUDA stream.
enum class cuda_stream_priority {
  /// Default priority.
  normal,
  /// Highest priority supported by the device (for work with a hard deadline).
  high
};

/// \brief Owning (RAII) handle of a CUDA stream.
///
/// The stream is created on construction through \ref create() and destroyed with the object. It is
/// move-only: a stream must have exactly one owner, as destroying it while work is queued is
/// undefined.
class cuda_stream
{
public:
  /// \brief Creates a stream.
  ///
  /// \param[in] priority Priority of the work submitted to the stream.
  /// \return The stream, or a description of the error.
  ///
  /// \remark The stream does not synchronize with the default stream, so
  /// unrelated work does not serialize against it.
  static cuda_expected<cuda_stream> create(cuda_stream_priority priority = cuda_stream_priority::normal);

  cuda_stream() = default;
  ~cuda_stream();

  cuda_stream(const cuda_stream&)            = delete;
  cuda_stream& operator=(const cuda_stream&) = delete;

  cuda_stream(cuda_stream&& other) noexcept;
  cuda_stream& operator=(cuda_stream&& other) noexcept;

  /// Returns true if the object owns a stream.
  bool is_valid() const { return handle != nullptr; }

  /// \brief Returns the underlying \c cudaStream_t.
  ///
  /// \remark The returned handle is owned by this object and must not be destroyed by the caller.
  void* native() const { return handle; }

  /// \brief Waits until all the work queued in the stream has completed.
  ///
  /// \return A successful result, or a description of the error.
  ///
  /// \remark This does not block inside the CUDA runtime. It checks whether the work has finished
  /// and hands the CPU to other threads in between checks, so a caller running at real-time
  /// priority cannot starve the driver threads that would report completion.
  cuda_result synchronize() const;

  /// \brief Returns true if all the work queued in the stream has completed, without waiting.
  bool is_completed() const;

private:
  explicit cuda_stream(void* handle_) : handle(handle_) {}

  void* handle = nullptr;
};

} // namespace cuda
} // namespace ocudu
