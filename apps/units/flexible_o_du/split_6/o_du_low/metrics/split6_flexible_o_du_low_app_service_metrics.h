// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "split6_flexible_o_du_low_metrics.h"

namespace ocudu {

/// Application service metrics for the split 6 flexible O-DU low.
class split6_flexible_o_du_low_app_service_metrics_impl final
{
  split6_flexible_o_du_low_metrics metrics;

public:
  explicit split6_flexible_o_du_low_app_service_metrics_impl(const split6_flexible_o_du_low_metrics& metrics_) :
    metrics(metrics_)
  {
  }

  // Returns the flexible O-DU metrics.
  const split6_flexible_o_du_low_metrics& get_metrics() const { return metrics; }
};

} // namespace ocudu
