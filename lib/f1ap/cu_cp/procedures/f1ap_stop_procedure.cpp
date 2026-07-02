// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "f1ap_stop_procedure.h"
#include "../ue_context/f1ap_cu_ue_context.h"
#include "f1ap_cu_event_manager.h"

using namespace ocudu;
using namespace ocucp;

f1ap_stop_procedure::f1ap_stop_procedure(f1ap_du_processor_notifier& cu_cp_notifier_,
                                         f1ap_ue_context_list&       ue_ctxt_list_,
                                         f1ap_event_manager&         ev_mng_) :
  cu_cp_notifier(cu_cp_notifier_), ue_ctxt_list(ue_ctxt_list_), ev_mng(ev_mng_)
{
}

void f1ap_stop_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  // Stop all UEs associated with this DU.
  if (ue_ctxt_list.size() > 0) {
    CORO_AWAIT(handle_transaction_info_loss());
  }

  // Cancel all DU-wide (non-UE-associated) transactions, e.g. Write-Replace Warning, TRP information exchange,
  // positioning measurement, gNB-CU configuration update.
  ev_mng.transactions.stop();

  CORO_RETURN();
}

async_task<void> f1ap_stop_procedure::handle_transaction_info_loss()
{
  ue_transaction_info_loss_event ev;
  ev.ues_lost.reserve(ue_ctxt_list.size());

  // Add DU UEs to the list of UEs with transaction information lost.
  ev.ues_lost.reserve(ue_ctxt_list.size());
  for (auto& ue : ue_ctxt_list) {
    ev.ues_lost.push_back(ue.second.ue_ids.ue_index);
  }

  // After receiving an F1 Removal Request, no more F1AP Rx PDUs are expected. Cancel running UE F1AP transactions.
  // Note: size of ue_list may change during this operation (e.g. if a concurrent UE context release was being
  // processed and got cancelled). Therefore, we leverage the list ev.ues_lost for the iteration.
  for (cu_cp_ue_index_t ue_idx : ev.ues_lost) {
    auto* u = ue_ctxt_list.find(ue_idx);
    if (u != nullptr) {
      u->ev_mng.cancel_all();
    }
  }
  ev.ues_lost.erase(std::remove_if(ev.ues_lost.begin(),
                                   ev.ues_lost.end(),
                                   [this](cu_cp_ue_index_t ue_idx) { return ue_ctxt_list.find(ue_idx) == nullptr; }),
                    ev.ues_lost.end());

  return cu_cp_notifier.on_transaction_info_loss(std::move(ev));
}
