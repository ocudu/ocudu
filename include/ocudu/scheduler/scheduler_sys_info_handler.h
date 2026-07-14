// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/du_types.h"
#include "ocudu/scheduler/config/si_scheduling_config.h"

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

/// \brief Requests the scheduler to broadcast a PWS (ETWS/CMAS) short-message notification and activate the target
/// SI-message for one complete broadcast (i.e. \c nof_segments consecutive SI-message window transmissions).
///
/// Repetition (TS 38.473, Section 8.5.1 "Repetition Period"/"Number of Broadcasts Requested") is entirely handled by
/// the MAC layer, which re-issues this request once per broadcast occurrence.
struct pws_broadcast_request {
  /// Cell index specific to this PWS broadcast indication.
  du_cell_index_t cell_index;
  /// Index of the SI-message carrying the SIB6/7/8 to activate.
  unsigned si_msg_idx;
  /// Number of segments composing the warning message, i.e. the number of consecutive SI-message window
  /// transmissions needed to complete one broadcast.
  unsigned nof_segments;
};

/// Interface used to notify new SIB1 or SI message updates to the scheduler.
class scheduler_sys_info_handler
{
public:
  virtual ~scheduler_sys_info_handler() = default;

  /// Handle cell system information scheduling update.
  virtual void handle_si_update_request(const si_scheduling_update_request& req) = 0;

  /// Handle a PWS (Write-Replace Warning) broadcast indication for one complete broadcast.
  virtual void handle_pws_broadcast_indication(const pws_broadcast_request& req) = 0;
};

} // namespace ocudu
