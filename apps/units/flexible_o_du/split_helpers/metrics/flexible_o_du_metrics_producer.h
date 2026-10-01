// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "apps/services/metrics/metrics_notifier.h"
#include "apps/services/metrics/metrics_producer.h"
#include "flexible_o_du_app_service_metrics.h"
#include "flexible_o_du_metrics_notifier.h"

namespace ocudu {

/// O-RAN DU metrics producer implementation.
class o_du_metrics_producer_impl : public flexible_o_du_metrics_notifier, public app_services::metrics_producer
{
public:
  explicit o_du_metrics_producer_impl(app_services::metrics_notifier& notifier_) : notifier(notifier_) {}

  // See interface for documentation.
  void on_new_metrics(const flexible_o_du_metrics& metrics) override;

  // See interface for documentation.
  void on_new_report_period() override {}

private:
  app_services::metrics_notifier& notifier;
};

} // namespace ocudu
