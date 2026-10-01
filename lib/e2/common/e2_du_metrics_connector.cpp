// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/e2/e2_du_metrics_connector.h"

using namespace ocudu;

e2_du_metrics_connector::e2_du_metrics_connector() {}

void e2_du_metrics_connector::report_metrics(const scheduler_cell_metrics& metrics)
{
  if (e2_meas_provider) {
    // Pass metrics to the E2 Measurement Provider.
    e2_meas_provider->report_metrics(metrics);
  }
}

void e2_du_metrics_connector::report_metrics(const rlc_metrics& metrics)
{
  if (e2_meas_provider) {
    // Pass metrics to the E2 Measurement Provider.
    e2_meas_provider->report_metrics(metrics);
  }
}

void e2_du_metrics_connector::connect_e2_du_meas_provider(std::unique_ptr<e2_du_metrics_notifier> meas_provider)
{
  e2_meas_provider = std::move(meas_provider);
}
