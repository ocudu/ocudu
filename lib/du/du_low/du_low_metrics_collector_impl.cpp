// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "du_low_metrics_collector_impl.h"
#include "ocudu/du/du_low/du_low_metrics.h"

using namespace ocudu;
using namespace odu;

void du_low_metrics_collector_impl::collect_metrics(du_low_metrics& metrics)
{
  if (upper_phy_collector == nullptr) {
    return;
  }

  upper_phy_collector->collect_metrics(metrics.upper_metrics);
}
