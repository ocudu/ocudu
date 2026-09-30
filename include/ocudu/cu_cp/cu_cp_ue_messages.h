// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/cu_cp/up_context.h"
#include "ocudu/ran/meas_types.h"
#include "ocudu/security/security.h"

namespace ocudu::ocucp {

/// \brief RRC context transferred from one UE object to the other during mobility.
struct rrc_ue_transfer_context {
  security::security_context  sec_context;
  std::optional<rrc_meas_cfg> meas_cfg;
  up_context                  up_ctx;
  /// List of active SRBs (TODO: add PDCP config).
  static_vector<srb_id_t, MAX_NOF_SRBS> srbs;
  byte_buffer                           handover_preparation_info;
  byte_buffer                           ue_cap_rat_container_list;
  bool                                  is_inter_cu_handover = false;
};

} // namespace ocudu::ocucp
