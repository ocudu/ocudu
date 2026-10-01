// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/gnb_du_id.h"
#include <string>

namespace ocudu {
namespace ocucp {

/// Context of the DU that has setup an F1 connection to the CU-CP.
struct f1ap_du_context {
  gnb_du_id_t gnb_du_id = gnb_du_id_t::invalid;
  std::string gnb_du_name;
  uint8_t     gnb_du_rrc_version;
};

} // namespace ocucp
} // namespace ocudu
