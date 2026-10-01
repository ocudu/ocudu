// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "o_cu_up_e2_config_translators.h"
#include "apps/services/worker_manager/worker_manager_config.h"
#include "o_cu_up_e2_config.h"
#include "ocudu/e2/e2ap_configuration_helpers.h"

using namespace ocudu;

e2ap_config ocudu::generate_e2_config(const e2_config&              config,
                                      gnb_id_t                      gnb_id,
                                      const std::string&            plmn,
                                      std::optional<gnb_cu_up_id_t> gnb_cu_up_id)
{
  e2ap_config out_cfg      = ocudu::config_helpers::make_default_e2ap_config();
  out_cfg.gnb_id           = gnb_id;
  out_cfg.plmn             = plmn;
  out_cfg.gnb_cu_up_id     = gnb_cu_up_id;
  out_cfg.e2sm_kpm_enabled = config.e2sm_kpm_enabled;
  out_cfg.e2sm_rc_enabled  = config.e2sm_rc_enabled;
  out_cfg.e2sm_ccc_enabled = false;

  return out_cfg;
}

void ocudu::fill_o_cu_up_e2_worker_manager_config(worker_manager_config& config, const o_cu_up_e2_config& unit_cfg)
{
  auto& pcap_cfg = config.pcap_cfg;
  if (unit_cfg.pcaps.enabled) {
    pcap_cfg.is_e2ap_enabled = true;
  }
}
