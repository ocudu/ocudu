// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/f1u/cu_up/f1u_config.h"
#include "ocudu/pdcp/pdcp_config.h"
#include "ocudu/ran/cu_types.h"
#include "ocudu/ran/cu_up_types.h"
#include "ocudu/ran/rb_id.h"
#include <cstdint>
#include <type_traits>
#include <vector>

namespace ocudu {
namespace ocuup {

struct e1ap_bearer_context_inactivity_notification {
  cu_up_ue_index_t              ue_index    = cu_up_ue_index_t::INVALID_CU_UP_UE_INDEX;
  bool                          ue_inactive = false;
  std::vector<drb_id_t>         inactive_drbs;
  std::vector<pdu_session_id_t> inactive_pdu_sessions;
};

/// QoS Configuration, i.e. 5QI and the associated SDAP, PDCP and F1-U configuration for DRBs.
struct cu_up_qos_config {
  /// Implementation-specific parameters for PDCP.
  pdcp_custom_config pdcp_custom_cfg;
  /// Implementation-specific parameters for F1-U.
  f1u_config f1u_cfg;
};

} // namespace ocuup
} // namespace ocudu
