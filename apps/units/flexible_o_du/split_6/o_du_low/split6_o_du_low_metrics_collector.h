// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

namespace ocudu {

struct split6_flexible_o_du_low_metrics;

/// Split 6 O-DU low metrics collector.
class split6_o_du_low_metrics_collector
{
public:
  virtual ~split6_o_du_low_metrics_collector() = default;

  /// Collects the split 6 flexible O-DU low metrics in the given struct.
  virtual void collect_metrics(split6_flexible_o_du_low_metrics& metrics) = 0;
};

} // namespace ocudu
