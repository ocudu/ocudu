// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "du_cell_stop_procedure.h"
#include "../du_cell_manager.h"
#include "../du_ue/du_ue_manager.h"
#include "du_ue_reset_procedure.h"
#include "ocudu/adt/format.h"
#include "ocudu/support/async/async_no_op_task.h"
#include "ocudu/support/async/async_timer.h"
#include "ocudu/support/async/when_all.h"

using namespace ocudu;
using namespace odu;

du_cell_stop_procedure::du_cell_stop_procedure(du_ue_manager&           ue_mng_,
                                               du_cell_manager&         cell_mng_,
                                               const du_manager_params& du_params_,
                                               du_cell_index_t          cell_index_,
                                               ue_removal_mode          mode_) :
  ue_mng(ue_mng_),
  cell_mng(cell_mng_),
  du_params(du_params_),
  cell_index(cell_index_),
  mode(mode_),
  proc_logger(ocudulog::fetch_basic_logger("DU-MNG"), "DU Cell Stop"),
  timer(du_params.services.timers.create_unique_timer(du_params.services.du_mng_exec))
{
}

void du_cell_stop_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  proc_logger.log_proc_started();

  if (not cell_mng.is_cell_active(cell_index)) {
    // Cell is not active. Nothing to do.
    CORO_EARLY_RETURN();
  }

  if (mode == ue_removal_mode::trigger_f1_ue_release_request) {
    // Graceful drain: bar the cell and drain its UEs concurrently. Barring makes the live MIB advertise
    // cellBarred=true (TS 38.304) so idle UEs reselect away and released UEs do not re-camp here. Running the
    // bar-and-settle alongside the drain lets the barred MIB reach the air without adding latency to the stop;
    // the settling window is held before the cell is torn down, so the bar is transmitted before SSB stops.
    CORO_AWAIT(when_all(create_bar_and_drain_tasks()));
  } else {
    // F1 Reset and silent removal modes are reserved for fault and shutdown paths where graceful UE-side
    // handling is not required, so the cell is not barred.
    CORO_AWAIT(rem_ues_with_matching_pcell());
  }

  // Stop cell.
  CORO_AWAIT(cell_mng.stop(cell_index));

  // Wait for some slots to ensure all pending grants were flushed.
  // TODO: Move this wait to the MAC once the FAPI stop procedure is supported.
  static constexpr std::chrono::milliseconds mac_grant_flush_timeout{50};
  CORO_AWAIT(async_wait_for(timer, mac_grant_flush_timeout));

  proc_logger.log_proc_completed();

  CORO_RETURN();
}

std::vector<async_task<void>> du_cell_stop_procedure::create_bar_and_drain_tasks()
{
  std::vector<async_task<void>> tasks;
  // Two concurrent tasks: the bar-and-settle and the UE drain.
  tasks.reserve(2);
  tasks.push_back(cell_mng.set_cell_barred_and_wait(cell_index));
  tasks.push_back(rem_ues_with_matching_pcell());
  return tasks;
}

static void fill_ues_to_rem(std::vector<du_ue_index_t>& ues_to_rem, du_ue_manager& ue_mng, du_cell_index_t cell_index)
{
  auto& ue_db = ue_mng.get_du_ues();
  ues_to_rem.clear();
  ues_to_rem.reserve(ue_db.size());
  for (const auto& u : ue_db) {
    if (u.pcell_index == cell_index) {
      ues_to_rem.push_back(u.ue_index);
    }
  }
}

async_task<void> du_cell_stop_procedure::force_ue_removal()
{
  fill_ues_to_rem(ues_to_rem, ue_mng, cell_index);
  if (ues_to_rem.empty()) {
    // There are no UEs associated with this cell. Nothing to do.
    return launch_no_op_task();
  }
  std::optional<f1_reset_request::cause_type> cause;
  if (mode == ue_removal_mode::trigger_f1_reset) {
    cause = f1_reset_request::cause_type::cell_removal;
  }
  return launch_async<du_ue_reset_procedure>(ues_to_rem, ue_mng, du_params, cause);
}

async_task<void> du_cell_stop_procedure::rem_ues_with_matching_pcell()
{
  // Stop accepting new UEs in this cell.
  cell_mng.stop_accepting_ues(cell_index);

  if (mode == ue_removal_mode::trigger_f1_ue_release_request) {
    // Wait for UEs to be released by the CU.
    return await_cu_to_release_ues();
  }

  // Use F1 Reset procedure directly.
  return force_ue_removal();
}

async_task<void> du_cell_stop_procedure::await_cu_to_release_ues()
{
  // Implementation-defined timeout for graceful UE removal. After which, we will force UE removal.
  static constexpr unsigned nof_checks = cu_release_timeout / cu_release_check_period;

  return launch_async([this, count = 0U](coro_context<async_task<void>>& ctx) mutable {
    CORO_BEGIN(ctx);

    fill_ues_to_rem(ues_to_rem, ue_mng, cell_index);
    while (not ues_to_rem.empty()) {
      // There are UEs associated with this cell that need to be released.

      // > Request CU to release UEs via F1 UE CONTEXT RELEASE REQUEST.
      for (du_ue_index_t ue_idx : ues_to_rem) {
        du_params.f1ap.ue_mng.handle_ue_context_release_request(
            f1ap_ue_context_release_request{ue_idx, f1ap_ue_context_release_request::cause_type::other});
      }

      // > Wait for all UEs to be released.
      for (; count != nof_checks; ++count) {
        CORO_AWAIT(async_wait_for(timer, cu_release_check_period));

        bool ues_exist = false;
        for (du_ue_index_t ue_idx : ues_to_rem) {
          if (ue_mng.find_ue(ue_idx) != nullptr) {
            // There are still UEs connected to the cell.
            ues_exist = true;
            break;
          }
        }
        if (not ues_exist) {
          // All previously requested UEs to be released have been released. We will check if there are other UEs
          // that were created in the meantime and, if so, repeat the process.
          fill_ues_to_rem(ues_to_rem, ue_mng, cell_index);
          break;
        }
      }

      if (count == nof_checks) {
        // Timeout reached and there are still UEs attached to the cell. Force their removal.
        CORO_AWAIT(force_ue_removal());
      }
    }

    CORO_RETURN();
  });
}
