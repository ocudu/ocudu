// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "cu_cp_unit_config.h"
#include "ocudu/adt/expected.h"

namespace ocudu {

/// Validates the given CU-CP unit configuration. Returns true on success, false otherwise.
bool validate_cu_cp_unit_config(const cu_cp_unit_config& config);

/// Validates a single report configuration. Returns the reason when it is invalid.
error_type<std::string> validate_report_config(const cu_cp_unit_report_config& config);

} // namespace ocudu
