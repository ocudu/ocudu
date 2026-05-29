// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/nrppa/nrppa_factory.h"
#include "nrppa_impl.h"
#include "ocudu/support/async/async_task_scheduler.h"

/// Notice this would be the only place were we include concrete class implementation files.

using namespace ocudu;
using namespace ocucp;

std::unique_ptr<nrppa_interface> ocudu::ocucp::create_nrppa(const cu_cp_configuration& cfg,
                                                            nrppa_cu_cp_notifier&      cu_cp_notifier,
                                                            async_task_scheduler&      common_task_sched)
{
  auto nrppa = std::make_unique<nrppa_impl>(cfg, cu_cp_notifier, common_task_sched);
  return nrppa;
}
