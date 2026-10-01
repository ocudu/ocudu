// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "du_metrics_aggregator_impl.h"
#include "ocudu/adt/format.h"
#include "ocudu/mac/mac_metrics.h"
#include "ocudu/scheduler/scheduler_metrics.h"
#include "ocudu/support/executors/execute_until_success.h"

using namespace ocudu;
using namespace odu;

// class du_manager_metrics_aggregator_impl

du_manager_metrics_aggregator_impl::du_manager_metrics_aggregator_impl(
    const du_manager_params::metrics_config_params& params_,
    task_executor&                                  du_mng_exec_,
    timer_manager&                                  timers_,
    f1ap_metrics_collector&                         f1ap_collector_) :
  params(params_),
  du_mng_exec(du_mng_exec_),
  timers(timers_),
  f1ap_collector(f1ap_collector_),
  du_mng_collector(params.proc_enabled)
{
  (void)du_mng_exec;
  (void)timers;

  if (params.du_metrics != nullptr) {
    if (params.f1ap_enabled) {
      next_report.f1ap.emplace();
    }
    if (params.mac_enabled or params.sched_enabled) {
      next_report.mac.emplace();
    }
    if (params.proc_enabled) {
      next_report.du_proc.emplace();
    }
  }
}

du_manager_metrics_aggregator_impl::~du_manager_metrics_aggregator_impl() = default;

void du_manager_metrics_aggregator_impl::aggregate_mac_metrics_report(const mac_metric_report& report)
{
  // In case the DU metrics notifier was specified, report the DU metrics.
  if (params.du_metrics != nullptr) {
    // Save MAC metrics.
    next_report.mac = report;

    trigger_report();
  }
}

void du_manager_metrics_aggregator_impl::trigger_report()
{
  next_report.start_time = std::chrono::steady_clock::now() - params.period;
  next_report.period     = params.period;
  next_report.version    = next_version++;

  if (next_report.du_proc.has_value()) {
    // Generate DU procedure report.
    du_mng_collector.collect_metric_report(*next_report.du_proc);
  }

  if (next_report.f1ap.has_value()) {
    // Generate F1AP metrics report.
    f1ap_collector.collect_metrics_report(*next_report.f1ap);
  }

  // TODO: Generate RLC report.

  // Forward new report.
  params.du_metrics->on_new_metric_report(next_report);

  // Reset report.
  // Note: We use clear() to keep the vector capacity.
  if (params.f1ap_enabled) {
    next_report.f1ap->ues.clear();
  }
  if (params.mac_enabled) {
    next_report.mac->dl.cells.clear();
  }
  if (params.sched_enabled) {
    next_report.mac->sched.cells.clear();
  }
}

void du_manager_metrics_aggregator_impl::handle_cell_start(du_cell_index_t cell_index) {}

void du_manager_metrics_aggregator_impl::handle_cell_stop(du_cell_index_t cell_index) {}
