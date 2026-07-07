// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/meas_types.h"
#include "ocudu/ran/pdcp/pdcp_t_reordering.h"

namespace ocudu::ocucp {

/// PDCP configuration for a SRB.
struct srb_pdcp_config {
  /// Value in ms of t-Reordering specified in TS 38.323.
  pdcp_t_reordering t_reordering = pdcp_t_reordering::infinity;
};

/// RRC UE configuration.
struct rrc_ue_cfg_t {
  /// PDCP configuration for SRB1.
  srb_pdcp_config                     srb1_pdcp_cfg;
  std::vector<rrc_meas_timing>        meas_timings;
  bool                                force_reestablishment_fallback = false;
  bool                                force_resume_fallback          = false;
  std::optional<std::chrono::seconds> rrc_reject_wait_time           = std::nullopt;
  /// \brief Guard time used for RRC message exchange with UE.
  std::chrono::milliseconds rrc_procedure_guard_time_ms{500};
};

} // namespace ocudu::ocucp
