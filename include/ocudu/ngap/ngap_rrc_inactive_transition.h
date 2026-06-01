// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/cu_cp_types.h"

namespace ocudu::ocucp {

struct ngap_rrc_inactive_transition_report {
  enum class ngap_rrc_state { inactive = 0, connected };

  cu_cp_ue_index_t            ue_index = cu_cp_ue_index_t::invalid;
  ngap_rrc_state              rrc_state;
  cu_cp_user_location_info_nr user_location_info;
};

} // namespace ocudu::ocucp
