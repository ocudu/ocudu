// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "du_low_appconfig_validators.h"
#include "apps/helpers/hal/hal_appconfig_validator.h"
#include "apps/helpers/logger/logger_appconfig_validator.h"
#include "du_low_appconfig.h"

using namespace ocudu;

bool ocudu::validate_du_low_appconfig(const du_low_appconfig& config)
{
  if (!validate_logger_appconfig(config.log_cfg)) {
    return false;
  }

  if (!validate_hal_appconfig(config.hal_config)) {
    return false;
  }

  return true;
}
