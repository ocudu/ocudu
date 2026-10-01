// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "apps/services/buffer_pool/buffer_pool_manager.h"
#include "apps/services/buffer_pool/metrics/buffer_pool_metrics.h"
#include "apps/services/metrics/metrics_notifier.h"
#include "apps/services/metrics/metrics_producer.h"

namespace ocudu {
namespace app_services {

/// Buffer pool metrics producer implementation.
class buffer_pool_metrics_producer_impl : public app_services::metrics_producer
{
public:
  explicit buffer_pool_metrics_producer_impl(app_services::metrics_notifier& notifier_) : notifier(notifier_) {}

  void on_new_report_period() override
  {
    size_t central_cache_size = buffer_pool_manager::get_central_cache_size();
    notifier.on_new_metric(buffer_pool_metrics_impl(central_cache_size));
  }

private:
  app_services::metrics_notifier& notifier;
};

} // namespace app_services
} // namespace ocudu
