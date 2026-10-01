// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "logger_appconfig_validator.h"
#include "logger_appconfig.h"

using namespace ocudu;

bool ocudu::validate_logger_appconfig(const logger_appconfig& config)
{
  return !config.filename.empty();
}
