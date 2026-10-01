// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "scheduler_policy.h"
#include "ocudu/scheduler/config/scheduler_expert_config.h"
#include <memory>

namespace ocudu {

class cell_configuration;

/// Creatre intra-cell, intra-slice scheduler policy.
std::unique_ptr<scheduler_policy> create_scheduler_strategy(const scheduler_policy_config& policy_cfg,
                                                            const cell_configuration&      cell_cfg);

} // namespace ocudu
