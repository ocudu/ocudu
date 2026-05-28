// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/e1ap/cu_cp/e1ap_configuration.h"
#include "ocudu/e1ap/cu_cp/e1ap_cu_cp.h"
#include "ocudu/support/executors/task_executor.h"
#include "ocudu/support/timers.h"
#include <memory>

namespace ocudu::ocucp {

/// Creates an instance of an E1AP interface, notifying outgoing packets on the specified listener object.
std::unique_ptr<e1ap_cu_cp> create_e1ap(const e1ap_configuration&      e1ap_cfg_,
                                        cu_cp_cu_up_index_t            cu_up_index_,
                                        e1ap_message_notifier&         e1ap_pdu_notifier_,
                                        e1ap_cu_up_processor_notifier& e1ap_cu_up_processor_notifier_,
                                        e1ap_cu_cp_notifier&           cu_cp_notifier_,
                                        timer_manager&                 timers_,
                                        task_executor&                 ctrl_exec_,
                                        unsigned                       max_nof_supported_ues_);

} // namespace ocudu::ocucp
