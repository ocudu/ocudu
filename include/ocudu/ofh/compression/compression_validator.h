// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/adt/expected.h"
#include <string>

namespace ocudu {
namespace ofh {

struct ru_compression_params;

/// \brief Validates that the given compression parameters are supported.
///
/// The bit width is only checked for BFP.
///
/// \param[in] params  Compression parameters to validate.
/// \return Nothing when the parameters are supported, otherwise the reason they are not.
error_type<std::string> validate_compression_params(const ru_compression_params& params);

} // namespace ofh
} // namespace ocudu
