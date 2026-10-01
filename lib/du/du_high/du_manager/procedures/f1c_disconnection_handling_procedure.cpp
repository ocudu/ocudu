// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "f1c_disconnection_handling_procedure.h"
#include "../du_cell_manager.h"
#include "du_cell_stop_procedure.h"
#include "du_setup_procedure.h"
#include "ocudu/adt/format.h"
#include "ocudu/support/async/async_timer.h"

using namespace ocudu;
using namespace ocudu::odu;

f1c_disconnection_handling_procedure::f1c_disconnection_handling_procedure(du_proc_context_view ctxt_) :
  ctxt(ctxt_), proc_logger(ctxt.logger, "F1-C Disconnection Handling")
{
}

void f1c_disconnection_handling_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  proc_logger.log_proc_started();

  // Stop all cells.
  // Note: Do not delete the cells until the FAPI supports stop.
  for (; i != ctxt.cell_mng.nof_cells(); ++i) {
    if (ctxt.cell_mng.is_cell_active(to_du_cell_index(i))) {
      CORO_AWAIT(launch_async<du_cell_stop_procedure>(ctxt.ue_mng,
                                                      ctxt.cell_mng,
                                                      ctxt.params,
                                                      to_du_cell_index(i),
                                                      du_cell_stop_procedure::ue_removal_mode::no_f1_triggers));
    }
  }

  proc_logger.log_progress("DU activity was stopped. Attempting to re-establish F1-C connection..");

  // Attempt a new F1 setup connection.
  CORO_AWAIT(launch_async<du_setup_procedure>(ctxt, du_start_request{du_start_request::unlimited_retries}));

  proc_logger.log_proc_completed();

  CORO_RETURN();
}
