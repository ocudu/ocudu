// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

namespace ocudu {
namespace ocuup {

struct f1u_metrics_container;

/// \brief Notifier interface used to report F1-U metrics.
class f1u_metrics_notifier
{
public:
  virtual ~f1u_metrics_notifier() = default;

  /// \brief This method will be called periodically to report the latest F1-U metrics statistics.
  virtual void report_metrics(const f1u_metrics_container& metrics) = 0;
};
} // namespace ocuup
} // namespace ocudu
