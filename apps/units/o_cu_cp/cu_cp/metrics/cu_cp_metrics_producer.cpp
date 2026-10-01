// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "cu_cp_metrics_producer.h"
#include "cu_cp_metrics.h"
#include "ocudu/adt/format.h"

using namespace ocudu;

void cu_cp_metrics_producer_impl::notify_metrics_report_request(const cu_cp_metrics_report& report)
{
  notifier.on_new_metric(cu_cp_metrics_impl(report));
}
