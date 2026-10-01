// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "du_appconfig.h"

namespace ocudu {

/// Validates the given DU application configuration. Returns true on success, false otherwise.
bool validate_appconfig(const du_appconfig& config);

} // namespace ocudu
