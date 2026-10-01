// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "du_high_rlc_metrics_producer.h"
#include "du_high_rlc_metrics.h"

using namespace ocudu;

void rlc_metrics_producer_impl::report_metrics(const rlc_metrics& report)
{
  notifier.on_new_metric(rlc_metrics_impl(report));
}
