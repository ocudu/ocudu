// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "split6_flexible_o_du_low_metrics_consumers.h"
#include "split6_flexible_o_du_low_app_service_metrics.h"
#include "split6_flexible_o_du_low_metrics.h"

using namespace ocudu;

void split6_flexible_o_du_low_metrics_consumer_json::handle_metric(const app_services::metrics_set& metric)
{
  const split6_flexible_o_du_low_metrics& odu_metrics =
      static_cast<const split6_flexible_o_du_low_app_service_metrics_impl&>(metric).get_metrics();

  odu_low_metrics_handler.handle_metric(odu_metrics.du_low);
  ru_metrics_handler.handle_metric(odu_metrics.ru);
}

void split6_flexible_o_du_low_metrics_consumer_log::handle_metric(const app_services::metrics_set& metric)
{
  const split6_flexible_o_du_low_metrics& odu_metrics =
      static_cast<const split6_flexible_o_du_low_app_service_metrics_impl&>(metric).get_metrics();

  odu_low_metrics_handler.handle_metric(odu_metrics.du_low);
  ru_metrics_handler.handle_metric(odu_metrics.ru);
}
