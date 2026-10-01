// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ocudulog/logger.h"
#include "ocudu/ran/pci.h"

namespace ocudu {

struct sched_result;

class scheduler_result_logger
{
public:
  explicit scheduler_result_logger(bool log_broadcast_, pci_t pci_);

  /// Log scheduler result for a specific cell and slot.
  /// \param[in] result Scheduling result for this slot.
  /// \param[in] slot_latency Latency that it took for the scheduler to make the decision for the slot.
  void on_scheduler_result(const sched_result&       result,
                           std::chrono::microseconds slot_latency = std::chrono::microseconds{0});

private:
  void log_debug(const sched_result& result, std::chrono::microseconds slot_latency);

  void log_info(const sched_result& result, std::chrono::microseconds slot_latency);

  ocudulog::basic_logger& logger;
  bool                    log_broadcast;
  bool                    enabled;
  const pci_t             pci;

  fmt::memory_buffer fmtbuf;
};

} // namespace ocudu
