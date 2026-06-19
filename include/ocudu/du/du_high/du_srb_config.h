// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/mac/mac_lc_config.h"
#include "ocudu/rlc/rlc_config.h"
#include <optional>

namespace ocudu {
namespace odu {

/// \brief SRB Configuration, i.e. associated RLC and MAC configuration for SRBs in the DU
struct du_srb_config {
  rlc_config rlc;
  /// If set, a proactive UL grant is triggered in reaction to each DL allocation on this SRB. Currently honored for
  /// SRB1.
  std::optional<mac_lc_config::triggered_ul_grant_cfg> triggered_ul_grant;
};

} // namespace odu
} // namespace ocudu
