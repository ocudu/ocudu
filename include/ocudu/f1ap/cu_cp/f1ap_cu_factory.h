// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/f1ap/cu_cp/f1ap_configuration.h"
#include "ocudu/f1ap/cu_cp/f1ap_cu.h"
#include "ocudu/f1ap/f1ap_message_notifier.h"
#include "ocudu/support/executors/task_executor.h"
#include <memory>

namespace ocudu {
namespace ocucp {

/// Creates an instance of an F1AP interface, notifying outgoing packets on the specified listener object.
std::unique_ptr<f1ap_cu> create_f1ap(const f1ap_configuration&   f1ap_cfg_,
                                     f1ap_message_notifier&      f1ap_pdu_notifier_,
                                     f1ap_du_processor_notifier& du_processor_notifier_,
                                     timer_manager&              timers_,
                                     task_executor&              ctrl_exec_);

} // namespace ocucp
} // namespace ocudu
