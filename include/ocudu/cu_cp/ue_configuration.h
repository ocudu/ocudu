// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/i_rnti.h"

namespace ocudu::ocucp {

/// UE configuration passed to CU-CP.
struct ue_configuration {
  std::chrono::seconds inactivity_timer{7200};
  /// Timeout for requesting a PDU session in seconds, before the UE is released.
  std::chrono::seconds request_pdu_session_timeout = std::chrono::seconds{2};
  /// When set to false, UEs will not be set to RRC inactive.
  bool enable_rrc_inactive = false;
  /// RAN Paging cycle for RRC inactive UEs in number of radio frames.
  uint16_t ran_paging_cycle = 32;
  /// T380 timer value in minutes.
  std::chrono::minutes t380 = std::chrono::minutes{10};
  /// I-RNTI profile used to compose the Full-I-RNTI of a suspended UE (TS 38.300 table F-1).
  full_i_rnti_profile full_i_rnti_prof = full_i_rnti_profile::profile_0;
  /// I-RNTI profile used to compose the Short-I-RNTI of a suspended UE (TS 38.300 table F-2).
  short_i_rnti_profile short_i_rnti_prof = short_i_rnti_profile::profile_0;
};

} // namespace ocudu::ocucp
