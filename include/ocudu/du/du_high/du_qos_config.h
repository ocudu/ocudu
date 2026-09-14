// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/f1u/du/f1u_config.h"
#include "ocudu/mac/mac_lc_config.h"
#include "ocudu/rlc/rlc_config.h"

namespace ocudu {
namespace odu {

/// \brief QoS Configuration, i.e. 5QI and the associated RLC configuration for DRBs
struct du_qos_config {
  rlc_config                                           rlc;
  f1u_config                                           f1u;
  std::optional<mac_lc_config::triggered_ul_grant_cfg> triggered_ul_grant;
  /// UL HARQ mode a grant must use for a 5QI logical channel to be multiplexed into it. Unrestricted when absent.
  std::optional<ul_harq_mode> allowed_harq_mode;
};

} // namespace odu
} // namespace ocudu
