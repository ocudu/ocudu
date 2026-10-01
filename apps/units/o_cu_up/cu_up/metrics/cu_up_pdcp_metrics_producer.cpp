// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "cu_up_pdcp_metrics_producer.h"
#include "cu_up_pdcp_metrics.h"

using namespace ocudu;

void cu_up_pdcp_metrics_producer_impl::report_metrics(const pdcp_metrics_container& report)
{
  notifier.on_new_metric(cu_up_pdcp_metrics_impl(report));
}
