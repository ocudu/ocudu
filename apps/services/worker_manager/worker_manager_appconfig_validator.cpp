// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "worker_manager_appconfig_validator.h"
#include "worker_manager_appconfig.h"

using namespace ocudu;

bool ocudu::validate_expert_execution_appconfig(const expert_execution_appconfig& config)
{
  // Ensure the number of non-real time threads does not exceed the number of CPU cores
  auto&    cpu_desc  = cpu_architecture_info::get();
  uint32_t nof_cores = cpu_desc.get_host_nof_available_cpus();
  if (config.threads.main_pool.nof_threads and config.threads.main_pool.nof_threads.value() > nof_cores) {
    fmt::print("Invalid expert execution config: main_pool.nof_threads={} must not exceed nof_cores={}\n",
               config.threads.main_pool.nof_threads.value(),
               nof_cores);
    return false;
  }

  return true;
}
