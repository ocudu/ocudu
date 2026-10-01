// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "apps/services/metrics/metrics_notifier.h"
#include "apps/services/metrics/metrics_producer.h"
#include "ocudu/f1u/cu_up/f1u_metrics.h"

namespace ocudu {

/// CU-UP F1-U metrics producer implementation.
class cu_up_f1u_metrics_producer_impl : public ocuup::f1u_metrics_notifier, public app_services::metrics_producer
{
public:
  explicit cu_up_f1u_metrics_producer_impl(app_services::metrics_notifier& notifier_) : notifier(notifier_) {}

  // See interface for documentation.
  void report_metrics(const ocuup::f1u_metrics_container& metrics) override;

  // See interface for documentation.
  void on_new_report_period() override {}

private:
  app_services::metrics_notifier& notifier;
};

} // namespace ocudu
