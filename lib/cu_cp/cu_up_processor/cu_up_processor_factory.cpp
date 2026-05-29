// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "cu_up_processor_factory.h"
#include "cu_up_processor_impl.h"
#include "ocudu/support/async/async_task_scheduler.h"

/// Notice this would be the only place were we include concrete class implementation files.

using namespace ocudu;
using namespace ocucp;

std::unique_ptr<cu_up_processor>
ocudu::ocucp::create_cu_up_processor(const cu_up_processor_config_t cu_up_processor_config_,
                                     e1ap_message_notifier&         e1ap_notifier_,
                                     e1ap_cu_cp_notifier&           cu_cp_notifier_,
                                     async_task_scheduler&          common_task_sched_)
{
  auto cu_up_processor = std::make_unique<cu_up_processor_impl>(
      cu_up_processor_config_, e1ap_notifier_, cu_cp_notifier_, common_task_sched_);
  return cu_up_processor;
}
