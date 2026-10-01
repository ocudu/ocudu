// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

namespace ocudu {

struct expert_execution_appconfig;

/// Validates the given expert configuration of the application. Returns true on success, false otherwise.
bool validate_expert_execution_appconfig(const expert_execution_appconfig& config);

} // namespace ocudu
