// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/du/du_high/du_metrics_report.h"

namespace ocudu {
namespace odu {

/// \brief Interface used to push new DU metrics reports.
class du_metrics_notifier
{
public:
  virtual ~du_metrics_notifier() = default;

  /// \brief Called when a new DU metrics report is generated.
  /// \param report Generated report.
  virtual void on_new_metric_report(const du_metrics_report& report) = 0;
};

} // namespace odu
} // namespace ocudu
