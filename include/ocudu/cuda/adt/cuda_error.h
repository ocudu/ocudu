// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/adt/expected.h"
#include <string>
#include <string_view>

namespace ocudu {
namespace cuda {

/// \brief Result of a CUDA runtime operation.
///
/// A CUDA error is reported rather than thrown: the accelerated paths run on real-time
/// threads where an exception is not an acceptable control-flow mechanism.
using cuda_result = error_type<std::string>;

/// \brief Result of a CUDA runtime operation producing a value.
template <typename T>
using cuda_expected = expected<T, std::string>;

/// \brief Converts the last CUDA runtime error into a result.
///
/// \param[in] operation Name of the operation, used to build the error description.
/// \return A successful result if no error is pending, otherwise a description of the error.
///
/// \remark This clears the pending error state. An asynchronous error raised by an earlier
/// operation is reported here so the operation named in the description is where the error was
/// detected, not necessarily where it was raised.
cuda_result check_last_cuda_error(std::string_view operation);

/// \brief Converts a CUDA runtime status into a result.
///
/// \param[in] status    Value returned by a CUDA runtime call, of type \c cudaError_t.
/// \param[in] operation Name of the operation, used to build the error description.
/// \return A successful result if the status indicates success, otherwise a description of it.
cuda_result check_cuda_error(int status, std::string_view operation);

/// \brief Returns true if the CUDA context has been left unusable by a previous error.
///
/// Errors such as an illegal memory access are sticky: once raised, every subsequent CUDA call in
/// the process fails with the same error until the context is destroyed. Recovery of an individual
/// operation is not possible in that state.
bool is_cuda_context_lost(int status);

} // namespace cuda
} // namespace ocudu
