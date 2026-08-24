// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "logical_cell_controller.h"
#include "bounded_executor_dispatch.h"
#include "routines/cell_activation_routine.h"
#include "routines/cell_barring_routine.h"
#include "routines/cell_deactivation_routine.h"
#include "routines/cell_lifecycle_target.h"
#include "ocudu/support/async/async_no_op_task.h"
#include "ocudu/support/async/coroutine.h"
#include "ocudu/support/async/execute_on_blocking.h"

using namespace ocudu;
using namespace ocudu::ocucp;

logical_cell_controller::logical_cell_controller(const cu_cp_configuration&        cfg_,
                                                 du_processor_repository&          du_db_,
                                                 ue_manager&                       ue_mng_,
                                                 async_task_scheduler&             common_task_sched_,
                                                 cu_cp_ue_context_release_handler& ue_release_handler_) :
  cfg(cfg_),
  du_db(du_db_),
  ue_mng(ue_mng_),
  common_task_sched(common_task_sched_),
  ue_release_handler(ue_release_handler_),
  logical_cells(cfg.cells)
{
}

async_task<cu_cp_cell_command_response> logical_cell_controller::deactivate_cell(const nr_cell_global_id_t& cgi)
{
  // Strict served-cells lookup: a cell that is already deactivated cannot be deactivated again.
  cu_cp_du_index_t du_index = du_db.find_du(cgi);
  if (du_index == cu_cp_du_index_t::invalid) {
    logger.warning("deactivate_cell rejected. Cause: No DU found serving NR-CGI plmn={} nci={:#x}",
                   cgi.plmn_id.to_string(),
                   cgi.nci.value());
    return launch_no_op_task(cu_cp_cell_command_response{});
  }

  std::vector<cell_lifecycle_target> targets = {cell_lifecycle_target{du_index, cgi, std::nullopt, {}}};

  return launch_async(
      [this,
       cgi,
       du_index,
       targets        = std::move(targets),
       ues_to_release = std::vector<cu_cp_ue_index_t>{},
       prev_state     = std::optional<cell_admin_state>{},
       result = cell_deactivation_result{}](coro_context<async_task<cu_cp_cell_command_response>>& ctx) mutable {
        CORO_BEGIN(ctx);
        // Hold the administrative state as shutting_down while the graceful stop drains the cell; it becomes
        // locked when the stop completes, so the recorded intent survives DU restarts and AMF reconnections.
        // Recorded inside the task: a command that is never scheduled must not leave intent behind. Every
        // reported cell is realized as a logical cell at F1 setup, so a missing record means the registry and
        // the DU records disagree: fail the command instead of mutating only the DU-side state.
        prev_state = logical_cells.set_admin_state(cgi.nci, cell_admin_state::shutting_down);
        if (!prev_state.has_value()) {
          CORO_EARLY_RETURN(cu_cp_cell_command_response{false});
        }
        // The CU-CP drives the full graceful stop (bar, then release the cell's UEs, then deactivate), rather
        // than relying on the DU to autonomously bar/drain, so that the behaviour does not depend on DU-specific
        // cell-stop handling (which is not mandated by F1AP). The UEs are collected when the task runs, not when
        // the command was created: UEs attaching while the task was queued must be drained too.
        ues_to_release = collect_ues_on_cell(du_db, ue_mng, du_index, cgi);
        CORO_AWAIT_VALUE(
            result,
            launch_async<cell_deactivation_routine>(cfg,
                                                    std::move(targets),
                                                    std::move(ues_to_release),
                                                    ngap_cause_t{ngap_cause_radio_network_t::cell_not_available},
                                                    /* bar_cells_first = */ true,
                                                    du_db,
                                                    logical_cells,
                                                    ue_release_handler,
                                                    ue_mng,
                                                    logger));
        if (result.success) {
          // The graceful stop completed: the cell is now administratively locked.
          logical_cells.set_admin_state(cgi.nci, cell_admin_state::locked);
          CORO_EARLY_RETURN(cu_cp_cell_command_response{true});
        }
        // The failed stop can resume inside the executor task that is removing the DU, before the cell is
        // de-realized: re-post to the back of the CU-CP executor so the removal settles before the state is
        // resolved from what actually took effect.
        CORO_AWAIT(defer_on_blocking(*cfg.services.cu_cp_executor, *cfg.services.timers));
        // Resolve the recorded state from the outcome, not from the command result alone: the stop's stages can
        // partially complete, and the state must match the cell, or a later F1 setup or AMF reconnection
        // resurrects a cell the operator took down. De-realization resolves an interrupted stop itself (the DU
        // vanished mid-stop); only resolve here while the stop still owns the shutting_down transient.
        if (const logical_cell* cell = logical_cells.find_cell(cgi.nci);
            cell != nullptr && cell->admin_state == cell_admin_state::shutting_down) {
          if (cell->operational_state == cell_operational_state::disabled) {
            // The deactivation took effect: the cell is off the air and administratively locked, even though an
            // earlier stage failed the command.
            logical_cells.set_admin_state(cgi.nci, cell_admin_state::locked);
          } else {
            // The cell is still on the air: restore the previous administrative state. If the stop's bar stage
            // was acknowledged, the cell remains barred at the DU — record the barred intent so the registry
            // matches the on-air state. The record is transient: the registry clears it when the DU goes away
            // or a later deactivation takes effect, and an operator cell_bar/cell_unbar takes it over.
            logical_cells.set_admin_state(cgi.nci, *prev_state);
            if (result.bars_acked) {
              logical_cells.record_failed_stop_bar(cgi.nci);
            }
          }
        }
        CORO_RETURN(cu_cp_cell_command_response{false});
      });
}

async_task<cu_cp_cell_command_response> logical_cell_controller::activate_cell(const nr_cell_global_id_t& cgi)
{
  // Any-state lookup: the cell to activate is currently in the DU's deactivated list, which the strict
  // served-cells lookup would miss.
  cu_cp_du_index_t du_index = du_db.find_du_any_state(cgi);
  if (du_index == cu_cp_du_index_t::invalid) {
    logger.warning("activate_cell rejected. Cause: No DU found serving NR-CGI plmn={} nci={:#x}",
                   cgi.plmn_id.to_string(),
                   cgi.nci.value());
    return launch_no_op_task(cu_cp_cell_command_response{});
  }

  return launch_async([this,
                       cgi,
                       du_index,
                       targets     = std::vector<cell_lifecycle_target>{},
                       reapply_bar = false,
                       prev_state  = std::optional<cell_admin_state>{},
                       success     = false](coro_context<async_task<cu_cp_cell_command_response>>& ctx) mutable {
    CORO_BEGIN(ctx);
    // Set the administrative state to unlocked when the command actually runs (a command that is never
    // scheduled must not mutate intent), and check whether barred intent has to be re-applied after the
    // activation. A missing logical cell record fails the command: every reported cell is realized at F1
    // setup.
    prev_state = logical_cells.set_admin_state(cgi.nci, cell_admin_state::unlocked);
    if (!prev_state.has_value()) {
      CORO_EARLY_RETURN(cu_cp_cell_command_response{false});
    }
    reapply_bar = logical_cells.find_cell(cgi.nci)->barred;
    // Restore the PLMNs that were parked when the cell was deactivated, and carry the recorded PCI, so the
    // activation brings the cell back with the same parameters the DU originally reported. Resolved when the
    // task runs, not when the command was created: a lock queued right before this unlock parks the PLMNs
    // only when it executes.
    {
      std::optional<pci_t>       pci;
      std::vector<plmn_identity> plmns_to_activate;
      if (du_processor* du_proc = du_db.find_du_processor(du_index); du_proc != nullptr) {
        if (const du_configuration_context* du_ctxt = du_proc->get_context(); du_ctxt != nullptr) {
          if (const du_cell_configuration* cell_record = du_ctxt->find_cell_any_state(cgi); cell_record != nullptr) {
            pci               = cell_record->pci;
            plmns_to_activate = cell_record->deactivated_plmns;
          }
        }
      }
      targets = {cell_lifecycle_target{du_index, cgi, pci, std::move(plmns_to_activate)}};
    }
    CORO_AWAIT_VALUE(success,
                     launch_async<cell_activation_routine>(cfg, std::move(targets), du_db, logical_cells, logger));
    if (!success) {
      // Restore the previous state, so recorded intent stays consistent with the reported outcome (a failed
      // activation must not leave the cell marked as unlocked).
      logical_cells.set_admin_state(cgi.nci, *prev_state);
    }
    if (success && reapply_bar) {
      // Re-apply the logical cell's barred intent now that the cell is active again. The command reports
      // failure when the re-bar fails, so operator intent and reported outcome stay consistent.
      CORO_AWAIT_VALUE(success,
                       launch_async<cell_barring_routine>(
                           cfg,
                           std::vector<cell_lifecycle_target>{cell_lifecycle_target{du_index, cgi, std::nullopt, {}}},
                           /* barred = */ true,
                           du_db,
                           logger));
    }
    CORO_RETURN(cu_cp_cell_command_response{success});
  });
}

async_task<cu_cp_cell_command_response> logical_cell_controller::bar_cell(const nr_cell_global_id_t& cgi, bool barred)
{
  // Any-state lookup: barred intent can also be recorded for a currently deactivated cell.
  cu_cp_du_index_t du_index = du_db.find_du_any_state(cgi);
  if (du_index == cu_cp_du_index_t::invalid) {
    logger.warning("bar_cell rejected. Cause: No DU found serving NR-CGI plmn={} nci={:#x}",
                   cgi.plmn_id.to_string(),
                   cgi.nci.value());
    return launch_no_op_task(cu_cp_cell_command_response{});
  }

  std::vector<cell_lifecycle_target> targets = {cell_lifecycle_target{du_index, cgi, std::nullopt, {}}};

  return launch_async(
      [this, cgi, barred, targets = std::move(targets), prev_barred = std::optional<bool>{}, success = false](
          coro_context<async_task<cu_cp_cell_command_response>>& ctx) mutable {
        CORO_BEGIN(ctx);
        // Record the operator's barred intent when the command actually runs, so it survives DU restarts and a
        // command that is never scheduled leaves no intent behind. A missing logical cell record fails the
        // command: every reported cell is realized at F1 setup.
        prev_barred = logical_cells.set_barred(cgi.nci, barred);
        if (!prev_barred.has_value()) {
          CORO_EARLY_RETURN(cu_cp_cell_command_response{false});
        }
        // Whether the cell is currently active: a dormant cell transmits no MIB to bar, so only the intent is
        // recorded (it is re-applied when the cell is activated). Checked when the task runs, so a deactivation
        // queued before this command is accounted for.
        if (du_db.find_du(cgi) == cu_cp_du_index_t::invalid) {
          logger.info("Cell nci={:#x} is deactivated. Barred intent ({}) recorded and applied on activation",
                      cgi.nci.value(),
                      barred);
          CORO_EARLY_RETURN(cu_cp_cell_command_response{true});
        }
        CORO_AWAIT_VALUE(success, launch_async<cell_barring_routine>(cfg, std::move(targets), barred, du_db, logger));
        if (!success) {
          // Restore the previous intent, so recorded intent stays consistent with the reported outcome.
          logical_cells.set_barred(cgi.nci, *prev_barred);
        }
        CORO_RETURN(cu_cp_cell_command_response{success});
      });
}

bool logical_cell_controller::dispatch_cell_command(const char* name, std::function<bool()> validate_and_schedule)
{
  return dispatch_bounded<bool>(*cfg.services.cu_cp_executor, logger, name, std::move(validate_and_schedule))
      .value_or(false);
}

std::optional<cu_cp_cell_state> logical_cell_controller::dispatch_get_cell_state(const nr_cell_global_id_t& cgi)
{
  std::optional<std::optional<cu_cp_cell_state>> state = dispatch_bounded<std::optional<cu_cp_cell_state>>(
      *cfg.services.cu_cp_executor, logger, "get_cell_state", [this, cgi]() { return get_cell_state(cgi); });
  return state.has_value() ? *state : std::nullopt;
}

bool logical_cell_controller::dispatch_deactivate_cell(const nr_cell_global_id_t& cgi)
{
  return dispatch_cell_command("deactivate_cell", [this, cgi]() {
    // Pre-validate the CGI synchronously so the WS/O1 handler can surface an immediate error to the caller
    // without waiting for the async procedure. Strict served-cells lookup, mirroring deactivate_cell(): a
    // cell that is already deactivated cannot be deactivated again.
    if (du_db.find_du(cgi) == cu_cp_du_index_t::invalid) {
      logger.warning("Dispatch deactivate_cell rejected. Cause: No DU found serving NR-CGI plmn={} nci={:#x}",
                     cgi.plmn_id.to_string(),
                     cgi.nci.value());
      return false;
    }

    async_task<cu_cp_cell_command_response> task = deactivate_cell(cgi);
    return common_task_sched.schedule(launch_async([t = std::move(task)](coro_context<async_task<void>>& ctx) mutable {
      CORO_BEGIN(ctx);
      CORO_AWAIT(t);
      CORO_RETURN();
    }));
  });
}

bool logical_cell_controller::dispatch_activate_cell(const nr_cell_global_id_t& cgi)
{
  return dispatch_cell_command("activate_cell", [this, cgi]() {
    // Any-state lookup, mirroring activate_cell(): the cell to activate is currently in the DU's deactivated
    // list, which the strict served-cells lookup would miss.
    if (du_db.find_du_any_state(cgi) == cu_cp_du_index_t::invalid) {
      logger.warning("Dispatch activate_cell rejected. Cause: No DU found serving NR-CGI plmn={} nci={:#x}",
                     cgi.plmn_id.to_string(),
                     cgi.nci.value());
      return false;
    }

    async_task<cu_cp_cell_command_response> task = activate_cell(cgi);
    return common_task_sched.schedule(launch_async([t = std::move(task)](coro_context<async_task<void>>& ctx) mutable {
      CORO_BEGIN(ctx);
      CORO_AWAIT(t);
      CORO_RETURN();
    }));
  });
}

bool logical_cell_controller::dispatch_bar_cell(const nr_cell_global_id_t& cgi, bool barred)
{
  return dispatch_cell_command("bar_cell", [this, cgi, barred]() {
    // Any-state lookup, mirroring bar_cell(): barred intent can also be recorded for a deactivated cell.
    if (du_db.find_du_any_state(cgi) == cu_cp_du_index_t::invalid) {
      logger.warning("Dispatch bar_cell rejected. Cause: No DU found serving NR-CGI plmn={} nci={:#x}",
                     cgi.plmn_id.to_string(),
                     cgi.nci.value());
      return false;
    }

    async_task<cu_cp_cell_command_response> task = bar_cell(cgi, barred);
    return common_task_sched.schedule(launch_async([t = std::move(task)](coro_context<async_task<void>>& ctx) mutable {
      CORO_BEGIN(ctx);
      CORO_AWAIT(t);
      CORO_RETURN();
    }));
  });
}

std::optional<cu_cp_cell_state> logical_cell_controller::get_cell_state(const nr_cell_global_id_t& cgi) const
{
  const logical_cell* cell = logical_cells.find_cell(cgi.nci);
  if (cell == nullptr) {
    return std::nullopt;
  }
  return cu_cp_cell_state{cell->admin_state, cell->operational_state, cell->barred};
}

std::vector<nr_cell_identity> logical_cell_controller::handle_du_cells_reported(cu_cp_du_index_t             du_index,
                                                                                span<const du_reported_cell> cells)
{
  std::vector<nr_cell_identity> activate;
  activate.reserve(cells.size());
  std::vector<cell_lifecycle_target> cells_to_bar;
  for (const du_reported_cell& reported : cells) {
    const logical_cell& cell = logical_cells.realize_cell(reported.cgi.nci, du_index);
    if (cell.admin_state != cell_admin_state::unlocked) {
      logical_cells.set_operational_state(reported.cgi.nci, cell_operational_state::disabled);
      continue;
    }
    // The F1 Setup Response activates the cell at the DU.
    logical_cells.set_operational_state(reported.cgi.nci, cell_operational_state::enabled);
    activate.push_back(reported.cgi.nci);
    if (cell.barred) {
      cells_to_bar.push_back(cell_lifecycle_target{du_index, reported.cgi, reported.pci, {}});
    }
  }

  // Re-apply the barred intent to the activated cells. The task must not start inline: the FIFO task
  // scheduler begins executing a scheduled task immediately when idle, i.e. within the executor task that is
  // handling the F1 Setup Request. Re-posting to the back of the CU-CP executor makes the barring resume
  // only after that task has completed and the F1 Setup Response has been handed to the transport; in-order
  // F1AP delivery then guarantees the DU processes the setup before the bar-carrying configuration update.
  if (!cells_to_bar.empty()) {
    bool scheduled = common_task_sched.schedule(launch_async(
        [this, targets = std::move(cells_to_bar), cells_barred = false](coro_context<async_task<void>>& ctx) mutable {
          CORO_BEGIN(ctx);
          CORO_AWAIT(defer_on_blocking(*cfg.services.cu_cp_executor, *cfg.services.timers));
          CORO_AWAIT_VALUE(
              cells_barred,
              launch_async<cell_barring_routine>(cfg, std::move(targets), /* barred = */ true, du_db, logger));
          (void)cells_barred;
          CORO_RETURN();
        }));
    if (!scheduled) {
      logger.warning("du={}: Failed to schedule the barred-intent re-application after F1 setup. Cause: task "
                     "queue is full",
                     fmt::underlying(du_index));
    }
  }

  return activate;
}

void logical_cell_controller::handle_du_removed(cu_cp_du_index_t du_index)
{
  logical_cells.derealize_du_cells(du_index);
}
