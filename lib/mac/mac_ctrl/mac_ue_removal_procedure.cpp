// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "mac_ue_removal_procedure.h"
#include "proc_logger.h"
#include "ocudu/support/async/when_all.h"

using namespace ocudu;

void mac_ue_removal_procedure::operator()(coro_context<async_task<mac_ue_delete_response>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.debug("{}: started...", mac_log_prefix(req.ue_index, req.rnti, name()));

  // > Remove UE from scheduler.
  // Note: Removing the UE from the scheduler before the MAC avoids potential race conditions (assuming the scheduler
  // doesn't allocate UEs after being removed).
  CORO_AWAIT(sched_configurator.handle_ue_removal_request(req));

  // > Remove the UE from the MAC DL and MAC UL concurrently. They run on independent executors (DL cell executor and
  // UL PDU executor) over disjoint state, so racing them halves the per-UE round-trips back to the CTRL executor.
  CORO_AWAIT(when_all(create_ul_dl_rem_tasks()));

  // > Enqueue UE deletion
  ctrl_mac.remove_ue(req.ue_index);

  logger.info("{}: finished successfully", mac_log_prefix(req.ue_index, req.rnti, name()));

  // 4. Signal end of procedure and pass response
  CORO_RETURN(mac_ue_delete_response{true});
}

std::vector<async_task<void>> mac_ue_removal_procedure::create_ul_dl_rem_tasks()
{
  std::vector<async_task<void>> tasks;
  tasks.reserve(2);
  tasks.push_back(ul_mac.remove_ue(req));
  tasks.push_back(dl_mac.remove_ue(req));
  return tasks;
}
