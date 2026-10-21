// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "du_procedure_metrics_collector.h"
#include "ocudu/du/du_high/du_manager/du_manager.h"
#include "ocudu/du/du_high/du_manager/du_manager_params.h"
#include "ocudu/du/du_high/du_metrics_notifier.h"
#include "ocudu/support/timers.h"

namespace ocudu {

class mac_metrics_notifier;
class scheduler_metrics_notifier;
struct scheduler_metrics_report;

namespace odu {

class du_manager_metrics_aggregator_impl final : public du_manager_mac_metric_aggregator
{
public:
  du_manager_metrics_aggregator_impl(const du_manager_params::metrics_config_params& params_,
                                     task_executor&                                  du_mng_exec_,
                                     timer_manager&                                  timers_,
                                     f1ap_metrics_collector&                         f1ap_collector_);
  ~du_manager_metrics_aggregator_impl() override;

  /// \brief Start reporting the DU metrics while the MAC is not reporting.
  ///
  /// The MAC only reports while cells are active, which is not the case while the DU is not connected to the CU-CP.
  void start();

  /// Stop the reports started with start().
  void stop();

  // DU metrics collector interface
  void aggregate_mac_metrics_report(const mac_metric_report& report) override;

  /// Get collector of DU control-plane procedure metrics.
  du_procedure_metrics_collector& get_proc_collector() { return du_mng_collector; }

  void handle_cell_start(du_cell_index_t cell_index);
  void handle_cell_stop(du_cell_index_t cell_index);

private:
  void trigger_report();

  void handle_idle_timer_expiry();

  const du_manager_params::metrics_config_params params;
  task_executor&                                 du_mng_exec;
  timer_manager&                                 timers;
  f1ap_metrics_collector&                        f1ap_collector;
  du_procedure_metrics_collector                 du_mng_collector;
  unique_timer                                   idle_timer;

  unsigned          next_version                   = 0;
  unsigned          nof_periods_without_mac_report = 0;
  du_metrics_report next_report{};
};

} // namespace odu
} // namespace ocudu
