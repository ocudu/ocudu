// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/cu_cp/cu_cp_configuration.h"
#include "ocudu/nrppa/nrppa.h"
#include "ocudu/support/async/async_task_scheduler.h"

namespace ocudu::ocucp {

/// Creates an instance of an NRPPA interface, notifying outgoing packets on the specified listener object.
std::unique_ptr<nrppa_interface> create_nrppa(const cu_cp_configuration& cfg,
                                              nrppa_cu_cp_notifier&      cu_cp_notifier,
                                              async_task_scheduler&      common_task_sched);

} // namespace ocudu::ocucp
