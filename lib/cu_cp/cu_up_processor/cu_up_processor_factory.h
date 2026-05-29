// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "cu_up_processor.h"
#include "cu_up_processor_config.h"
#include "ocudu/e1ap/common/e1ap_common.h"
#include "ocudu/support/async/async_task_scheduler.h"
#include <memory>

namespace ocudu::ocucp {

/// Creates an instance of an CU-UP processor interface
std::unique_ptr<cu_up_processor> create_cu_up_processor(cu_up_processor_config_t cu_up_processor_config_,
                                                        e1ap_message_notifier&   e1ap_notifier_,
                                                        e1ap_cu_cp_notifier&     cu_cp_notifier_,
                                                        async_task_scheduler&    common_task_sched_);

} // namespace ocudu::ocucp
