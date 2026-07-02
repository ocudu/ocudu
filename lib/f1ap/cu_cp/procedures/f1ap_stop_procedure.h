// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/support/async/async_task.h"

namespace ocudu {
namespace ocucp {

class f1ap_du_processor_notifier;
class f1ap_ue_context_list;
class f1ap_event_manager;

/// Procedure called during the shutdown of the F1AP to clear up existing transactions.
class f1ap_stop_procedure
{
public:
  f1ap_stop_procedure(f1ap_du_processor_notifier& cu_cp_notifier_,
                      f1ap_ue_context_list&       ue_ctxt_list_,
                      f1ap_event_manager&         ev_mng_);

  void operator()(coro_context<async_task<void>>& ctx);

private:
  async_task<void> handle_transaction_info_loss();

  f1ap_du_processor_notifier& cu_cp_notifier;
  f1ap_ue_context_list&       ue_ctxt_list;
  f1ap_event_manager&         ev_mng;
};

} // namespace ocucp
} // namespace ocudu
