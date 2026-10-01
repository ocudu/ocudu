// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "split6_flexible_o_du_low_metrics_producer.h"
#include "split6_flexible_o_du_low_app_service_metrics.h"
#include "split6_flexible_o_du_low_metrics.h"

using namespace ocudu;

void split6_o_du_low_metrics_producer_impl::on_new_metrics(const split6_flexible_o_du_low_metrics& metrics)
{
  notifier.on_new_metric(split6_flexible_o_du_low_app_service_metrics_impl(metrics));
}
