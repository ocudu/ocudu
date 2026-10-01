// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

namespace ocudu {

struct du_appconfig;
struct worker_manager_config;

/// Fills the DU worker manager parameters of the given worker manager configuration.
void fill_du_worker_manager_config(worker_manager_config& config, const du_appconfig& unit_cfg);

} // namespace ocudu
