// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/rrm.h"
#include "ocudu/scheduler/config/scheduler_expert_config.h"

namespace ocudu {

/// Cell-specific Default RAN slice configuration.
struct slice_rrm_policy_config {
  /// Maximum RAN scheduling policy.
  static constexpr unsigned max_priority = 255;

  /// RRM Policy identifier.
  rrm_policy_member rrc_member;
  /// \brief Radio resources, in RBs, allocated to this group.
  ///
  /// These limits are computed based on the rrmPolicyRatios provided for the RRM policy and based on the number of RBs
  /// of the cell.
  rrm_policy_ratio_rb_limits rbs;
  /// RAN slice scheduling priority. Values: {0, ..., 255}.
  unsigned priority = 0;
  /// Policy scheduler configuration for the slice.
  scheduler_policy_config policy_sched_cfg = time_qos_scheduler_config{};
};

} // namespace ocudu
