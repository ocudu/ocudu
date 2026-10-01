// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "app_execution_metrics.h"
#include "apps/services/metrics/metrics_notifier.h"
#include "apps/services/metrics/metrics_producer.h"
#include "ocudu/support/executors/metrics/executor_metrics_notifier.h"

namespace ocudu {

class app_execution_metrics_producer_impl : public app_services::metrics_producer, public executor_metrics_notifier
{
public:
  explicit app_execution_metrics_producer_impl(app_services::metrics_notifier<executor_metrics_impl>& notifier_) :
    notifier(notifier_)
  {
  }

  // See interface for documentation.
  void on_new_report_period() override {}

  // See interface for documentation.
  void on_new_metrics(const executor_metrics& metrics) override
  {
    notifier.on_new_metric(executor_metrics_impl(metrics));
  }

private:
  app_services::metrics_notifier<executor_metrics_impl>& notifier;
};

} // namespace ocudu
