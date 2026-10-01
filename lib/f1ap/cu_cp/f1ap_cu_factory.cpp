// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/f1ap/cu_cp/f1ap_cu_factory.h"
#include "f1ap_cu_impl.h"
#include "ocudu/adt/format.h"

/// Notice this would be the only place were we include concrete class implementation files.

using namespace ocudu;
using namespace ocucp;

std::unique_ptr<f1ap_cu> ocudu::ocucp::create_f1ap(const f1ap_configuration&   f1ap_cfg_,
                                                   f1ap_message_notifier&      f1ap_pdu_notifier_,
                                                   f1ap_du_processor_notifier& du_processor_notifier_,
                                                   timer_manager&              timers_,
                                                   task_executor&              ctrl_exec_)
{
  auto f1ap_cu =
      std::make_unique<f1ap_cu_impl>(f1ap_cfg_, f1ap_pdu_notifier_, du_processor_notifier_, timers_, ctrl_exec_);
  return f1ap_cu;
}
