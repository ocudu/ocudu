// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/du_types.h"
#include "ocudu/scheduler/scheduler_configurator.h"

namespace ocudu::config_helpers {

/// Build a default UE cell configuration from a given RAN cell configuration.
ue_cell_config make_default_ue_cell_config(const ran_cell_config& cell_cfg,
                                           du_cell_index_t        cell_idx = to_du_cell_index(0));

} // namespace ocudu::config_helpers
