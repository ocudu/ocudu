// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ocudulog/log_channel.h"
#include "ocudu/scheduler/scheduler_metrics.h"

namespace ocudu {

/// STDOUT consumer for the scheduler cell metrics.
class scheduler_cell_metrics_consumer_stdout
{
  /// Maximum number of metric lines in STDOUT without a header, ie, print header every
  /// MAX_NOF_STDOUT_METRIC_LINES_WITHOUT_HEADER lines.
  static constexpr unsigned MAX_NOF_STDOUT_METRIC_LINES_WITHOUT_HEADER = 10;

public:
  /// Handle scheduler metrics.
  void handle_metric(const scheduler_metrics_report& report);

  /// Prints the header in the next metric handle.
  void print_header() { nof_lines = MAX_NOF_STDOUT_METRIC_LINES_WITHOUT_HEADER; }

private:
  unsigned nof_lines = MAX_NOF_STDOUT_METRIC_LINES_WITHOUT_HEADER;
};

/// Logger consumer for the scheduler cell metrics.
class scheduler_cell_metrics_consumer_log
{
public:
  explicit scheduler_cell_metrics_consumer_log(ocudulog::log_channel& log_chan_) : log_chan(log_chan_)
  {
    ocudu_assert(log_chan.enabled(), "Logger log channel is not enabled");
  }

  /// Handle scheduler metrics.
  void handle_metric(const scheduler_metrics_report& report);

private:
  ocudulog::log_channel& log_chan;
};

/// E2 consumer for the scheduler cell metrics.
class scheduler_cell_metrics_consumer_e2
{
public:
  explicit scheduler_cell_metrics_consumer_e2(scheduler_metrics_notifier& notifier_) : notifier(notifier_) {}

  /// Handle scheduler metrics.
  void handle_metric(const scheduler_metrics_report& report);

private:
  scheduler_metrics_notifier& notifier;
};

} // namespace ocudu
