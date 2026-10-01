// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/du/du_high/du_metrics_report.h"
#include <utility>

namespace ocudu {

/// DU metrics.
class du_metrics_impl final
{
  odu::du_metrics_report report;

public:
  explicit du_metrics_impl(odu::du_metrics_report report_) : report(std::move(report_)) {}

  // Returns the DU metrics report.
  const odu::du_metrics_report& get_report() const { return report; }
};

} // namespace ocudu
