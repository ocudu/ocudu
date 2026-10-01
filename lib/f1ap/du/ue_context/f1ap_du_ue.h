// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "f1ap_ue_context.h"
#include "ue_bearer_manager.h"
#include "ocudu/f1ap/du/f1ap_du.h"
#include "ocudu/f1ap/f1ap_ue_id_types.h"
#include "ocudu/ran/du_types.h"

namespace ocudu {
namespace odu {

class f1ap_du_ue
{
public:
  f1ap_du_ue(du_ue_index_t          ue_index_,
             gnb_du_ue_f1ap_id_t    gnb_f1ap_du_ue_id_,
             f1ap_du_configurator&  du_handler_,
             f1ap_message_notifier& f1ap_msg_notifier_,
             task_executor&         ctrl_exec,
             task_executor&         ue_exec,
             timer_manager&         timers) :
    context(ue_index_, gnb_f1ap_du_ue_id_),
    f1ap_msg_notifier(f1ap_msg_notifier_),
    du_handler(du_handler_),
    bearers(context, f1ap_msg_notifier, du_handler, ctrl_exec, ue_exec, timers)
  {
  }

  f1ap_ue_context        context;
  f1ap_message_notifier& f1ap_msg_notifier;
  f1ap_du_configurator&  du_handler;
  ue_bearer_manager      bearers;
};

} // namespace odu
} // namespace ocudu
