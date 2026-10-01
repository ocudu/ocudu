// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "procedures/du_proc_context_view.h"
#include "ocudu/du/du_high/du_manager/du_manager.h"
#include "ocudu/support/synchronization/sync_event.h"

namespace ocudu {

class fifo_async_task_scheduler;

namespace ocudu_ntn {
class ntn_configuration_manager;
}

namespace odu {

class du_manager_controller_impl final : public du_manager_controller
{
public:
  du_manager_controller_impl(const du_proc_context_view&           proc_ctxt_,
                             fifo_async_task_scheduler&            main_task_sched_,
                             ocudu_ntn::ntn_configuration_manager* ntn_mng_);
  ~du_manager_controller_impl() override
  {
    // Stop if not stopped yet.
    stop();
  }

  void start() override;

  void stop() override;

private:
  /// Handle transition from operational to idle state.
  void handle_du_stop_request(scoped_sync_token tk);

  /// Main control loop of tasks in the DU-high.
  fifo_async_task_scheduler&  main_task_sched;
  const du_proc_context_view& proc_ctxt;
  /// NTN configuration manager of this DU, or nullptr when no NTN cell is configured. Owned by the DU manager and
  /// declared before this controller, so it stays alive for as long as this controller can stop it.
  ocudu_ntn::ntn_configuration_manager* ntn_mng;

  /// \brief State of the DU seen from the caller side (instead of the DU manager execution context).
  /// This flag is useful to avoid redundant start/stop tasks.
  bool running_guard_flag = false;
};

} // namespace odu
} // namespace ocudu
