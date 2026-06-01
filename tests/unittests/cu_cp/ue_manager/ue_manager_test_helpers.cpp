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
