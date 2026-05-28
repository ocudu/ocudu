// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "gnb_appconfig_validators.h"
#include "apps/helpers/logger/logger_appconfig_validator.h"
#include "apps/services/worker_manager/worker_manager_appconfig_validator.h"
#include "apps/units/flexible_o_du/o_du_high/du_high/du_high_config.h"
#include "apps/units/o_cu_cp/cu_cp/cu_cp_unit_config.h"

using namespace ocudu;

static bool validate_hal_config(const std::optional<hal_appconfig>& config)
{
#ifdef DPDK_FOUND
  if (config && config->eal_args.empty()) {
    fmt::print("It is mandatory to fill the EAL configuration arguments to initialize DPDK correctly\n");
    return false;
  }
#endif
  return true;
}

bool ocudu::validate_appconfig(const gnb_appconfig& config)
{
  if (!validate_logger_appconfig(config.log_cfg)) {
    return false;
  }

  if (!validate_expert_execution_appconfig(config.expert_execution_cfg)) {
    return false;
  }

  if (!validate_hal_config(config.hal_config)) {
    return false;
  }

  return true;
}

bool ocudu::validate_plmn_and_tacs(const du_high_unit_config& du_hi_cfg, const cu_cp_unit_config& cu_cp_cfg)
{
  if (cu_cp_cfg.amf_config.no_core) {
    return true;
  }

  std::vector<cu_cp_unit_supported_ta_item> supported_tas;

  supported_tas.reserve(cu_cp_cfg.amf_config.amf.supported_tas.size());
  for (const auto& supported_ta : cu_cp_cfg.amf_config.amf.supported_tas) {
    supported_tas.push_back(supported_ta);
  }

  for (const auto& amf : cu_cp_cfg.extra_amfs) {
    for (const auto& supported_ta : amf.supported_tas) {
      supported_tas.push_back(supported_ta);
    }
  }

  for (const auto& cell : du_hi_cfg.cells_cfg) {
    bool found = false;
    for (const auto& supported_ta : supported_tas) {
      for (const auto& plmn_item : supported_ta.plmn_list) {
        if (plmn_item.plmn_id == cell.cell.plmn && supported_ta.tac == cell.cell.tac) {
          found = true;
          break;
        }
      }
    }

    if (!found) {
      fmt::print("Could not find cell PLMN '{}' and cell TAC '{}' in the CU-CP supported tracking areas list\n",
                 cell.cell.plmn,
                 cell.cell.tac);
      return false;
    }
  }

  return true;
}
