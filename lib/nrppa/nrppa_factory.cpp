// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/nrppa/nrppa_factory.h"
#include "nrppa_impl.h"
#include "ocudu/adt/format.h"
#include "ocudu/support/async/async_task_scheduler.h"

/// Notice this would be the only place were we include concrete class implementation files.

using namespace ocudu;
using namespace ocucp;

std::unique_ptr<nrppa_interface> ocudu::ocucp::create_nrppa(const std::vector<supported_tracking_area>& supported_tas,
                                                            nrppa_cu_cp_notifier&                       cu_cp_notifier,
                                                            async_task_scheduler& common_task_sched,
                                                            timer_manager&        timers,
                                                            task_executor&        task_exec)
{
  auto nrppa = std::make_unique<nrppa_impl>(supported_tas, cu_cp_notifier, common_task_sched, timers, task_exec);
  return nrppa;
}
