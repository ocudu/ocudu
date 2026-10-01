// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "du_metrics_producer.h"
#include "du_metrics.h"

using namespace ocudu;

void du_metrics_producer_impl::on_new_metric_report(const odu::du_metrics_report& report)
{
  notifier.on_new_metric(du_metrics_impl(report));
}
