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
  du_mng_collector(params.proc_enabled),
  idle_timer(timers.create_unique_timer(du_mng_exec))
{
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

void du_manager_metrics_aggregator_impl::start()
{
  if (params.du_metrics == nullptr) {
    return;
  }

  idle_timer.set(params.period, [this]() { handle_idle_timer_expiry(); });
  idle_timer.run();
}

void du_manager_metrics_aggregator_impl::stop()
{
  idle_timer.stop();
}

void du_manager_metrics_aggregator_impl::aggregate_mac_metrics_report(const mac_metric_report& report)
{
  // In case the DU metrics notifier was specified, report the DU metrics.
  if (params.du_metrics != nullptr) {
    // Save MAC metrics.
    next_report.mac                = report;
    nof_periods_without_mac_report = 0;

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
  if (params.mac_enabled and next_report.mac.has_value()) {
    next_report.mac->dl.cells.clear();
  }
  if (params.sched_enabled and next_report.mac.has_value()) {
    next_report.mac->sched.cells.clear();
  }
}

void du_manager_metrics_aggregator_impl::handle_idle_timer_expiry()
{
  // Report without MAC metrics if the MAC skipped a full period, so that a report is generated in every period.
  // Note: A single period is tolerated, as the MAC reports are not aligned with this timer.
  if (++nof_periods_without_mac_report > 1) {
    next_report.mac.reset();
    trigger_report();
  }

  idle_timer.run();
}

void du_manager_metrics_aggregator_impl::handle_cell_start(du_cell_index_t cell_index) {}

void du_manager_metrics_aggregator_impl::handle_cell_stop(du_cell_index_t cell_index) {}
