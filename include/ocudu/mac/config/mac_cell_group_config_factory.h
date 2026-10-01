// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "mac_cell_group_params.h"
#include "ocudu/mac/mac_cell_group_config.h"

namespace ocudu {
namespace config_helpers {

mac_cell_group_config make_initial_mac_cell_group_config(const mac_cell_group_params& mcg_params = {});

} // namespace config_helpers
} // namespace ocudu
