// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "trp_information_exchange_routine.h"
#include "ocudu/ran/cu_cp_types.h"
#include "ocudu/support/async/coroutine.h"

using namespace ocudu::ocucp;

trp_information_exchange_routine::trp_information_exchange_routine(
    const trp_information_request_t&                request_,
    du_processor_repository&                        du_db_,
    std::map<cu_cp_du_index_t, nrppa_f1ap_adapter>& nrppa_f1ap_ev_notifiers_) :
  request(request_),
  du_db(du_db_),
  nrppa_f1ap_ev_notifiers(nrppa_f1ap_ev_notifiers_),
  logger(ocudulog::fetch_basic_logger("CU-CP"))
{
}

void trp_information_exchange_routine::operator()(coro_context<async_task<trp_information_cu_cp_response_t>>& ctx)
{
  CORO_BEGIN(ctx);

  du_indexes = du_db.get_du_processor_indexes();

  // TODO: Use when_all.
  for (du_it = du_indexes.begin(); du_it != du_indexes.end(); du_it++) {
    du_index = *du_it.base();

    f1ap = &du_db.get_du_processor(du_index).get_f1ap_handler();

    CORO_AWAIT_VALUE(sub_proc_outcome, f1ap->handle_trp_information_request(request));

    handle_sub_procedure_outcome();
  }

  CORO_RETURN(result_msg);
}

void trp_information_exchange_routine::handle_sub_procedure_outcome()
{
  if (sub_proc_outcome.has_value()) {
    // Create and store NRPPA F1AP adapters.
    nrppa_f1ap_ev_notifiers.emplace(du_index, nrppa_f1ap_adapter{});
    nrppa_f1ap_ev_notifiers.at(du_index).connect_f1ap(*f1ap);

    // Append result to the response message.
    result_msg.trp_info_responses[du_index] = sub_proc_outcome.value();
    result_msg.f1ap_notifiers.emplace(du_index, &nrppa_f1ap_ev_notifiers.at(du_index));
  } else {
    logger.warning("TRP Information Exchange Procedure failed for DU index={}", du_index);
  }
}
