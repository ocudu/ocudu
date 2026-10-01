// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/f1ap/du/f1ap_du_metrics_report.h"

namespace ocudu {
namespace odu {

/// \brief Class used to collect F1AP metrics.
class f1ap_metrics_collector
{
public:
  virtual ~f1ap_metrics_collector() = default;

  /// \brief Generate an F1AP metrics report.
  /// \param report Report to fill.
  virtual void collect_metrics_report(f1ap_metrics_report& report) = 0;
};

} // namespace odu
} // namespace ocudu
