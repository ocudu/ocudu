// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "du_low_appconfig_translators.h"
#include "apps/services/worker_manager/worker_manager_config.h"
#include "du_low_appconfig.h"

using namespace ocudu;
using namespace std::chrono_literals;

void ocudu::fill_du_low_worker_manager_config(worker_manager_config& config, const du_low_appconfig& app_cfg)
{
  // Current DU low application only supports one cell.
  config.config_affinities.resize(1);

  config.nof_main_pool_threads     = app_cfg.expert_execution_cfg.threads.main_pool.nof_threads;
  config.main_pool_task_queue_size = app_cfg.expert_execution_cfg.threads.main_pool.task_queue_size;
  config.main_pool_backoff_period =
      std::chrono::microseconds{app_cfg.expert_execution_cfg.threads.main_pool.backoff_period};
  config.main_pool_affinity_cfg = app_cfg.expert_execution_cfg.affinities.main_pool_cpu_cfg;
}
