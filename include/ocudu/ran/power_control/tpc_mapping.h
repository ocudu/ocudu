// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/support/ocudu_assert.h"

namespace ocudu {

inline int tpc_mapping(unsigned tpc_command)
{
  switch (tpc_command) {
    case 0:
      return -1;
    case 1:
      return 0;
    case 2:
      return 1;
    case 3:
      return 3;
    default:
      ocudu_assertion_failure("Invalid TPC command");
      return 0;
  }
}

} // namespace ocudu
