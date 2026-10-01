// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "realtime_timing_worker.h"
#include "ocudu/ofh/timing/ofh_timing_manager.h"

namespace ocudu {
namespace ofh {

class timing_manager_impl : public timing_manager
{
  realtime_timing_worker worker;

public:
  timing_manager_impl(ocudulog::basic_logger& logger, task_executor& executor, const realtime_worker_cfg& config) :
    worker(logger, executor, config)
  {
  }

  // See interface for documentation.
  operation_controller& get_controller() override;

  // See interface for documentation.
  ota_symbol_boundary_notifier_manager& get_ota_symbol_boundary_notifier_manager() override;

  // See interface for documentation.
  timing_metrics_collector& get_metrics_collector() override;
};

} // namespace ofh
} // namespace ocudu
