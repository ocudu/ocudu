// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "apps/services/metrics/metrics_notifier.h"

namespace ocudu {
namespace app_services {

class metrics_set;

/// Metrics notifier proxy implementation.
class metrics_notifier_proxy_impl : public metrics_notifier
{
  metrics_notifier* proxy = nullptr;

public:
  // See interface for documentation.
  void on_new_metric(const metrics_set& metric) override
  {
    if (proxy) {
      proxy->on_new_metric(metric);
    }
  }

  void connect(metrics_notifier& notifier) { proxy = &notifier; }
};

} // namespace app_services
} // namespace ocudu
