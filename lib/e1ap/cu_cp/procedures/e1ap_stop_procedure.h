// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/support/async/async_task.h"

namespace ocudu {
namespace ocucp {

class e1ap_cu_cp_notifier;
class e1ap_ue_context_list;
class e1ap_transaction_manager;

/// Procedure called during the shutdown of the E1AP to clear up existing transactions.
class e1ap_stop_procedure
{
public:
  e1ap_stop_procedure(e1ap_cu_cp_notifier&      cu_cp_notifier_,
                      e1ap_ue_context_list&     ue_ctxt_list_,
                      e1ap_transaction_manager& ev_mng_);

  void operator()(coro_context<async_task<void>>& ctx);

private:
  async_task<void> handle_transaction_info_loss();

  e1ap_cu_cp_notifier&      cu_cp_notifier;
  e1ap_ue_context_list&     ue_ctxt_list;
  e1ap_transaction_manager& ev_mng;
};

} // namespace ocucp
} // namespace ocudu
