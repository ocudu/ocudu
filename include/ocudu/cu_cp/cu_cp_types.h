// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/pdcp/pdcp_config.h"
#include "ocudu/ran/cu_cp_types.h"

namespace ocudu::ocucp {

/// QoS Configuration, i.e. 5QI and the associated PDCP
/// and SDAP configuration for DRBs
struct cu_cp_qos_config {
  pdcp_config pdcp;
};

/// \brief Indication from a DU that a UE has successfully accessed a target cell (CHO execution).
struct cu_cp_access_success_indication {
  cu_cp_ue_index_t    ue_index        = cu_cp_ue_index_t::invalid; ///< Target UE index (sender of Access Success).
  cu_cp_ue_index_t    source_ue_index = cu_cp_ue_index_t::invalid; ///< Resolved CHO source UE index.
  nr_cell_global_id_t cgi;
};

} // namespace ocudu::ocucp
