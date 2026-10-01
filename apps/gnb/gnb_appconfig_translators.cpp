// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "gnb_appconfig_translators.h"
#include "apps/services/worker_manager/worker_manager_config.h"
#include "gnb_appconfig.h"

using namespace ocudu;
using namespace std::chrono_literals;

void ocudu::fill_gnb_worker_manager_config(worker_manager_config& config, const gnb_appconfig& app_cfg)
{
  ocudu_assert(config.cu_up_cfg, "CU-UP worker config does not exist");
  ocudu_assert(config.du_hi_cfg, "DU high worker config does not exist");

  config.nof_main_pool_threads     = app_cfg.expert_execution_cfg.threads.main_pool.nof_threads;
  config.main_pool_task_queue_size = app_cfg.expert_execution_cfg.threads.main_pool.task_queue_size;
  config.main_pool_backoff_period =
      std::chrono::microseconds{app_cfg.expert_execution_cfg.threads.main_pool.backoff_period};
  config.main_pool_affinity_cfg = app_cfg.expert_execution_cfg.affinities.main_pool_cpu_cfg;
}
