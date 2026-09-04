// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "cu_up_appconfig_validator.h"
#include "apps/helpers/f1u/f1u_appconfig_validator.h"
#include "apps/helpers/logger/logger_appconfig_validator.h"
#include "apps/services/worker_manager/worker_manager_appconfig_validator.h"
#include "cu_up_appconfig.h"

using namespace ocudu;

bool ocudu::validate_cu_up_appconfig(const cu_up_appconfig& config)
{
  if (!validate_logger_appconfig(config.log_cfg)) {
    return false;
  }

  if (!validate_expert_execution_appconfig(config.expert_execution_cfg)) {
    return false;
  }

  if (!validate_f1u_sockets_appconfig(config.f1u_cfg)) {
    return false;
  }

  return true;
}
