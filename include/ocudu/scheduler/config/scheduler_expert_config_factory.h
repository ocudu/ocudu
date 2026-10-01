// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/scheduler/config/scheduler_expert_config.h"

namespace ocudu {
namespace config_helpers {

inline scheduler_expert_config make_default_scheduler_expert_config()
{
  scheduler_expert_config cfg;

  // The UE is not expected to decode a PDSCH scheduled with P-RNTI, RA-RNTI, SI-RNTI and Qm > 2 i.e. MCS index of 9 in
  // Table 5.1.3.1-1 of TS 38.214.
  // Note: A MCS index of 7 can handle Msg4 of size 458 bytes for PDSCH occupying symbols (2,..,14].
  // Note: A MCS index of 8 can handle Msg4 of size 325 bytes for PDSCH occupying symbols (3,..,14].
  cfg.ue.max_msg4_mcs        = 9;
  cfg.log_broadcast_messages = true;

  return cfg;
}

} // namespace config_helpers
} // namespace ocudu
