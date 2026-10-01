// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "split6_o_du_unit_config.h"

namespace ocudu {

/// Validates the given O-RAN DU Split 6 unit configuration. Returns true on success, false otherwise.
bool validate_split6_o_du_unit_config(const split6_o_du_unit_config& config);

} // namespace ocudu
