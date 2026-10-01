// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/cu_cp/cu_cp_executor_mapper.h"
#include "ocudu/adt/mpmc_queue.h"
#include "ocudu/support/executors/executor_decoration_factory.h"
#include "ocudu/support/executors/strand_executor.h"

using namespace ocudu;
using namespace ocucp;

namespace {

class cu_cp_executor_mapper_impl : public cu_cp_executor_mapper
{
  static constexpr uint32_t task_worker_queue_size = 2048;
  using strand_type = task_strand<task_executor*, concurrent_queue_policy::lockfree_mpmc>;

public:
  cu_cp_executor_mapper_impl(const strand_based_executor_config& config) :
    pool_exec(config.pool_executor),
    ctrl_strand(std::make_unique<strand_type>(&config.pool_executor, task_worker_queue_size))
  {
    // Decorate strand if needed.
    if (config.exec_metrics_channel_registry != nullptr) {
      execution_decoration_config cfg;
      cfg.metrics.emplace("cu_cp_exec", *config.exec_metrics_channel_registry, false);
      ctrl_strand = decorate_executor(std::move(ctrl_strand), cfg);
    }
  }

  task_executor& ctrl_executor() override { return *ctrl_strand; }

  task_executor& e2_executor() override
  {
    // Note: For now, we reuse the same strand for E2 tasks.
    return *ctrl_strand;
  }

  task_executor& n2_rx_executor() override { return pool_exec; }

  task_executor& f1c_rx_executor() override { return pool_exec; }

  task_executor& xnc_rx_executor() override { return pool_exec; }

  task_executor& e1_rx_executor() override { return pool_exec; }

  task_executor& e2_rx_executor() override { return pool_exec; }

private:
  task_executor& pool_exec;

  std::unique_ptr<task_executor> ctrl_strand;
};

} // namespace

std::unique_ptr<cu_cp_executor_mapper> ocucp::make_cu_cp_executor_mapper(const strand_based_executor_config& config)
{
  return std::make_unique<cu_cp_executor_mapper_impl>(config);
}
