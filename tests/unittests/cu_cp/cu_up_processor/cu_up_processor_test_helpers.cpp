// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "cu_up_processor_test_helpers.h"
#include "lib/cu_cp/cu_up_processor/cu_up_processor_config.h"
#include "lib/cu_cp/cu_up_processor/cu_up_processor_factory.h"
#include "ocudu/cu_cp/cu_cp_configuration_helpers.h"

using namespace ocudu;
using namespace ocucp;

class dummy_task_sched final : public async_task_scheduler
{
public:
  bool schedule(async_task<void> task) override { return task_sched.schedule(std::move(task)); }

private:
  fifo_async_task_scheduler task_sched{32};
};

cu_up_processor_test::cu_up_processor_test() :
  cu_cp_cfg([this]() {
    cu_cp_configuration cucfg      = config_helpers::make_default_cu_cp_config();
    cucfg.services.timers          = &timers;
    cucfg.services.cu_cp_executor  = &ctrl_worker;
    cucfg.admission.max_nof_cu_ups = 4;
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
        .timers = timers, .cu_cp_executor = ctrl_worker, .logger = ocudulog::fetch_basic_logger("CU-CP")};
  }()),
  common_task_sched(std::make_unique<dummy_task_sched>())
{
  test_logger.set_level(ocudulog::basic_levels::debug);
  cu_cp_logger.set_level(ocudulog::basic_levels::debug);
  ocudulog::init();

  // create and start CU-UP processor
  cu_up_processor_config_t cu_up_cfg = {"ocucp", cu_cp_cu_up_index_t::min, cu_cp_cfg, cu_cp_logger};

  cu_up_processor_obj = create_cu_up_processor(std::move(cu_up_cfg), e1ap_notifier, cu_cp_notifier, *common_task_sched);
}

cu_up_processor_test::~cu_up_processor_test()
{
  // flush logger after each test
  ocudulog::flush();
}
