// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "du_appconfig_validators.h"
#include "apps/helpers/f1u/f1u_appconfig_validator.h"
#include "apps/helpers/hal/hal_appconfig_validator.h"
#include "apps/helpers/logger/logger_appconfig_validator.h"

using namespace ocudu;

bool ocudu::validate_appconfig(const du_appconfig& config)
{
  if (!validate_logger_appconfig(config.log_cfg)) {
    return false;
  }

  if (!validate_hal_appconfig(config.hal_config)) {
    return false;
  }

  if (config.f1ap_cfg.cu_cp_addresses.empty()) {
    fmt::print("CU-CP F1-C address is mandatory\n");
    return false;
  }

  for (const auto& addr : config.f1ap_cfg.cu_cp_addresses) {
    if (addr.empty()) {
      fmt::print("CU-CP F1-C address cannot be empty\n");
      return false;
    }
  }

  if (!validate_f1u_sockets_appconfig(config.f1u_cfg.f1u_sockets)) {
    return false;
  }

  return true;
}
