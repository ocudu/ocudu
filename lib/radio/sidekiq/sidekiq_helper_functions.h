// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/adt/complex.h"
#include "ocudu/adt/span.h"

namespace ocudu {

/// \brief Converts packed 12-bit signed integer samples to 16-bit complex integer samples.
/// \remark An assertion is triggered if the input and output sizes are not consistent.
void convert_i12_to_ci16(span<ci16_t> out, span<const uint32_t> in);

/// \brief Converts packed 16-bit signed integer samples to 12-bit complex integer samples.
/// \remark An assertion is triggered if the input and output sizes are not consistent.
void convert_ci16_to_i12(span<uint32_t> out, span<const ci16_t> in);

} // namespace ocudu
