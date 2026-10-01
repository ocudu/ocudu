// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "flexible_o_du_metrics.h"

namespace ocudu {

/// Application service metrics for the flexible O-DU.
class flexible_o_du_app_service_metrics_impl final
{
  flexible_o_du_metrics metrics;

public:
  explicit flexible_o_du_app_service_metrics_impl(const flexible_o_du_metrics& metrics_) : metrics(metrics_) {}

  // Returns the flexible O-DU metrics.
  const flexible_o_du_metrics& get_metrics() const { return metrics; }
};

} // namespace ocudu
