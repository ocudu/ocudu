// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/du_types.h"
#include "ocudu/scheduler/config/si_scheduling_config.h"
#include <chrono>

namespace ocudu {

/// Identifier for the version of the system information scheduling information.
using si_version_type = unsigned;

/// Information relative to the update of a cell SIB1 or SI messages.
struct si_scheduling_update_request {
  /// Cell index specific to the update of the SI scheduling.
  du_cell_index_t cell_index;
  /// SI epoch counter, monotonically increasing with each update.
  si_version_type version;
  /// Configuration of SI scheduling, including SIB1 payload length and SI messages.
  si_scheduling_config si_sched_cfg;
};

/// \brief Requests the scheduler to repeat a PWS (ETWS/CMAS) short-message broadcast indication a given number of
/// times at a given cadence, as per TS 38.473, Section 8.5.1 "Repetition Period"/"Number of Broadcasts Requested".
/// A new request for the same cell fully replaces any in-flight repeat state.
struct pws_broadcast_request {
  /// Cell index specific to this PWS broadcast indication.
  du_cell_index_t cell_index;
  /// Repetition Period.
  std::chrono::seconds repeat_period;
  /// Number of Broadcasts Requested.
  uint32_t nof_broadcasts_requested;
};

/// Interface used to notify new SIB1 or SI message updates to the scheduler.
class scheduler_sys_info_handler
{
public:
  virtual ~scheduler_sys_info_handler() = default;

  /// Handle cell system information scheduling update.
  virtual void handle_si_update_request(const si_scheduling_update_request& req) = 0;

  /// Handle a PWS (Write-Replace Warning) broadcast repeat/count indication.
  virtual void handle_pws_broadcast_indication(const pws_broadcast_request& req) = 0;
};

} // namespace ocudu
