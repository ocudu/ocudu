// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/cu_cp_types.h"
#include "ocudu/ran/rnti.h"
#include "ocudu/rrc/rrc_types.h"

namespace ocudu::ocucp {

struct rrc_resume_request {
  cu_cp_ue_index_t    ue_index = cu_cp_ue_index_t::invalid;
  nr_cell_global_id_t cgi;
  rnti_t              new_c_rnti;
  resume_cause_t      cause;
};

struct rrc_resume_request_response {
  bool                                   success = false;
  std::optional<rrc_radio_bearer_config> radio_bearer_cfg;
  byte_buffer                            master_cell_group;
  std::optional<rrc_meas_cfg>            meas_cfg;
  // Optional measurement gap config to include in RRC Resume.
  byte_buffer meas_gap_cfg;
};

} // namespace ocudu::ocucp
