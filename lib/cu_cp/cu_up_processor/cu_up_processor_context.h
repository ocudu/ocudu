// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/cu_cp_types.h"
#include <string>

namespace ocudu::ocucp {

struct cu_up_processor_context {
  cu_cp_cu_up_index_t cu_up_index = cu_cp_cu_up_index_t::invalid; /// Index assisgned by CU-CP
  uint64_t            id;                                         /// the gNB-CU-UP-ID
  std::string         cu_up_name = "none";                        /// gNB-CU-UP-Name
  std::string         cu_cp_name = "none";                        /// gNB-CU-CP-Name
};

} // namespace ocudu::ocucp
