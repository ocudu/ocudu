// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ue_creation_procedure.h"
#include "proc_logger.h"
#include "ocudu/adt/format.h"
#include "ocudu/support/async/when_all.h"

using namespace ocudu;

void mac_ue_create_request_procedure::operator()(coro_context<async_task<mac_ue_create_response>>& ctx)
{
  CORO_BEGIN(ctx);
  logger.debug("{}: started...", mac_log_prefix(req.ue_index, req.crnti, name()));

  // > Create UE in MAC CTRL.
  crnti_assigned = ctrl_unit.add_ue(req.ue_index, req.cell_index, req.crnti);
  if (crnti_assigned == rnti_t::INVALID_RNTI) {
    CORO_AWAIT(cancel_ue_creation());
    CORO_EARLY_RETURN(handle_mac_ue_create_result(false));
  }

  // > Update C-RNTI of the UE if it changed.
  req.crnti = crnti_assigned;

  // > Create UE DL and UL context and channels.
  CORO_AWAIT_VALUE(add_mac_ue_result, when_all(create_ul_dl_ue_tasks()));
  if (not add_mac_ue_result[0] or not add_mac_ue_result[1]) {
    CORO_AWAIT(cancel_ue_creation());
    CORO_EARLY_RETURN(handle_mac_ue_create_result(false));
  }
  logger.debug("{}: UE context created successfully", mac_log_prefix(req.ue_index, req.crnti, name()));

  // > Create UE context in Scheduler.
  CORO_AWAIT_VALUE(add_sched_ue_result, sched_configurator.handle_ue_creation_request(req));
  if (not add_sched_ue_result) {
    // >> Terminate procedure.
    CORO_AWAIT(cancel_ue_creation());
    CORO_EARLY_RETURN(handle_mac_ue_create_result(false));
  }

  // > After UE insertion in scheduler, send response to DU manager.
  CORO_RETURN(handle_mac_ue_create_result(true));
}

async_task<void> mac_ue_create_request_procedure::cancel_ue_creation()
{
  return launch_async([this](coro_context<async_task<void>>& ctx) {
    CORO_BEGIN(ctx);

    if (not add_mac_ue_result.empty() and add_mac_ue_result[1]) {
      // > Revert creation of UE in MAC DL.
      CORO_AWAIT(dl_unit.remove_ue(mac_ue_delete_request{req.cell_index, req.ue_index, req.crnti}));
    }

    if (not add_mac_ue_result.empty() and add_mac_ue_result[0]) {
      // > Revert creation of UE in MAC UL.
      CORO_AWAIT(ul_unit.remove_ue(mac_ue_delete_request{req.cell_index, req.ue_index, req.crnti}));
    }

    if (crnti_assigned != rnti_t::INVALID_RNTI) {
      // > Revert creation of UE in MAC CTRL.
      ctrl_unit.remove_ue(req.ue_index);
    }

    CORO_RETURN();
  });
}

std::vector<async_task<bool>> mac_ue_create_request_procedure::create_ul_dl_ue_tasks()
{
  // Note: Helper method created because initializer list does not support std::move.
  std::vector<async_task<bool>> tasks;
  tasks.reserve(2);
  tasks.push_back(ul_unit.add_ue(req));
  tasks.push_back(dl_unit.add_ue(req));
  return tasks;
}

mac_ue_create_response mac_ue_create_request_procedure::handle_mac_ue_create_result(bool result)
{
  if (result) {
    logger.info("{}: finished successfully", mac_log_prefix(req.ue_index, req.crnti, name()));
  } else {
    logger.warning("{}: failed", mac_log_prefix(req.ue_index, req.crnti, name()));
  }

  // Respond back to DU manager with result
  mac_ue_create_response resp{};
  resp.ue_index        = req.ue_index;
  resp.cell_index      = req.cell_index;
  resp.allocated_crnti = result ? crnti_assigned : rnti_t::INVALID_RNTI;
  return resp;
}
