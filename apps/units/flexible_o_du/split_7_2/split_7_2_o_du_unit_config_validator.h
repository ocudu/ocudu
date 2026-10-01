// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "split_7_2_o_du_unit_config.h"

namespace ocudu {

/// Validates the given split 7.2 O-RAN DU unit configuration. Returns true on success, false otherwise.
bool validate_split_7_2_o_du_unit_config(const split_7_2_o_du_unit_config& config);

} // namespace ocudu
