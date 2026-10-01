// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/gnb_id.h"
#include "ocudu/ran/guami.h"
#include "ocudu/ran/supported_tracking_area.h"
#include <chrono>
#include <vector>

namespace ocudu::ocucp {

struct xnap_configuration {
  std::chrono::milliseconds            procedure_timeout{5000};
  std::chrono::milliseconds            reconnect_timer{10000};
  gnb_id_t                             gnb_id;
  std::vector<supported_tracking_area> tai_support_list;
  std::vector<guami_t>                 guami_list;
  /// When true, the CU-CP will not initiate outbound XNAP connections but will accept inbound ones.
  bool no_connection_init = false;
};

} // namespace ocudu::ocucp
