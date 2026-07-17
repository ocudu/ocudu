// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ocudulog/logger.h"
#include "ocudu/ran/cu_cp_types.h"
#include "ocudu/support/async/fifo_async_task_scheduler.h"
#include "ocudu/support/executors/task_executor.h"
#include "ocudu/support/timers.h"
#include <map>

namespace ocudu::ocucp {

/// NGAP task scheduler configuration.
struct ngap_task_scheduler_config {
  uint16_t max_nof_amfs;
};

/// NGAP task scheduler dependencies.
struct ngap_task_scheduler_dependencies {
  timer_manager&          timers;
  task_executor&          exec;
  ocudulog::basic_logger& logger;
};

/// \brief Service provided by CU-CP to schedule async tasks for a given AMF.
class ngap_task_scheduler
{
public:
  ngap_task_scheduler(const ngap_task_scheduler_config& cfg, const ngap_task_scheduler_dependencies& dependencies);
  ~ngap_task_scheduler() = default;

  /// CU-UP task scheduler.
  bool handle_amf_async_task(cu_cp_amf_index_t amf_index, async_task<void>&& task);

  /// Makes a unique timer and returns it.
  unique_timer make_unique_timer() const;

  /// Gets the timer manager.
  timer_manager& get_timer_manager() const;

private:
  timer_manager&          timers;
  task_executor&          exec;
  ocudulog::basic_logger& logger;

  // Task event loops indexed by amf_index.
  std::map<cu_cp_amf_index_t, fifo_async_task_scheduler> amf_ctrl_loop;
};

} // namespace ocudu::ocucp
