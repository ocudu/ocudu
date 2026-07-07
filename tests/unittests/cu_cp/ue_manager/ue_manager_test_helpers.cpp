// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ue_manager_test_helpers.h"
#include "ocudu/cu_cp/cu_cp_configuration_helpers.h"
#include "ocudu/ran/cu_cp_types.h"
#include <gtest/gtest.h>
#include <memory>

using namespace ocudu;
using namespace ocucp;

ue_manager_test::ue_manager_test() :
  cu_cp_cfg([this]() {
    cu_cp_configuration cucfg     = config_helpers::make_default_cu_cp_config();
    cucfg.services.timers         = &timers;
    cucfg.services.cu_cp_executor = &cu_worker;
    cucfg.admission.max_nof_dus   = 6;
    cucfg.admission.max_nof_ues   = cucfg.admission.max_nof_dus * ues_per_du;
    return cucfg;
  }()),
  ue_cfg([this]() {
    return ue_manager_config{.gnb_id              = cu_cp_cfg.node.gnb_id,
                             .max_nof_ues         = cu_cp_cfg.admission.max_nof_ues,
                             .drb_config          = cu_cp_cfg.bearers.drb_config,
                             .max_nof_drbs_per_ue = cu_cp_cfg.admission.max_nof_drbs_per_ue,
                             .int_algo_pref_list  = cu_cp_cfg.security.int_algo_pref_list,
                             .enc_algo_pref_list  = cu_cp_cfg.security.enc_algo_pref_list,
                             .enable_rrc_metrics  = cu_cp_cfg.metrics.layers_cfg.enable_rrc_metrics,
                             .ue                  = cu_cp_cfg.ue};
  }()),
  ue_dependencies([this]() {
    return ue_manager_dependencies{
        .timers = timers, .cu_cp_executor = cu_worker, .logger = ocudulog::fetch_basic_logger("CU-CP")};
  }())
{
  test_logger.set_level(ocudulog::basic_levels::debug);
  ue_mng_logger.set_level(ocudulog::basic_levels::debug);
  ocudulog::init();
}

ue_manager_test::~ue_manager_test()
{
  // flush logger after each test
  ocudulog::flush();
}
