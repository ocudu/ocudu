// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

namespace ocudu {

struct du_low_appconfig;

/// Validates the given DU low application configuration. Returns true on success, false otherwise.
bool validate_du_low_appconfig(const du_low_appconfig& config);

} // namespace ocudu
