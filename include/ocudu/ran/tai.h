// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/plmn_identity.h"
#include "ocudu/ran/tac.h"

namespace ocudu {

/// 3GPP TS 38.413 section 9.3.3.11.
struct tai_t {
  plmn_identity plmn_id = plmn_identity::test_value();
  tac_t         tac;
};

} // namespace ocudu
