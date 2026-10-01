// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ocudulog/logger.h"
#include "ocudu/ocudulog/ocudulog.h"

namespace ocudu {

/// Registers the NTN loggers in the logger service.
inline void register_ntn_loggers(const ocudulog::basic_levels& ntn_level)
{
  auto& ntn_logger = ocudulog::fetch_basic_logger("NTN", true);
  ntn_logger.set_level(ntn_level);
}

} // namespace ocudu
