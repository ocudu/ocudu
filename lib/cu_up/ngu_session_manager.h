// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/gtpu/gtpu_gateway.h"

namespace ocudu::ocuup {

class ngu_session_manager
{
public:
  virtual ~ngu_session_manager()                       = default;
  virtual gtpu_tnl_pdu_session& get_next_ngu_gateway() = 0;
};

} // namespace ocudu::ocuup
