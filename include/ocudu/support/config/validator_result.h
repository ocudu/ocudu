// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/adt/expected.h"
#include <string>

namespace ocudu {

/// This type represents the result of a validator function. In case of failure, it contains a string describing the
/// error. In case of success, the .has_value() method returns true.
using validator_result = error_type<std::string>;

} // namespace ocudu
