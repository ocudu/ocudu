// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "e1ap_stop_procedure.h"
#include "../ue_context/e1ap_cu_cp_ue_context.h"
#include "e1ap_transaction_manager.h"
#include "ocudu/e1ap/cu_cp/e1ap_cu_cp.h"

using namespace ocudu;
using namespace ocucp;

e1ap_stop_procedure::e1ap_stop_procedure(e1ap_cu_cp_notifier&      cu_cp_notifier_,
                                         e1ap_ue_context_list&     ue_ctxt_list_,
                                         e1ap_transaction_manager& ev_mng_) :
  cu_cp_notifier(cu_cp_notifier_), ue_ctxt_list(ue_ctxt_list_), ev_mng(ev_mng_)
{
}

void e1ap_stop_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  // Stop all UEs associated with this CU-UP.
  if (ue_ctxt_list.size() > 0) {
    CORO_AWAIT(handle_transaction_info_loss());
  }

  // Cancel all CU-CP-wide (non-UE-associated) transactions, e.g. E1 Reset.
  ev_mng.transactions.stop();

  CORO_RETURN();
}

async_task<void> e1ap_stop_procedure::handle_transaction_info_loss()
{
  ue_transaction_info_loss_event ev;
  ev.ues_lost.reserve(ue_ctxt_list.size());

  // Add CU-UP UEs to the list of UEs with transaction information lost.
  ev.ues_lost.reserve(ue_ctxt_list.size());
  for (auto& ue : ue_ctxt_list) {
    ev.ues_lost.push_back(ue.second.ue_ids.ue_index);
  }

  // After receiving an E1 Release Request, no more E1AP Rx PDUs are expected. Cancel running UE E1AP transactions.
  // Note: size of ue_list may change during this operation (e.g. if a concurrent UE context release was being
  // processed and got cancelled). Therefore, we leverage the list ev.ues_lost for the iteration.
  for (cu_cp_ue_index_t ue_idx : ev.ues_lost) {
    auto* u = ue_ctxt_list.find_ue(ue_idx);
    if (u != nullptr) {
      u->bearer_ev_mng.cancel_all();
    }
  }
  ev.ues_lost.erase(std::remove_if(ev.ues_lost.begin(),
                                   ev.ues_lost.end(),
                                   [this](cu_cp_ue_index_t ue_idx) { return ue_ctxt_list.find_ue(ue_idx) == nullptr; }),
                    ev.ues_lost.end());

  return cu_cp_notifier.on_transaction_info_loss(ev);
}
