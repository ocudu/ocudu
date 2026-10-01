// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/scheduler/config/scheduler_expert_config.h"
#include "ocudu/scheduler/scheduler_configurator.h"

namespace ocudu {

/// \brief Configuration used to create a scheduler instance.
struct scheduler_config {
  const scheduler_expert_config& expert_params;
  sched_configuration_notifier&  config_notifier;
};

} // namespace ocudu
