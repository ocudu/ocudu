// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/cu_cp_paging.h"
#include "ocudu/ran/tai.h"
#include <chrono>

namespace ocudu::ocucp {

enum class cu_cp_rrc_inactive_transition_report_request : uint8_t {
  // Send RRC Inactive Transition Reports when the UE enters or leaves RRC_INACTIVE state.
  subsequent_state_transition_report = 0,
  // Send one single RRC Inactive Transition Report when UE transitions from RRC_INACTIVE to RRC_IDLE.
  single_rrc_connected_state_report,
  // Stop sending RRC Inactive Transition Reports.
  cancel_report
};

struct cu_cp_core_network_assist_info_for_inactive_ie_exts {
  std::optional<uint64_t>               extended_ue_idx_value;
  std::optional<cu_cp_paging_edrx_info> nr_paging_drx_info;
};

struct cu_cp_core_network_assist_info_for_inactive {
  uint64_t                            ue_id_idx_value = 0;
  std::optional<uint16_t>             ue_specific_drx;
  std::optional<std::chrono::seconds> periodic_registration_update_timer;
  // If mico_mode_ind is true, the NG-RAN node shall, if supported, consider that the registration area
  // for the UE is the full PLMN and ignore the TAI List for RRC Inactive IE, see 3GPP TS 38.413 section 8.3.1.2.
  bool                                                               mico_mode_ind = false;
  std::vector<tai_t>                                                 tai_list_for_inactive;
  std::optional<cu_cp_core_network_assist_info_for_inactive_ie_exts> ie_exts;
};

} // namespace ocudu::ocucp
