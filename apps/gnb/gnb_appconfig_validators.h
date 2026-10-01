// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "gnb_appconfig.h"

namespace ocudu {

struct cu_cp_unit_config;
struct du_high_unit_config;

/// Validates the given GNB application configuration. Returns true on success, false otherwise.
bool validate_appconfig(const gnb_appconfig& config);

/// Validates that the DU PLMNs and TACs are present in the CU-CP config.
bool validate_plmn_and_tacs(const du_high_unit_config& du_hi_cfg, const cu_cp_unit_config& cu_cp_cfg);

} // namespace ocudu
