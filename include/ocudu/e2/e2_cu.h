// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/e2/e2.h"
#include "ocudu/f1u/cu_up/f1u_metrics.h"
#include "ocudu/pdcp/pdcp_metrics.h"

namespace ocudu {

struct cu_cp_metrics_report;

class e2_cu_metrics_notifier : public pdcp_metrics_notifier, public ocuup::f1u_metrics_notifier
{
public:
  virtual ~e2_cu_metrics_notifier() = default;

  virtual void report_metrics(const cu_cp_metrics_report& metrics) = 0;

  using ocuup::f1u_metrics_notifier::report_metrics;
  using pdcp_metrics_notifier::report_metrics;
};

class e2_cu_metrics_interface
{
public:
  virtual ~e2_cu_metrics_interface() = default;

  /// \brief connects e2_cu_metric_provider
  /// \param[in] meas_provider pointer to the e2_cu_metric_provider
  virtual void connect_e2_cu_meas_provider(std::unique_ptr<e2_cu_metrics_notifier> meas_provider) = 0;
};

} // namespace ocudu
