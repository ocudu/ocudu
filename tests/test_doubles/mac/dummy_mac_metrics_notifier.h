// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/mac/mac_metrics.h"
#include "ocudu/mac/mac_metrics_notifier.h"

namespace ocudu {

class dummy_mac_metrics_notifier : public mac_metrics_notifier
{
public:
  std::optional<mac_metric_report> last_report;

  void on_new_metrics_report(const mac_metric_report& report) override { last_report = report; }
};

} // namespace ocudu
