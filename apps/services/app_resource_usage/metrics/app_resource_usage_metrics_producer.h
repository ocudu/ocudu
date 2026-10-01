// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "apps/services/app_resource_usage/app_resource_usage.h"
#include "apps/services/metrics/metrics_notifier.h"
#include "apps/services/metrics/metrics_producer.h"

namespace ocudu {

/// Resource usage metrics producer implementation.
class resource_usage_metrics_producer_impl : public app_services::metrics_producer
{
public:
  resource_usage_metrics_producer_impl(app_services::metrics_notifier&   notifier_,
                                       app_services::app_resource_usage& app_resource_usage_) :
    notifier(notifier_), app_resource_usage(app_resource_usage_)
  {
  }

  void on_new_report_period() override
  {
    resource_usage_metrics new_metrics = app_resource_usage.get_new_metrics();
    notifier.on_new_metric(resource_usage_metrics_impl(new_metrics));
  }

private:
  app_services::metrics_notifier&   notifier;
  app_services::app_resource_usage& app_resource_usage;
};

} // namespace ocudu
