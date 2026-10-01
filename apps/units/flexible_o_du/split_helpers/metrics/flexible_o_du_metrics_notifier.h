// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

namespace ocudu {

struct flexible_o_du_metrics;

/// Flexible O-DU metrics notifier.
class flexible_o_du_metrics_notifier
{
public:
  virtual ~flexible_o_du_metrics_notifier() = default;

  /// Notifies a new flexible O-DU metric.
  virtual void on_new_metrics(const flexible_o_du_metrics& metric) = 0;
};

} // namespace ocudu
