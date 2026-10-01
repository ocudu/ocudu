// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/span.h"
#include <memory>

namespace ocudu {

class timer_manager;
class task_executor;
namespace odu {
class du_high_executor_mapper;
} // namespace odu

namespace test_helpers {

class du_high_worker_manager
{
public:
  virtual ~du_high_worker_manager() = default;

  virtual void stop() = 0;

  virtual odu::du_high_executor_mapper& get_exec_mapper() = 0;

  virtual task_executor& timer_executor() = 0;

  virtual void wait_pending_tasks() = 0;
};

struct du_high_worker_config {
  unsigned             nof_cell_workers   = 1;
  bool                 use_os_thread_prio = false;
  span<const unsigned> du_cell_cores;
  timer_manager&       timers;
};

/// Create DU-high executor mapper.
std::unique_ptr<du_high_worker_manager> create_multi_threaded_du_high_executor_mapper(const du_high_worker_config& cfg);

} // namespace test_helpers
} // namespace ocudu
