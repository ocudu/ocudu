// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "du_stop_procedure.h"
#include "await_all_ues.h"
#include "ocudu/adt/format.h"
#include "ocudu/support/async/async_timer.h"
#include "ocudu/support/async/execute_on.h"

using namespace ocudu;
using namespace odu;

du_stop_procedure::du_stop_procedure(du_ue_manager&           ue_mng_,
                                     du_cell_manager&         cell_mng_,
                                     f1ap_connection_manager& f1ap_conn_mng_) :
  ue_mng(ue_mng_),
  cell_mng(cell_mng_),
  f1ap_conn_mng(f1ap_conn_mng_),
  proc_logger(ocudulog::fetch_basic_logger("DU-MNG"), "DU Stop")
{
}

void du_stop_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  proc_logger.log_proc_started();

  // Stop all cells.
  // Note: This will stop all scheduling activity and unlock any pending scheduler UE update procedure.
  CORO_AWAIT(cell_mng.stop_all());

  proc_logger.log_progress("Stopped all cells");

  // Stop all UE traffic
  CORO_AWAIT(stop_ue_traffic());

  // Run F1 Remove Procedure.
  CORO_AWAIT(f1ap_conn_mng.handle_f1_removal_request());

  // Forcefully interrupt all UE activity.
  CORO_AWAIT(ue_mng.stop());

  proc_logger.log_proc_completed();

  CORO_RETURN();
}

async_task<void> du_stop_procedure::stop_ue_traffic()
{
  return await_all_ues(ue_mng, [](du_ue& u) { return u.handle_activity_stop_request(true); });
}
