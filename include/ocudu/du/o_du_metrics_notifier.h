// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

namespace ocudu {
namespace odu {

struct o_du_metrics;

/// O-RAN DU metrics notifier.
class o_du_metrics_notifier
{
public:
  virtual ~o_du_metrics_notifier() = default;

  /// Notifies new O-DU metrics.
  virtual void on_new_metrics(const o_du_metrics& metrics) = 0;
};

} // namespace odu
} // namespace ocudu
