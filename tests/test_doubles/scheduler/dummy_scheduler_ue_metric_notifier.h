// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/scheduler/scheduler_metrics.h"

namespace ocudu {

/// Sink for scheduler UE metrics.
class dummy_scheduler_ue_metrics_notifier : public scheduler_metrics_notifier
{
public:
  scheduler_cell_metrics last_metrics;

  void report_metrics(const scheduler_cell_metrics& metrics) override { last_metrics = metrics; }
};

class dummy_scheduler_cell_metrics_notifier : public scheduler_cell_metrics_notifier
{
public:
  const std::optional<scheduler_cell_metrics>& last() const { return last_metrics; }

  void request_metrics() { metrics_requested = true; }

  scheduler_cell_metrics& get_next() override { return current_metrics; }

  void commit(scheduler_cell_metrics& ptr) override
  {
    ocudu_assert(&ptr == &current_metrics, "Invalid reference passed");
    last_metrics = current_metrics;
    current_metrics.ue_metrics.clear();
    current_metrics.events.clear();
  }

  bool is_sched_report_required(slot_point_extended sl_tx) const override
  {
    bool ret          = metrics_requested;
    metrics_requested = false;
    return ret;
  }

private:
  scheduler_cell_metrics                current_metrics{};
  std::optional<scheduler_cell_metrics> last_metrics;
  mutable bool                          metrics_requested = false;
};

} // namespace ocudu
