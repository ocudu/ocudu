// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/du_types.h"
#include "ocudu/scheduler/config/si_scheduling_config.h"
#include <optional>

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

/// SI message of an ETWS/CMAS SI epoch that is broadcasting a warning.
struct pws_broadcasting_si_message {
  /// SIBs carried by the SI message.
  sib_type_set sib_set;
  /// \brief Number of segments composing the warning message, i.e. the number of consecutive SI-message window
  /// transmissions needed to complete one broadcast.
  ///
  /// \c std::nullopt means broadcast indefinitely, used for test_mode-configured content.
  std::optional<unsigned> nof_segments;
  /// \brief Length, in bytes, of the largest segment of the warning message.
  ///
  /// Real Write-Replace Warning content (and its segmentation) is only known once it is pushed, so it does not come
  /// from the static SI scheduling configuration.
  units::bytes msg_len;
  /// \brief SI epoch version at which the broadcast of this warning was (re)started.
  ///
  /// It matches the version of the epoch carrying it when that epoch was triggered by this warning, and is lower when
  /// the epoch was triggered by something else, in which case this broadcast must not be extended.
  si_version_type version = 0;
};

struct pws_si_scheduling_update_request {
  /// Cell index specific to the update of the SI scheduling.
  du_cell_index_t cell_index;
  /// SI epoch counter, drawn from the same space as the normal operation one.
  si_version_type version;
  /// Configuration of SI scheduling, including SIB1 payload length and SI messages.
  si_scheduling_config si_sched_cfg;
  /// \brief SI messages that are broadcasting a warning.
  ///
  /// A warning whose version matches \c version starts one more broadcast, which reissues the etwsAndCmasIndication
  /// short message. Repetition (TS 38.473, Section 8.5.1 "Repetition Period"/"Number of Broadcasts Requested") is
  /// entirely handled by the MAC layer, which stamps a warning with the version of the epoch it triggers, once per
  /// broadcast occurrence.
  static_vector<pws_broadcasting_si_message, MAX_PWS_SI_MESSAGES> broadcasting;
};

/// Interface used to notify new SIB1 or SI message updates to the scheduler.
class scheduler_sys_info_handler
{
public:
  virtual ~scheduler_sys_info_handler() = default;

  /// Handle cell system information scheduling update.
  virtual void handle_si_update_request(const si_scheduling_update_request& req) = 0;

  /// Handle an update of the System Information broadcast while a warning is on air.
  virtual void handle_pws_si_update_request(const pws_si_scheduling_update_request& req) = 0;
};

} // namespace ocudu
