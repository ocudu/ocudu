// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/nrppa/nrppa.h"
#include "ocudu/ran/supported_tracking_area.h"
#include "ocudu/support/async/async_task_scheduler.h"
#include "ocudu/support/timers.h"

namespace ocudu::ocucp {

/// Creates an instance of an NRPPA interface, notifying outgoing packets on the specified listener object.
std::unique_ptr<nrppa_interface> create_nrppa(const std::vector<supported_tracking_area>& supported_tas,
                                              nrppa_cu_cp_notifier&                       cu_cp_notifier,
                                              async_task_scheduler&                       common_task_sched,
                                              timer_manager&                              timers,
                                              task_executor&                              task_exec);

} // namespace ocudu::ocucp
