// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/cu_cp/cu_cp_metrics_notifier.h"

namespace ocudu {

/// CU-CP metrics.
class cu_cp_metrics_impl final
{
  cu_cp_metrics_report metrics;

public:
  explicit cu_cp_metrics_impl(const cu_cp_metrics_report& metrics_) : metrics(metrics_) {}

  const cu_cp_metrics_report& get_metrics() const { return metrics; }
};

} // namespace ocudu
