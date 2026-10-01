// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

/// \file
/// \brief Phase angle unwrapping declarations.

#pragma once

#include "ocudu/adt/complex.h"
#include "ocudu/adt/span.h"

namespace ocudu {
namespace ocuduvec {

/// \brief Computes the unwrapped arguments (phase angles) of a list of complex numbers.
///
/// \param[out] arguments     Output list of unwrapped arguments.
/// \param[in]  cplx_numbers  Input list of complex numbers.
///
/// \warning The input and output lists should have the same length.
/// \warning The memory allocated for input and output should be disjoint.
void unwrap_arguments(span<float> arguments, span<const cf_t> cplx_numbers);

} // namespace ocuduvec
} // namespace ocudu
