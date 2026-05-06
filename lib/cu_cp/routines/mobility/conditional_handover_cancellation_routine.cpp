// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "conditional_handover_cancellation_routine.h"
#include "../../ue_manager/ue_manager_impl.h"
#include "mobility_helpers.h"

using namespace ocudu;
using namespace ocudu::ocucp;

conditional_handover_cancellation_routine::conditional_handover_cancellation_routine(cu_cp_ue_index_t source_ue_index_,
                                                                                     ue_manager&      ue_mng_,
                                                                                     xnap_repository* xnap_db_,
                                                                                     ocudulog::basic_logger& logger_) :
  source_ue_index(source_ue_index_), ue_mng(ue_mng_), xnap_db(xnap_db_), logger(logger_)
{
}

void conditional_handover_cancellation_routine::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  source_ue = ue_mng.find_du_ue(source_ue_index);
  if (source_ue == nullptr) {
    CORO_EARLY_RETURN();
  }

  // Only proceed if CHO is still in the execution state.
  // conditional_handover_source_routine claims completion by transitioning to completion first.
  if (!source_ue->get_cho_context().has_value() ||
      source_ue->get_cho_context()->state != cu_cp_ue_cho_context::state_t::execution) {
    CORO_EARLY_RETURN();
  }

  // Claim ownership before any CORO_AWAIT to prevent conditional_handover_source_routine from racing.
  source_ue->get_cho_context()->state = cu_cp_ue_cho_context::state_t::completion;

  logger.info("ue={}: CHO execution timed out. Cancelling.", source_ue_index);

  // Build RRC removal request with condRecfgToRemList-r16.
  removal_request = {};
  removal_request.cho_cancellation_ids.emplace();
  for (const auto& candidate : source_ue->get_cho_context()->candidates) {
    if (candidate.cond_recfg_id.valid()) {
      removal_request.cho_cancellation_ids->push_back(candidate.cond_recfg_id.value());
    }
  }

  CORO_AWAIT_VALUE(removal_result, source_ue->get_rrc_ue()->handle_rrc_reconfiguration_request(removal_request));
  if (!removal_result) {
    logger.warning("ue={}: CHO cancellation RRC removal reconfig failed. Proceeding with target release.",
                   source_ue_index);
  }

  // Re-fetch source UE after CORO_AWAIT (it may have been released).
  source_ue = ue_mng.find_du_ue(source_ue_index);
  if (source_ue == nullptr) {
    CORO_EARLY_RETURN();
  }

  // Cancel inter-CU candidates via Xn and each intra-CU candidate's RRC reconfiguration transaction.
  const unsigned cancelled = cancel_cho_candidates(*source_ue, ue_mng, xnap_db);

  // clear() stops the timer defensively and resets state to idle.
  source_ue->get_cho_context()->clear();

  logger.info("ue={}: CHO cancellation complete. {} candidates cancelled.", source_ue_index, cancelled);

  CORO_RETURN();
}
