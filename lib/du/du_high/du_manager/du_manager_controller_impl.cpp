// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "du_manager_controller_impl.h"
#include "du_manager_context.h"
#include "procedures/du_setup_procedure.h"
#include "procedures/du_stop_procedure.h"
#include "ocudu/adt/format.h"
#include "ocudu/ntn/ntn_configuration_manager.h"
#include "ocudu/support/async/fifo_async_task_scheduler.h"
#include "ocudu/support/executors/task_executor.h"
#include "ocudu/support/ocudu_assert.h"

using namespace ocudu;
using namespace odu;

du_manager_controller_impl::du_manager_controller_impl(const du_proc_context_view&           proc_ctxt_,
                                                       fifo_async_task_scheduler&            task_sched_,
                                                       ocudu_ntn::ntn_configuration_manager* ntn_mng_) :
  main_task_sched(task_sched_), proc_ctxt(proc_ctxt_), ntn_mng(ntn_mng_)
{
}

void du_manager_controller_impl::start()
{
  // Check if not already running.
  if (running_guard_flag) {
    proc_ctxt.logger.warning("Discarding DU start request. Cause: DU Manager already started.");
    return;
  }
  running_guard_flag = true;

  const unsigned max_setup_attempts =
      proc_ctxt.params.f1ap.retry_tnl_connection ? du_start_request::unlimited_retries : 1;

  // The setup is only awaited if it is bound to either complete or close the application. When the F1-C TNL connection
  // is retried indefinitely, the wait is unbounded, and blocking here would leave the application unable to process a
  // shutdown request for as long as the CU-CP is unreachable. The setup then runs in the background instead.
  const bool wait_for_setup = max_setup_attempts != du_start_request::unlimited_retries;

  // Note: sync_event blocks on destruction until every token is gone, so a token is only handed to the setup task if
  // its completion is awaited.
  sync_event        setup_ev;
  scoped_sync_token setup_tk = wait_for_setup ? setup_ev.get_token() : scoped_sync_token{};

  sync_event dispatch_ev;
  if (not proc_ctxt.params.services.du_mng_exec.execute(
          [this, tk = dispatch_ev.get_token(), setup_tk = std::move(setup_tk), max_setup_attempts]() mutable {
            // Configure the cells before the F1 interface is set up, so that the layers below are ready to run by the
            // time this function returns. They are only activated once the CU-CP accepted the F1 Setup.
            configure_du_cells(proc_ctxt);

            main_task_sched.schedule(
                [this, tk = std::move(setup_tk), max_setup_attempts](coro_context<async_task<void>>& ctx) {
                  CORO_BEGIN(ctx);

                  // Connect to CU-CP and send F1 Setup Request and await for F1 setup response.
                  // Note: If the retries are enabled, the setup is repeated indefinitely, so that the DU does not
                  // require the CU-CP to be reachable on startup. Otherwise, a single attempt is made and the
                  // application is closed if it fails.
                  CORO_AWAIT(launch_async<du_setup_procedure>(proc_ctxt, du_start_request{max_setup_attempts}));

                  if (proc_ctxt.ctxt.stop_command_received) {
                    // The setup was cancelled by a stop request. Leave the DU in its stopped state.
                    CORO_EARLY_RETURN();
                  }

                  // Update DU state to "running".
                  proc_ctxt.ctxt.running = true;

                  // On tk destruction, caller thread that the operation is complete.
                  CORO_RETURN();
                });
          })) {
    report_fatal_error("Unable to initiate DU setup procedure");
  }

  // Block waiting for the cells to be configured and, if it is awaited, for the DU setup to complete.
  dispatch_ev.wait();
  setup_ev.wait();
  ocudu_sanity_check(running_guard_flag, "DU manager start()/stop() being used in an non-sequential manner");

  // Start the NTN periodic updates from here, as the manager arms its timers in the DU manager execution context and
  // blocks until it is done, which would deadlock if it was requested from that context.
  // Note: If the setup was not awaited, the DU may not be running yet, in which case handle_ntn_param_update()
  // discards the first refreshes. The periodic ones that follow take effect as soon as the DU runs.
  if (ntn_mng != nullptr) {
    ntn_mng->start();
  }
}

void du_manager_controller_impl::stop()
{
  if (not running_guard_flag) {
    // Stop was already requested. Do nothing.
    return;
  }
  running_guard_flag = false;

  // Stop the NTN periodic updates before tearing down the layers below: the manager holds references to this DU
  // manager and to the MAC subframe time mapper, and the MAC is destroyed before the DU manager owning it.
  if (ntn_mng != nullptr) {
    ntn_mng->stop();
  }

  sync_event ev;
  while (not proc_ctxt.params.services.du_mng_exec.execute(
      [this, tk = ev.get_token()]() mutable { handle_du_stop_request(std::move(tk)); })) {
    proc_ctxt.logger.error("Unable to dispatch DU Manager shutdown. Retrying...");
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  // Block waiting for async task to complete.
  ev.wait();
  ocudu_sanity_check(not running_guard_flag, "DU manager start()/stop() being used in an non-sequential manner");
}

void du_manager_controller_impl::handle_du_stop_request(scoped_sync_token tk)
{
  // Notify other procedures that the DU needs to stop.
  // Note: If the DU is in the process of being setup, this cancels the procedure.
  proc_ctxt.ctxt.stop_command_received = true;

  // Start DU stop procedure. The task is queued behind a setup procedure that may still be retrying the F1-C
  // connection, so it only runs once that procedure returned.
  main_task_sched.schedule(launch_async([this, tk = std::move(tk)](coro_context<async_task<void>>& ctx) mutable {
    CORO_BEGIN(ctx);

    if (proc_ctxt.ctxt.running) {
      // Tear down activity in remaining layers. There is nothing to tear down if the DU never got to run, e.g. when
      // the setup above was cancelled while it was still waiting for the CU-CP: no cell was activated, no UE exists
      // and no F1 interface was set up.
      CORO_AWAIT(launch_async<du_stop_procedure>(proc_ctxt.ue_mng, proc_ctxt.cell_mng, proc_ctxt.params.f1ap.conn_mng));
    }

    // DU stop successfully finished.
    // Dispatch main async task loop destruction via defer so that the current coroutine ends successfully before
    // we start destroying the DU manager (in case stop is called from dtor).
    // Note: tk is captured by copy so that a failed defer (which destroys the passed task and its token copy) does
    // not prematurely signal the sync_event nor leave tk in a moved-from state for the retries.
    while (not proc_ctxt.params.services.du_mng_exec.defer([this, tk]() {
      // Let main loop go out of scope and be destroyed.
      auto main_loop = main_task_sched.request_stop();

      proc_ctxt.ctxt.running               = false;
      proc_ctxt.ctxt.stop_command_received = false;
    })) {
      proc_ctxt.logger.warning("Unable to stop DU Manager. Retrying...");
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    CORO_RETURN();
  }));
}
