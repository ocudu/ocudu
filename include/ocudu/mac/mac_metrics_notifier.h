// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

namespace ocudu {

struct mac_metric_report;

/// Notifier used by MAC to report new metrics.
class mac_metrics_notifier
{
public:
  virtual ~mac_metrics_notifier() = default;

  /// \brief Called on every new MAC metrics report.
  /// \param report Metric report.
  virtual void on_new_metrics_report(const mac_metric_report& report) = 0;
};

} // namespace ocudu
