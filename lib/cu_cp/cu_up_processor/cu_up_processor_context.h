// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/cu_cp_types.h"

namespace ocudu::ocucp {

struct cu_up_processor_context {
  /// Index assigned by CU-CP.
  cu_cp_cu_up_index_t cu_up_index = cu_cp_cu_up_index_t::invalid;
  /// the gNB-CU-UP-ID.
  uint64_t id;
  /// gNB-CU-UP-Name.
  std::string cu_up_name = "none";
  /// gNB-CU-CP-Name.
  std::string cu_cp_name = "none";
};

} // namespace ocudu::ocucp
