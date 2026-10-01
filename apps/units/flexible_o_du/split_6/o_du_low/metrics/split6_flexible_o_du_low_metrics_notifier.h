// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

namespace ocudu {

struct split6_flexible_o_du_low_metrics;

/// Split 6 flexible O-DU low metrics notifier.
class split6_flexible_o_du_low_metrics_notifier
{
public:
  virtual ~split6_flexible_o_du_low_metrics_notifier() = default;

  /// Notifies new split 6 flexible O-DU low metrics.
  virtual void on_new_metrics(const split6_flexible_o_du_low_metrics& metrics) = 0;
};

} // namespace ocudu
