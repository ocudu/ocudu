// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/scheduler/scheduler_factory.h"
#include "scheduler_impl.h"

using namespace ocudu;

std::unique_ptr<mac_scheduler> ocudu::create_scheduler(const scheduler_config& sched_cfg)
{
  return std::make_unique<scheduler_impl>(sched_cfg);
}
