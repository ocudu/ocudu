// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/scheduler/config/logical_channel_config.h"

namespace ocudu {
namespace config_helpers {

/// \brief Creates a default logical channel configuration to be used by the scheduler.
constexpr logical_channel_config create_default_logical_channel_config(lcid_t lcid)
{
  logical_channel_config lc_ch{};
  lc_ch.lcid = lcid;
  // See TS 38.331, 9.2.1 Default SRB configurations.
  lc_ch.lc_group                  = is_srb(lcid) ? SRB_LCG_ID : NON_GBR_DRB_LCG_ID;
  lc_ch.lc_sr_mask                = false;
  lc_ch.lc_sr_delay_timer_applied = false;
  return lc_ch;
}

} // namespace config_helpers
} // namespace ocudu
