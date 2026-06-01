// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/cu_cp/cu_cp_types.h"
#include "ocudu/ran/gnb_id.h"
#include "ocudu/ran/qos/five_qi.h"
#include "ocudu/rrc/rrc_ue_config.h"
#include "ocudu/security/security.h"
#include <map>

namespace ocudu::ocucp {

struct rrc_cfg_t {
  /// The gNodeB identifier to use for measurement configs.
  gnb_id_t gnb_id;
  /// PDCP config to use when UE SRB2 are configured.
  srb_pdcp_config srb2_cfg;
  /// Configuration for available 5QI.
  std::map<five_qi_t, cu_cp_qos_config> drb_config;
  /// Integrity protection algorithms preference list
  security::preferred_integrity_algorithms int_algo_pref_list{security::integrity_algorithm::nia0};
  /// Encryption algorithms preference list
  security::preferred_ciphering_algorithms enc_algo_pref_list{security::ciphering_algorithm::nea0};
  /// Force re-establishment fallback.
  bool force_reestablishment_fallback = false;
  /// Force resume fallback.
  bool force_resume_fallback = false;
  /// Guard time for RRC procedures.
  std::chrono::milliseconds rrc_procedure_guard_time_ms{500};
};

} // namespace ocudu::ocucp
