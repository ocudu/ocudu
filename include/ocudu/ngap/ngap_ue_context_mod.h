// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ngap/ngap_types.h"
#include "ocudu/ran/cu_cp_types.h"
#include "ocudu/ran/guami.h"

namespace ocudu::ocucp {

struct ngap_ue_context_modification_request {
  cu_cp_ue_index_t                                          ue_index = cu_cp_ue_index_t::invalid;
  std::optional<aggregate_maximum_bit_rate_t>               ue_aggr_max_bit_rate;
  std::optional<ngap_core_network_assist_info_for_inactive> core_network_assist_info_for_inactive;
  std::optional<guami_t>                                    new_guami;
};

struct ngap_ue_context_modification_failure {
  cu_cp_ue_index_t ue_index = cu_cp_ue_index_t::invalid;
  ngap_cause_t     cause;
};

/// NGAP RRC states (3GPP TS 38.413 section 9.3.1.92).
enum class ngap_rrc_state { inactive = 0, connected };

struct ngap_ue_context_modification_response {
  cu_cp_ue_index_t                           ue_index = cu_cp_ue_index_t::invalid;
  std::optional<ngap_rrc_state>              rrc_state;
  std::optional<cu_cp_user_location_info_nr> user_location_info;
};

} // namespace ocudu::ocucp
