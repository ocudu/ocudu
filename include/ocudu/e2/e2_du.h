// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/rlc/rlc_metrics.h"
#include "ocudu/scheduler/scheduler_metrics.h"

namespace ocudu {

class e2_du_metrics_notifier : public scheduler_metrics_notifier, public rlc_metrics_notifier
{
public:
  virtual ~e2_du_metrics_notifier() = default;

  using rlc_metrics_notifier::report_metrics;
  using scheduler_metrics_notifier::report_metrics;
};

class e2_du_metrics_interface
{
public:
  virtual ~e2_du_metrics_interface() = default;

  /// \brief connects e2_du_metric_provider
  /// \param[in] meas_provider pointer to the e2_du_metric_provider
  virtual void connect_e2_du_meas_provider(std::unique_ptr<e2_du_metrics_notifier> meas_provider) = 0;
};
} // namespace ocudu
