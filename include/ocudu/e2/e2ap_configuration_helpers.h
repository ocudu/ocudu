// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/e2/e2ap_config.h"
#include "ocudu/ocudulog/ocudulog.h"

namespace ocudu {
namespace config_helpers {

/// Generates default cell configuration used by gNB DU. The default configuration should be valid.
inline e2ap_config make_default_e2ap_config()
{
  e2ap_config cfg{};
  cfg.gnb_id            = {411, 22};
  cfg.plmn              = "00101";
  cfg.max_setup_retries = 5;
  cfg.e2sm_kpm_enabled  = false;
  cfg.e2sm_rc_enabled   = false;
  cfg.e2sm_ccc_enabled  = false;
  return cfg;
}

/// Returns true if the given E2 configuration is valid, otherwise false.
inline bool is_valid_configuration(const e2ap_config& config)
{
  if (config.plmn.empty()) {
    fmt::print("PLMN id is empty\n");
    return false;
  }

  if (config.plmn.find("0x") != std::string::npos) {
    fmt::print("PLMN must not contain 0x\n");
    return false;
  }

  return true;
}

} // namespace config_helpers
} // namespace ocudu
